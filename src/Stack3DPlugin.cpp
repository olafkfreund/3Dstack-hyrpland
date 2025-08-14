#include "../include/Stack3DPlugin.hpp"
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#include <hyprland/src/Compositor.hpp>

Stack3DPlugin::Stack3DPlugin(HANDLE handle)
    : m_handle(handle),
    m_currentState(StackState::SPREAD_LAYOUT) {
    loadConfig();

    m_layoutCalculator = std::make_unique<LayoutCalculator>(m_config.perspective,
                                                            m_config.eyeDistance,
                                                            m_config.stackDepthStep,
                                                            m_config.spreadPadding);

    m_animationSystem = std::make_unique<AnimationSystem>(m_config.transitionDuration,
                                                          m_config.staggerDelay);

    m_physicsMotion = std::make_unique<PhysicsMotion>(m_config.springStrength,
                                                      m_config.damping);

    // Initialize timestamps
    auto now = std::chrono::steady_clock::now();
    m_lastTransition = now;
    m_lastWindowUpdate = now;

    initializeHooks();
    updateManagedWindows();
}

Stack3DPlugin::~Stack3DPlugin() {
    cleanupHooks();
}

void Stack3DPlugin::loadConfig() {
    // Use simple approach to avoid static_cast issues with API changes
    try {
        const auto* enabledPtr = HyprlandAPI::getConfigValue(m_handle, "plugin:stack3d:enabled");
        if (enabledPtr && enabledPtr->getDataStaticPtr()) {
            m_config.enabled = *reinterpret_cast<const Hyprlang::INT*>(enabledPtr->getDataStaticPtr());
        }
        
        const auto* durationPtr = HyprlandAPI::getConfigValue(m_handle, "plugin:stack3d:transition_duration");
        if (durationPtr && durationPtr->getDataStaticPtr()) {
            m_config.transitionDuration = *reinterpret_cast<const Hyprlang::FLOAT*>(durationPtr->getDataStaticPtr());
        }
        
        const auto* staggerPtr = HyprlandAPI::getConfigValue(m_handle, "plugin:stack3d:stagger_delay");
        if (staggerPtr && staggerPtr->getDataStaticPtr()) {
            m_config.staggerDelay = *reinterpret_cast<const Hyprlang::FLOAT*>(staggerPtr->getDataStaticPtr());
        }
        
        const auto* stylePtr = HyprlandAPI::getConfigValue(m_handle, "plugin:stack3d:transition_style");
        if (stylePtr && stylePtr->getDataStaticPtr()) {
            m_config.transitionStyle = static_cast<TransitionStyle>(*reinterpret_cast<const Hyprlang::INT*>(stylePtr->getDataStaticPtr()));
        }
        
        const auto* depthPtr = HyprlandAPI::getConfigValue(m_handle, "plugin:stack3d:stack_depth_step");
        if (depthPtr && depthPtr->getDataStaticPtr()) {
            m_config.stackDepthStep = *reinterpret_cast<const Hyprlang::FLOAT*>(depthPtr->getDataStaticPtr());
        }
        
        const auto* paddingPtr = HyprlandAPI::getConfigValue(m_handle, "plugin:stack3d:spread_padding");
        if (paddingPtr && paddingPtr->getDataStaticPtr()) {
            m_config.spreadPadding = *reinterpret_cast<const Hyprlang::FLOAT*>(paddingPtr->getDataStaticPtr());
        }
        
        const auto* layoutPtr = HyprlandAPI::getConfigValue(m_handle, "plugin:stack3d:default_layout");
        if (layoutPtr && layoutPtr->getDataStaticPtr()) {
            m_config.defaultLayout = static_cast<LayoutType>(*reinterpret_cast<const Hyprlang::INT*>(layoutPtr->getDataStaticPtr()));
        }
        
        const auto* springPtr = HyprlandAPI::getConfigValue(m_handle, "plugin:stack3d:spring_strength");
        if (springPtr && springPtr->getDataStaticPtr()) {
            m_config.springStrength = *reinterpret_cast<const Hyprlang::FLOAT*>(springPtr->getDataStaticPtr());
        }
        
        const auto* dampingPtr = HyprlandAPI::getConfigValue(m_handle, "plugin:stack3d:damping");
        if (dampingPtr && dampingPtr->getDataStaticPtr()) {
            m_config.damping = *reinterpret_cast<const Hyprlang::FLOAT*>(dampingPtr->getDataStaticPtr());
        }
        
        const auto* blurPtr = HyprlandAPI::getConfigValue(m_handle, "plugin:stack3d:motion_blur");
        if (blurPtr && blurPtr->getDataStaticPtr()) {
            m_config.motionBlur = *reinterpret_cast<const Hyprlang::INT*>(blurPtr->getDataStaticPtr());
        }
        
        const auto* perspPtr = HyprlandAPI::getConfigValue(m_handle, "plugin:stack3d:perspective");
        if (perspPtr && perspPtr->getDataStaticPtr()) {
            m_config.perspective = *reinterpret_cast<const Hyprlang::FLOAT*>(perspPtr->getDataStaticPtr());
        }
        
        const auto* eyePtr = HyprlandAPI::getConfigValue(m_handle, "plugin:stack3d:eye_distance");
        if (eyePtr && eyePtr->getDataStaticPtr()) {
            m_config.eyeDistance = *reinterpret_cast<const Hyprlang::FLOAT*>(eyePtr->getDataStaticPtr());
        }
    } catch (...) {
        // Use default config values on any error
    }
} // Stack3DPlugin::loadConfig

void Stack3DPlugin::toggleState() {
    if (!m_config.enabled || !isTransitionAllowed())
    {
        return;
    }

    StackState newState = (m_currentState == StackState::STACKED_3D) ?
                          StackState::SPREAD_LAYOUT : StackState::STACKED_3D;

    transitionToState(newState);
}

void Stack3DPlugin::transitionToState(StackState newState) {
    if (m_currentState == newState || m_currentState == StackState::TRANSITIONING)
    {
        return;
    }

    updateManagedWindows();
    if (m_managedWindows.empty())
    {
        return;
    }

    // Calculate current and target layouts
    std::vector<WindowLayout> currentLayouts;
    std::vector<WindowLayout> targetLayouts;

    if (m_currentState == StackState::STACKED_3D)
    {
        currentLayouts = m_layoutCalculator->calculateStackLayout(m_managedWindows);
    }
    else
    {
        currentLayouts = m_layoutCalculator->calculateSpreadLayout(m_managedWindows,
                                                                   m_config.defaultLayout);
    }

    if (newState == StackState::STACKED_3D)
    {
        targetLayouts = m_layoutCalculator->calculateStackLayout(m_managedWindows);
    }
    else
    {
        targetLayouts = m_layoutCalculator->calculateSpreadLayout(m_managedWindows,
                                                                  m_config.defaultLayout);
    }

    // Start transition
    m_currentState = StackState::TRANSITIONING;
    m_lastTransition = std::chrono::steady_clock::now();

    m_animationSystem->startTransition(currentLayouts,
                                       targetLayouts,
                                       m_config.transitionStyle,
                                       [this, newState]()
    {
        m_currentState = newState;
        HyprlandAPI::addNotification(m_handle,
                                     newState ==
                                     StackState::STACKED_3D ? "Entered 3D Stack Mode" : "Entered Spread Mode",
                                     CHyprColor(),
                                     1500);
    });
} // Stack3DPlugin::transitionToState

void Stack3DPlugin::registerKeybinds() {
    // Note: registerKeybinds method exists but dispatcher is registered in main.cpp
    // to avoid deprecated API warnings in multiple places
}

std::vector<CWindow *> Stack3DPlugin::getCurrentWorkspaceWindows() {
    std::vector<CWindow *> windows;
    windows.reserve(16); // Reserve space to avoid reallocations

    // Simplified and efficient window filtering
    for (auto &window : g_pCompositor->m_windows)
    {
        // Quick elimination checks first (most likely to fail)
        if (!window || !window->m_isMapped || window->isHidden() || window->m_isFloating)
        {
            continue;
        }

        windows.push_back(window.get());

        // Limit to reasonable number to prevent excessive processing
        if (windows.size() >= 32)
        {
            break;
        }
    }

    return windows;
}

void Stack3DPlugin::updateManagedWindows() {
    // Rate limit expensive window updates to prevent performance issues
    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastWindowUpdate).count();

    // Only update window list every 100ms max to prevent excessive polling
    if (timeSinceLastUpdate < 100)
    {
        return;
    }

    m_lastWindowUpdate = now;
    m_managedWindows = getCurrentWorkspaceWindows();
}

bool Stack3DPlugin::isTransitionAllowed() {
    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastTransition = std::chrono::duration_cast<std::chrono::milliseconds>(now -
                                                                                         m_lastTransition).count();

    return timeSinceLastTransition > 100; // Minimum 100ms between transitions
}

void Stack3DPlugin::initializeHooks() {
    m_windowOpenHook = HyprlandAPI::registerCallbackDynamic(m_handle,
                                                            "openWindow",
                                                            [this](void *, SCallbackInfo &, std::any data)
    {
        auto *window = std::any_cast<CWindow *>(data);
        onWindowOpen(window);
    });

    m_windowCloseHook = HyprlandAPI::registerCallbackDynamic(m_handle,
                                                             "closeWindow",
                                                             [this](void *, SCallbackInfo &, std::any data)
    {
        auto *window = std::any_cast<CWindow *>(data);
        onWindowClose(window);
    });

    m_windowFocusHook = HyprlandAPI::registerCallbackDynamic(m_handle,
                                                             "activeWindow",
                                                             [this](void *, SCallbackInfo &, std::any data)
    {
        auto *window = std::any_cast<CWindow *>(data);
        onWindowFocus(window);
    });

    m_workspaceChangeHook = HyprlandAPI::registerCallbackDynamic(m_handle,
                                                                 "workspace",
                                                                 [this](void *, SCallbackInfo &, std::any)
    {
        onWorkspaceChange();
    });
}

void Stack3DPlugin::cleanupHooks() {
    // Hooks are automatically cleaned up by Hyprland when plugin unloads
}

void Stack3DPlugin::onWindowOpen(CWindow *window) {
    // Only update if it's a significant window (not floating, not special)
    if (!window || window->m_isFloating || window->isHidden())
    {
        return;
    }
    updateManagedWindows();
}

void Stack3DPlugin::onWindowClose(CWindow *window) {
    if (window)
    {
        m_physicsMotion->removeWindow(window);
    }
    updateManagedWindows();
}

void Stack3DPlugin::onWindowFocus(CWindow *window) {
    // Optionally bring focused window to front in stack mode
    (void)window; // Suppress unused parameter warning
}

void Stack3DPlugin::onWorkspaceChange() {
    updateManagedWindows();
    // Reset to spread layout on workspace change
    if (m_currentState == StackState::STACKED_3D)
    {
        m_currentState = StackState::SPREAD_LAYOUT;
    }
}

void Stack3DPlugin::onConfigReload() {
    loadConfig();

    // Update subsystems with new config
    m_layoutCalculator->updateTransform(m_config.perspective,
                                        m_config.eyeDistance);
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
    if (m_currentState == StackState::SPREAD_LAYOUT)
    {
        transitionToState(StackState::SPREAD_LAYOUT); // Re-transition with new layout
    }
}

void Stack3DPlugin::temporaryPeek(float duration) {
    if (m_currentState != StackState::STACKED_3D)
    {
        return;
    }

    // TODO: Implement temporary peek functionality
    // This would briefly show spread layout then return to stack
    (void)duration; // Suppress unused parameter warning
}
