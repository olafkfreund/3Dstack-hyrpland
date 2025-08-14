#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#include <hyprland/src/managers/AnimationManager.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/SharedDefs.hpp>

#include "include/Stack3DPlugin.hpp"

// Global plugin instance
inline HANDLE PHANDLE = nullptr;
std::unique_ptr<Stack3DPlugin> g_pStack3DPlugin;

// Use C linkage for plugin functions to ensure correct symbol names
extern "C" {

APICALL EXPORT std::string pluginAPIVersion() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO pluginInit(HANDLE handle) {
    PHANDLE = handle;
    
    const std::string HASH = __hyprland_api_get_hash();
    
    // ALWAYS add this to your plugins.
    // It will prevent random crashes coming from
    // mismatched header versions.
    if (HASH != GIT_COMMIT_HASH) {
        HyprlandAPI::addNotification(PHANDLE, "[Stack3D] Mismatched headers! Can't proceed.", 
                                   CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[Stack3D] Version mismatch");
    }

    // Register configuration values with defaults (following official plugin pattern)
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:enabled", Hyprlang::INT{1});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:transition_duration", Hyprlang::FLOAT{0.8});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:stagger_delay", Hyprlang::FLOAT{0.05});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:transition_style", Hyprlang::INT{0}); // 0 = SMOOTH_SLIDE
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:stack_depth_step", Hyprlang::FLOAT{100.0});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:spread_padding", Hyprlang::FLOAT{20.0});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:default_layout", Hyprlang::INT{0}); // 0 = GRID
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:spring_strength", Hyprlang::FLOAT{0.8});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:damping", Hyprlang::FLOAT{0.92});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:motion_blur", Hyprlang::INT{1});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:perspective", Hyprlang::FLOAT{800.0});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:eye_distance", Hyprlang::FLOAT{1000.0});

    // Skip config hook for minimal test
    // static auto configHook = HyprlandAPI::registerCallbackDynamic(PHANDLE, "configReloaded", [&](void* self, SCallbackInfo&, std::any data) {
    //     (void)self; (void)data;
    //     if (g_pStack3DPlugin) {
    //         g_pStack3DPlugin->onConfigReload();
    //     }
    // });

    // State tracking for 3D stack mode
    static bool isStackMode = false;
    static int currentFrontWindow = 0;
    static std::vector<Vector2D> originalPositions;
    static std::vector<Vector2D> originalSizes;

    // Full implementation dispatcher
    HyprlandAPI::addDispatcherV2(PHANDLE, "stack3d", [](std::string arg) -> SDispatchResult {
        HyprlandAPI::addNotification(PHANDLE, "[3DStack] Command: " + arg, 
                                     CHyprColor{0.0, 1.0, 0.0, 1.0}, 2000);
        if (arg == "toggle") {
            // Get current workspace windows (simplified for debugging)
            std::vector<CWindow*> workspaceWindows;
            
            for (auto &window : g_pCompositor->m_windows) {
                if (!window || !window->m_isMapped || window->isHidden() || window->isFullscreen()) {
                    continue;
                }
                // For now, include all mapped windows to test basic functionality
                workspaceWindows.push_back(window.get());
            }
            
            HyprlandAPI::addNotification(PHANDLE, "Found " + std::to_string(workspaceWindows.size()) + " windows", 
                                         CHyprColor{0.0, 0.5, 1.0, 1.0}, 2000);
            
            if (workspaceWindows.empty()) {
                HyprlandAPI::addNotification(PHANDLE, "No windows to stack", 
                                             CHyprColor{1.0, 0.5, 0.0, 1.0}, 2000);
                return SDispatchResult{.success = true, .error = ""};
            }
            
            // Get screen center position
            Vector2D screenCenter;
            if (g_pCompositor->m_lastMonitor) {
                screenCenter = Vector2D(
                    g_pCompositor->m_lastMonitor->m_transformedSize.x / 2,
                    g_pCompositor->m_lastMonitor->m_transformedSize.y / 2
                );
            } else {
                screenCenter = Vector2D(960, 540); // Fallback center
            }
            
            // Toggle between 3D stack and normal layout
            isStackMode = !isStackMode;
            
            if (isStackMode) {
                // ENTERING 3D STACK MODE
                // Store original positions and sizes
                originalPositions.clear();
                originalSizes.clear();
                for (auto* window : workspaceWindows) {
                    originalPositions.push_back(window->m_realPosition->goal());
                    originalSizes.push_back(window->m_realSize->goal());
                }
                
                // Calculate stacks (max 6 windows per stack)
                const int windowsPerStack = 6;
                const int numStacks = (workspaceWindows.size() + windowsPerStack - 1) / windowsPerStack;
                const float stackSpacing = 400.0f; // Distance between stacks
                
                for (size_t i = 0; i < workspaceWindows.size(); ++i) {
                    auto* window = workspaceWindows[i];
                    
                    // Determine which stack this window belongs to
                    int stackIndex = i / windowsPerStack;
                    int positionInStack = i % windowsPerStack;
                    
                    // Calculate base position for this stack (centered around screen center)
                    Vector2D stackCenter = Vector2D(
                        screenCenter.x + (stackIndex - (numStacks - 1) / 2.0f) * stackSpacing,
                        screenCenter.y
                    );
                    
                    // 3D Stack Effect: Move all windows to stack center with left-offset depth
                    Vector2D windowSize = Vector2D(800, 600); // Standard window size in stack
                    
                    // Position with 3D depth effect (slight left offset for each layer)
                    Vector2D stackPos = Vector2D(
                        stackCenter.x - windowSize.x/2 - (positionInStack * 20.0f), // Left offset for depth
                        stackCenter.y - windowSize.y/2 - (positionInStack * 15.0f)  // Slight up offset
                    );
                    
                    // Apply position and size
                    window->m_realPosition->setValueAndWarp(stackPos);
                    window->m_realSize->setValueAndWarp(windowSize);
                    
                    // Apply 3D depth effect with transparency
                    // Front window (position 0) is always fully opaque, others fade based on depth
                    float alpha = (positionInStack == 0) ? 1.0f : 
                                  std::max(0.4f, 1.0f - (positionInStack * 0.15f));
                    if (window->m_activeInactiveAlpha) {
                        window->m_activeInactiveAlpha->setValueAndWarp(alpha);
                    }
                }
                
                HyprlandAPI::addNotification(PHANDLE, 
                    "3D Stack Mode: " + std::to_string(workspaceWindows.size()) + " windows in " + 
                    std::to_string(numStacks) + " stacks", 
                    CHyprColor{0.0, 1.0, 0.0, 1.0}, 3000);
                    
            } else {
                // EXITING 3D STACK MODE - Restore original positions
                for (size_t i = 0; i < workspaceWindows.size() && i < originalPositions.size(); ++i) {
                    auto* window = workspaceWindows[i];
                    
                    // Restore original position and size
                    window->m_realPosition->setValueAndWarp(originalPositions[i]);
                    if (i < originalSizes.size()) {
                        window->m_realSize->setValueAndWarp(originalSizes[i]);
                    }
                    
                    // Restore full opacity
                    if (window->m_activeInactiveAlpha) {
                        window->m_activeInactiveAlpha->setValueAndWarp(1.0f);
                    }
                }
                
                HyprlandAPI::addNotification(PHANDLE, 
                    "Normal Mode: Windows restored to original positions", 
                    CHyprColor{0.0, 1.0, 0.0, 1.0}, 2000);
            }
            
            return SDispatchResult{.success = true, .error = ""};
            
        } else if (arg == "cycle") {
            // Get current workspace windows
            std::vector<CWindow*> workspaceWindows;
            for (auto &window : g_pCompositor->m_windows) {
                if (!window || !window->m_isMapped || window->isHidden() || window->isFullscreen()) {
                    continue;
                }
                workspaceWindows.push_back(window.get());
            }
            
            if (workspaceWindows.empty()) {
                HyprlandAPI::addNotification(PHANDLE, "No windows to cycle", 
                                             CHyprColor{1.0, 0.5, 0.0, 1.0}, 2000);
                return SDispatchResult{.success = true, .error = ""};
            }
            
            // Only allow cycling when in stack mode
            if (!isStackMode) {
                HyprlandAPI::addNotification(PHANDLE, "Must be in 3D stack mode to cycle windows", 
                                             CHyprColor{1.0, 0.5, 0.0, 1.0}, 2000);
                return SDispatchResult{.success = true, .error = ""};
            }
            
            // Advance to next front window
            const int windowsPerStack = 6;
            currentFrontWindow = (currentFrontWindow + 1) % windowsPerStack;
            
            // Update window transparencies to bring new window to front
            for (size_t i = 0; i < workspaceWindows.size(); ++i) {
                auto* window = workspaceWindows[i];
                int positionInStack = i % windowsPerStack;
                
                // Set alpha: current front window is fully opaque, others fade based on depth
                float alpha = (positionInStack == currentFrontWindow) ? 1.0f : 
                              std::max(0.4f, 1.0f - (positionInStack * 0.15f));
                              
                if (window->m_activeInactiveAlpha) {
                    window->m_activeInactiveAlpha->setValueAndWarp(alpha);
                }
            }
            
            HyprlandAPI::addNotification(PHANDLE, 
                "Cycled to window layer " + std::to_string(currentFrontWindow), 
                CHyprColor{0.0, 1.0, 1.0, 1.0}, 1500);
            return SDispatchResult{.success = true, .error = ""};
            
        } else {
            HyprlandAPI::addNotification(PHANDLE, "Unknown command: " + arg, 
                                         CHyprColor{1.0, 0.5, 0.0, 1.0}, 2000);
        }
        
        return SDispatchResult{.success = true, .error = ""};
    });

    HyprlandAPI::addNotification(PHANDLE, "[Stack3D] Plugin loaded successfully!", 
                                 CHyprColor{0.2, 1.0, 0.2, 1.0}, 3000);

    return {"Hyprland 3D Stack", 
            "3D window stacking plugin", 
            "3D Stack Team", 
            "1.0.0"};
}

APICALL EXPORT void pluginExit() {
    if (g_pStack3DPlugin) {
        g_pStack3DPlugin.reset();
    }
}

} // extern "C"