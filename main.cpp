#include <src/Compositor.hpp>
#include <src/desktop/Window.hpp>
#include <src/config/ConfigManager.hpp>
#include <src/managers/KeybindManager.hpp>
#include <src/managers/AnimationManager.hpp>
#include <src/plugins/PluginAPI.hpp>

#include "include/Stack3DPlugin.hpp"

// Global plugin instance
inline HANDLE PHANDLE = nullptr;
std::unique_ptr<Stack3DPlugin> g_pStack3DPlugin;

// Use C linkage for plugin functions to ensure correct symbol names
extern "C" {

APICALL EXPORT const char* PLUGIN_API_VERSION() {
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

    HyprlandAPI::registerCallbackDynamic(PHANDLE, "configReloaded", [&](void* self, SCallbackInfo&, std::any data) {
        (void)self; (void)data;
        if (g_pStack3DPlugin) {
            g_pStack3DPlugin->onConfigReload();
        }
    });

    // Initialize plugin
    g_pStack3DPlugin = std::make_unique<Stack3DPlugin>(PHANDLE);
    
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