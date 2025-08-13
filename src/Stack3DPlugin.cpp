#include "../include/Stack3DPlugin.hpp"
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#include <hyprland/src/Compositor.hpp>

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
    // Load configuration from Hyprland config
    auto* const PCONFIG = g_pConfigManager;
    
    m_config.enabled = PCONFIG->getConfigValuePtr("plugin:stack3d:enable")->intValue;
    m_config.transitionDuration = PCONFIG->getConfigValuePtr("plugin:stack3d:transition_duration")->floatValue;
    m_config.staggerDelay = PCONFIG->getConfigValuePtr("plugin:stack3d:stagger_delay")->floatValue;
    m_config.stackDepthStep = PCONFIG->getConfigValuePtr("plugin:stack3d:stack_depth_step")->floatValue;
    m_config.spreadPadding = PCONFIG->getConfigValuePtr("plugin:stack3d:spread_padding")->floatValue;
    m_config.springStrength = PCONFIG->getConfigValuePtr("plugin:stack3d:spring_strength")->floatValue;
    m_config.damping = PCONFIG->getConfigValuePtr("plugin:stack3d:damping")->floatValue;
    m_config.motionBlur = PCONFIG->getConfigValuePtr("plugin:stack3d:motion_blur")->intValue;
    m_config.perspective = PCONFIG->getConfigValuePtr("plugin:stack3d:perspective")->floatValue;
    m_config.eyeDistance = PCONFIG->getConfigValuePtr("plugin:stack3d:eye_distance")->floatValue;
    
    // Parse transition style
    std::string styleStr = PCONFIG->getConfigValuePtr("plugin:stack3d:transition_style")->strValue;
    if (styleStr == "bounce_in") m_config.transitionStyle = TransitionStyle::BOUNCE_IN;
    else if (styleStr == "elastic_out") m_config.transitionStyle = TransitionStyle::ELASTIC_OUT;
    else if (styleStr == "cascade_wave") m_config.transitionStyle = TransitionStyle::CASCADE_WAVE;
    else if (styleStr == "spiral_motion") m_config.transitionStyle = TransitionStyle::SPIRAL_MOTION;
    else if (styleStr == "magnetic_attract") m_config.transitionStyle = TransitionStyle::MAGNETIC_ATTRACT;
    else if (styleStr == "liquid_flow") m_config.transitionStyle = TransitionStyle::LIQUID_FLOW;
    else m_config.transitionStyle = TransitionStyle::SMOOTH_SLIDE;
    
    // Parse layout type
    std::string layoutStr = PCONFIG->getConfigValuePtr("plugin:stack3d:default_layout")->strValue;
    if (layoutStr == "circular") m_config.defaultLayout = LayoutType::CIRCULAR;
    else if (layoutStr == "spiral") m_config.defaultLayout = LayoutType::SPIRAL;
    else if (layoutStr == "fibonacci") m_config.defaultLayout = LayoutType::FIBONACCI;
    else m_config.defaultLayout = LayoutType::GRID;
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
                CColor{0.2f, 0.8f, 0.3f, 1.0f}, 1500);
        });
}

void Stack3DPlugin::registerKeybinds() {
    HyprlandAPI::addKeybind(m_handle, m_config.toggleKey, [this](const std::string&) {
        toggleState();
    });
    
    HyprlandAPI::addKeybind(m_handle, m_config.peekKey, [this](const std::string&) {
        if (m_currentState == StackState::STACKED_3D) {
            temporaryPeek(1.0f);
        }
    });
    
    HyprlandAPI::addKeybind(m_handle, m_config.cycleLayoutKey, [this](const std::string&) {
        cycleLayoutType();
    });
}

std::vector<CWindow*> Stack3DPlugin::getCurrentWorkspaceWindows() {
    std::vector<CWindow*> windows;
    
    auto* workspace = g_pCompositor->getWorkspaceByID(g_pCompositor->m_pLastMonitor->activeWorkspace);
    if (!workspace) return windows;
    
    for (auto& window : g_pCompositor->m_vWindows) {
        if (window->m_iWorkspaceID == workspace->m_iID && 
            window->m_bIsMapped && 
            !window->isHidden() && 
            !window->m_bIsFloating) {
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
        [this](void*, std::any data) {
            auto* window = std::any_cast<CWindow*>(data);
            onWindowOpen(window);
        });
    
    m_windowCloseHook = HyprlandAPI::registerCallbackDynamic(m_handle, "closeWindow",
        [this](void*, std::any data) {
            auto* window = std::any_cast<CWindow*>(data);
            onWindowClose(window);
        });
    
    m_windowFocusHook = HyprlandAPI::registerCallbackDynamic(m_handle, "activeWindow",
        [this](void*, std::any data) {
            auto* window = std::any_cast<CWindow*>(data);
            onWindowFocus(window);
        });
    
    m_workspaceChangeHook = HyprlandAPI::registerCallbackDynamic(m_handle, "workspace",
        [this](void*, std::any) {
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