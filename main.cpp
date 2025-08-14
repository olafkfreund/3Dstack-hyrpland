#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#include <hyprland/src/managers/AnimationManager.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

#include "include/Stack3DPlugin.hpp"

// Global plugin instance
inline HANDLE PHANDLE = nullptr;
std::unique_ptr<Stack3DPlugin> g_pStack3DPlugin;

// Use C linkage for plugin functions to ensure correct symbol names
extern "C" {

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
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

    HyprlandAPI::registerCallbackDynamic(PHANDLE, "configReloaded", [&](void* self, SCallbackInfo&, std::any data) {
        (void)self; (void)data;
        if (g_pStack3DPlugin) {
            g_pStack3DPlugin->onConfigReload();
        }
    });

    // Initialize plugin
    g_pStack3DPlugin = std::make_unique<Stack3DPlugin>(PHANDLE);
    
    // Register dispatcher following official plugin pattern
    HyprlandAPI::addDispatcher(PHANDLE, "stack3d", [&](const std::string& arg) {
        if (!g_pStack3DPlugin) return;
        
        if (arg == "toggle") {
            g_pStack3DPlugin->toggleState();
        } else if (arg == "cycle") {
            g_pStack3DPlugin->cycleLayoutType();
        } else if (arg == "peek") {
            g_pStack3DPlugin->temporaryPeek(2.0f); // 2 second peek
        }
    });
    
    // Register keybinds
    g_pStack3DPlugin->registerKeybinds();

    HyprlandAPI::addNotification(PHANDLE, "[Stack3D] Plugin loaded successfully!", 
                                 CHyprColor{0.2, 1.0, 0.2, 1.0}, 3000);

    return {"Stack3D Animation", 
            "3D stack sliding animations for windows", 
            "Stack3D Team", 
            "1.0.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    if (g_pStack3DPlugin) {
        g_pStack3DPlugin.reset();
    }
}

} // extern "C"