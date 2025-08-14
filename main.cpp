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

    static auto configHook = HyprlandAPI::registerCallbackDynamic(PHANDLE, "configReloaded", [&](void* self, SCallbackInfo&, std::any data) {
        (void)self; (void)data;
        if (g_pStack3DPlugin) {
            g_pStack3DPlugin->onConfigReload();
        }
    });

    // Simple dispatcher that works without plugin object
    HyprlandAPI::addDispatcherV2(PHANDLE, "stack3d", [&](std::string arg) -> SDispatchResult {
        HyprlandAPI::addNotification(PHANDLE, "[Stack3D] Command: " + arg, 
                                     CHyprColor{1.0, 1.0, 0.0, 1.0}, 2000);
        
        if (arg == "toggle") {
            // Get current workspace windows only
            PHLWORKSPACE currentWorkspace = nullptr;
            if (g_pCompositor->m_lastMonitor && g_pCompositor->m_lastMonitor->m_activeWorkspace) {
                currentWorkspace = g_pCompositor->m_lastMonitor->m_activeWorkspace;
            }
            
            std::vector<CWindow*> workspaceWindows;
            int totalWindows = 0;
            int mappedWindows = 0;
            int filteredOut = 0;
            
            for (auto &window : g_pCompositor->m_windows) {
                totalWindows++;
                
                if (!window || !window->m_isMapped || window->isHidden()) {
                    continue;
                }
                mappedWindows++;
                
                // Be less restrictive - include floating windows too
                if (window->isFullscreen()) {
                    filteredOut++;
                    continue;
                }
                
                // Only check workspace if we have one
                if (currentWorkspace && window->m_workspace != currentWorkspace) {
                    filteredOut++;
                    continue;
                }
                
                workspaceWindows.push_back(window.get());
                if (workspaceWindows.size() >= 10) break; // Limit to 10 windows
            }
            
            // Debug info about window detection
            std::string debugMsg = "Found: " + std::to_string(totalWindows) + " total, " 
                                  + std::to_string(mappedWindows) + " mapped, " 
                                  + std::to_string(filteredOut) + " filtered, "
                                  + std::to_string(workspaceWindows.size()) + " usable";
            HyprlandAPI::addNotification(PHANDLE, debugMsg, 
                                         CHyprColor{0.0, 0.0, 1.0, 1.0}, 3000);
            
            if (workspaceWindows.empty()) {
                HyprlandAPI::addNotification(PHANDLE, "No windows to stack", 
                                             CHyprColor{1.0, 0.5, 0.0, 1.0}, 2000);
                return SDispatchResult{.success = true, .error = ""};
            }
            
            // Toggle between 3D stack and normal layout
            static bool isStackMode = false;
            isStackMode = !isStackMode;
            
            for (size_t i = 0; i < workspaceWindows.size(); ++i) {
                auto* window = workspaceWindows[i];
                
                if (isStackMode) {
                    // 3D STACK MODE: Create dramatic stacking effect
                    // Progressive transparency: front window opaque, back windows transparent
                    float alpha = (i == 0) ? 1.0f : std::max(0.3f, 1.0f - (i * 0.15f));
                    if (window->m_activeInactiveAlpha) {
                        window->m_activeInactiveAlpha->setValueAndWarp(alpha);
                    }
                    
                    // Position offset for 3D depth effect
                    if (i > 0) {
                        Vector2D currentPos = window->m_realPosition->goal();
                        Vector2D stackOffset = Vector2D(i * 40.0f, i * 30.0f); // Clear depth offsets
                        window->m_realPosition->setValueAndWarp(currentPos + stackOffset);
                    }
                    
                    // Optional: slight size reduction for back windows to enhance depth
                    if (i > 0) {
                        Vector2D currentSize = window->m_realSize->goal();
                        Vector2D scaledSize = Vector2D(currentSize.x * 0.95f, currentSize.y * 0.95f);
                        window->m_realSize->setValueAndWarp(scaledSize);
                    }
                    
                } else {
                    // NORMAL MODE: Restore all properties
                    // Restore full opacity
                    if (window->m_activeInactiveAlpha) {
                        window->m_activeInactiveAlpha->setValueAndWarp(1.0f);
                    }
                    
                    // Restore original position
                    if (i > 0) {
                        Vector2D currentPos = window->m_realPosition->goal();
                        Vector2D stackOffset = Vector2D(i * 40.0f, i * 30.0f);
                        window->m_realPosition->setValueAndWarp(currentPos - stackOffset);
                    }
                    
                    // Restore original size
                    if (i > 0) {
                        Vector2D currentSize = window->m_realSize->goal();
                        Vector2D originalSize = Vector2D(currentSize.x / 0.95f, currentSize.y / 0.95f);
                        window->m_realSize->setValueAndWarp(originalSize);
                    }
                }
            }
            
            std::string modeMsg = isStackMode ? "3D Stack Mode" : "Normal Mode";
            modeMsg += " (" + std::to_string(workspaceWindows.size()) + " windows)";
            HyprlandAPI::addNotification(PHANDLE, modeMsg, 
                                         CHyprColor{0.0, 1.0, 0.0, 1.0}, 2000);
            return SDispatchResult{.success = true, .error = ""};
            
        } else if (arg == "cycle") {
            HyprlandAPI::addNotification(PHANDLE, "Layout cycling (placeholder)", 
                                         CHyprColor{0.0, 1.0, 1.0, 1.0}, 2000);
            return SDispatchResult{.success = true, .error = ""};
            
        } else if (arg == "peek") {
            HyprlandAPI::addNotification(PHANDLE, "Peek mode (placeholder)", 
                                         CHyprColor{1.0, 0.0, 1.0, 1.0}, 2000);
            return SDispatchResult{.success = true, .error = ""};
            
        } else {
            return SDispatchResult{.success = false, .error = "Unknown command: " + arg};
        }
    });

    HyprlandAPI::addNotification(PHANDLE, "[Stack3D] Dispatcher registered", 
                                 CHyprColor{0.0, 0.0, 1.0, 1.0}, 2000);

    // Initialize plugin - try/catch to handle any errors
    try {
        g_pStack3DPlugin = std::make_unique<Stack3DPlugin>(PHANDLE);
        HyprlandAPI::addNotification(PHANDLE, "[Stack3D] Plugin object created successfully!", 
                                     CHyprColor{0.0, 1.0, 1.0, 1.0}, 2000);
        
        // Register keybinds
        g_pStack3DPlugin->registerKeybinds();
    } catch (const std::exception& e) {
        HyprlandAPI::addNotification(PHANDLE, "[Stack3D] Failed to create plugin: " + std::string(e.what()), 
                                     CHyprColor{1.0, 0.0, 0.0, 1.0}, 5000);
        // Don't throw - let dispatcher work even if plugin object fails
    }

    HyprlandAPI::addNotification(PHANDLE, "[Stack3D] Plugin loaded successfully!", 
                                 CHyprColor{0.2, 1.0, 0.2, 1.0}, 3000);

    return {"Stack3D Animation", 
            "3D stack sliding animations for windows", 
            "Stack3D Team", 
            "1.0.0"};
}

APICALL EXPORT void pluginExit() {
    if (g_pStack3DPlugin) {
        g_pStack3DPlugin.reset();
    }
}

} // extern "C"