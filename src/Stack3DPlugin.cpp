#include "../include/Stack3DPlugin.hpp"
#include <src/config/ConfigManager.hpp>
#include <src/managers/KeybindManager.hpp>
#include <src/Compositor.hpp>

Stack3DPlugin::Stack3DPlugin(HANDLE handle) 
    : m_handle(handle), m_currentState(StackState::SPREAD_LAYOUT) {
    
    loadConfig();
    
    m_layoutCalculator = std::make_unique<LayoutCalculator>(
        m_config.perspective, m_config.eyeDistance, 
        m_config.stackDepthStep, m_config.spreadPadding);
    
    m_animationSystem = std::make_unique<AnimationSystem>(
        m_config.transitionDuration, m_config.staggerDelay);
    
    m_physicsMotion = std::make_unique<PhysicsMotion>(
        m_config.springStrength, m_config.damping);
    
    initializeHooks();
    updateManagedWindows();
}

Stack3DPlugin::~Stack3DPlugin() {
    cleanupHooks();
}

void Stack3DPlugin::loadConfig() {
    // For now, use default configuration values
    // In a real implementation, we would load from Hyprland config
    m_config.enabled = true;
    m_config.transitionDuration = 0.8f;
    m_config.staggerDelay = 0.05f;
    m_config.stackDepthStep = 100.0f;
    m_config.spreadPadding = 20.0f;
    m_config.springStrength = 0.8f;
    m_config.damping = 0.92f;
    m_config.motionBlur = true;
    m_config.perspective = 800.0f;
    m_config.eyeDistance = 1000.0f;
    m_config.transitionStyle = TransitionStyle::SMOOTH_SLIDE;
    m_config.defaultLayout = LayoutType::GRID;
}

void Stack3DPlugin::toggleState() {
    if (!m_config.enabled || !isTransitionAllowed()) return;
    
    StackState newState = (m_currentState == StackState::STACKED_3D) ? 
                         StackState::SPREAD_LAYOUT : StackState::STACKED_3D;
    
    transitionToState(newState);
}

void Stack3DPlugin::transitionToState(StackState newState) {
    if (m_currentState == newState || m_currentState == StackState::TRANSITIONING) return;
    
    updateManagedWindows();
    if (m_managedWindows.empty()) return;
    
    // Calculate current and target layouts
    std::vector<WindowLayout> currentLayouts;
    std::vector<WindowLayout> targetLayouts;
    
    if (m_currentState == StackState::STACKED_3D) {
        currentLayouts = m_layoutCalculator->calculateStackLayout(m_managedWindows);
    } else {
        currentLayouts = m_layoutCalculator->calculateSpreadLayout(m_managedWindows, m_config.defaultLayout);
    }
    
    if (newState == StackState::STACKED_3D) {
        targetLayouts = m_layoutCalculator->calculateStackLayout(m_managedWindows);
    } else {
        targetLayouts = m_layoutCalculator->calculateSpreadLayout(m_managedWindows, m_config.defaultLayout);
    }
    
    // Start transition
    m_currentState = StackState::TRANSITIONING;
    m_lastTransition = std::chrono::steady_clock::now();
    
    m_animationSystem->startTransition(currentLayouts, targetLayouts, m_config.transitionStyle,
        [this, newState]() {
            m_currentState = newState;
            HyprlandAPI::addNotification(m_handle, 
                newState == StackState::STACKED_3D ? "Entered 3D Stack Mode" : "Entered Spread Mode",
                CHyprColor(), 1500);
        });
}

void Stack3DPlugin::registerKeybinds() {
    // Register dispatcher for manual testing
    HyprlandAPI::addDispatcher(m_handle, "stack3d", [this](const std::string& arg) {
        if (arg == "toggle") {
            toggleState();
        } else if (arg == "cycle") {
            cycleLayoutType();
        }
    });
}

std::vector<CWindow*> Stack3DPlugin::getCurrentWorkspaceWindows() {
    std::vector<CWindow*> windows;
    
    // For now, get all visible windows in the current workspace
    // In a real implementation, we would use proper Hyprland APIs
    for (auto& window : g_pCompositor->m_windows) {
        if (window->m_isMapped && 
            !window->isHidden() && 
            !window->m_isFloating) {
            windows.push_back(window.get());
        }
    }
    
    return windows;
}

void Stack3DPlugin::updateManagedWindows() {
    m_managedWindows = getCurrentWorkspaceWindows();
}

bool Stack3DPlugin::isTransitionAllowed() {
    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastTransition = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - m_lastTransition).count();
    
    return timeSinceLastTransition > 100; // Minimum 100ms between transitions
}

void Stack3DPlugin::initializeHooks() {
    m_windowOpenHook = HyprlandAPI::registerCallbackDynamic(m_handle, "openWindow", 
        [this](void*, SCallbackInfo&, std::any data) {
            auto* window = std::any_cast<CWindow*>(data);
            onWindowOpen(window);
        });
    
    m_windowCloseHook = HyprlandAPI::registerCallbackDynamic(m_handle, "closeWindow",
        [this](void*, SCallbackInfo&, std::any data) {
            auto* window = std::any_cast<CWindow*>(data);
            onWindowClose(window);
        });
    
    m_windowFocusHook = HyprlandAPI::registerCallbackDynamic(m_handle, "activeWindow",
        [this](void*, SCallbackInfo&, std::any data) {
            auto* window = std::any_cast<CWindow*>(data);
            onWindowFocus(window);
        });
    
    m_workspaceChangeHook = HyprlandAPI::registerCallbackDynamic(m_handle, "workspace",
        [this](void*, SCallbackInfo&, std::any) {
            onWorkspaceChange();
        });
}

void Stack3DPlugin::cleanupHooks() {
    // Hooks are automatically cleaned up by Hyprland when plugin unloads
}

void Stack3DPlugin::onWindowOpen(CWindow* window) {
    updateManagedWindows();
    // Optionally trigger re-layout if in stack mode
}

void Stack3DPlugin::onWindowClose(CWindow* window) {
    m_physicsMotion->removeWindow(window);
    updateManagedWindows();
}

void Stack3DPlugin::onWindowFocus(CWindow* window) {
    // Optionally bring focused window to front in stack mode
    (void)window; // Suppress unused parameter warning
}

void Stack3DPlugin::onWorkspaceChange() {
    updateManagedWindows();
    // Reset to spread layout on workspace change
    if (m_currentState == StackState::STACKED_3D) {
        m_currentState = StackState::SPREAD_LAYOUT;
    }
}

void Stack3DPlugin::onConfigReload() {
    loadConfig();
    
    // Update subsystems with new config
    m_layoutCalculator->updateTransform(m_config.perspective, m_config.eyeDistance);
    m_layoutCalculator->setDepthStep(m_config.stackDepthStep);
    m_layoutCalculator->setPadding(m_config.spreadPadding);
    
    m_animationSystem->setDuration(m_config.transitionDuration);
    m_animationSystem->setMaxStagger(m_config.staggerDelay);
    
    m_physicsMotion->setGlobalSpring(m_config.springStrength);
    m_physicsMotion->setGlobalDamping(m_config.damping);
}

void Stack3DPlugin::cycleLayoutType() {
    int currentLayout = static_cast<int>(m_config.defaultLayout);
    currentLayout = (currentLayout + 1) % 4; // 4 layout types
    m_config.defaultLayout = static_cast<LayoutType>(currentLayout);
    
    // If currently in spread mode, update immediately
    if (m_currentState == StackState::SPREAD_LAYOUT) {
        transitionToState(StackState::SPREAD_LAYOUT); // Re-transition with new layout
    }
}

void Stack3DPlugin::temporaryPeek(float duration) {
    if (m_currentState != StackState::STACKED_3D) return;
    
    // TODO: Implement temporary peek functionality
    // This would briefly show spread layout then return to stack
    (void)duration; // Suppress unused parameter warning
}