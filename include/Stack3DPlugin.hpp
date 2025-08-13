#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

#include "LayoutCalculator.hpp"
#include "AnimationSystem.hpp"
#include "PhysicsMotion.hpp"

enum class StackState {
    STACKED_3D,
    TRANSITIONING,
    SPREAD_LAYOUT
};

struct PluginConfig {
    bool enabled = true;
    float transitionDuration = 0.8f;
    float staggerDelay = 0.05f;
    TransitionStyle transitionStyle = TransitionStyle::SMOOTH_SLIDE;
    float stackDepthStep = 100.0f;
    float spreadPadding = 20.0f;
    LayoutType defaultLayout = LayoutType::GRID;
    float springStrength = 0.8f;
    float damping = 0.92f;
    bool motionBlur = true;
    float perspective = 800.0f;
    float eyeDistance = 1000.0f;
    std::string toggleKey = "SUPER, grave";
    std::string peekKey = "SUPER, space";
    std::string cycleLayoutKey = "SUPER SHIFT, grave";
};

class Stack3DPlugin {
private:
    HANDLE m_handle;
    StackState m_currentState;
    PluginConfig m_config;
    
    std::unique_ptr<LayoutCalculator> m_layoutCalculator;
    std::unique_ptr<AnimationSystem> m_animationSystem;
    std::unique_ptr<PhysicsMotion> m_physicsMotion;
    
    std::vector<CWindow*> m_managedWindows;
    std::chrono::steady_clock::time_point m_lastTransition;
    
    // Event hooks
    SP<HOOK_CALLBACK_FN> m_windowOpenHook;
    SP<HOOK_CALLBACK_FN> m_windowCloseHook;
    SP<HOOK_CALLBACK_FN> m_windowFocusHook;
    SP<HOOK_CALLBACK_FN> m_workspaceChangeHook;

public:
    explicit Stack3DPlugin(HANDLE handle);
    ~Stack3DPlugin();
    
    // Core functionality
    void toggleState();
    void transitionToState(StackState newState);
    void cycleLayoutType();
    void temporaryPeek(float duration);
    
    // Configuration
    void loadConfig();
    void onConfigReload();
    
    // Event handlers
    void onWindowOpen(CWindow* window);
    void onWindowClose(CWindow* window);
    void onWindowFocus(CWindow* window);
    void onWorkspaceChange();
    
    // Keybinds
    void registerKeybinds();
    
    // Getters
    StackState getCurrentState() const { return m_currentState; }
    const PluginConfig& getConfig() const { return m_config; }
    
    // Window management
    std::vector<CWindow*> getCurrentWorkspaceWindows();
    void updateManagedWindows();
    
private:
    void initializeHooks();
    void cleanupHooks();
    bool isTransitionAllowed();
};