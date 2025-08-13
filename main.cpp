#include <src/Compositor.hpp>
#include <src/desktop/Window.hpp>
#include <src/config/ConfigManager.hpp>
#include <src/managers/KeybindManager.hpp>
#include <src/managers/AnimationManager.hpp>
#include <src/plugins/PluginAPI.hpp>

#include "include/Stack3DPlugin.hpp"

// Global plugin instance
std::unique_ptr<Stack3DPlugin> g_pStack3DPlugin;

// Plugin metadata
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    HyprlandAPI::registerCallbackDynamic(handle, "configReloaded", [&](void* self, SCallbackInfo&, std::any data) {
        (void)self; (void)data;
        if (g_pStack3DPlugin) {
            g_pStack3DPlugin->onConfigReload();
        }
    });

    // Initialize plugin
    g_pStack3DPlugin = std::make_unique<Stack3DPlugin>(handle);
    
    // Register keybinds
    g_pStack3DPlugin->registerKeybinds();

    HyprlandAPI::addNotification(handle, "[stack3d] Plugin loaded successfully!", 
                                 CHyprColor(), 3000);

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