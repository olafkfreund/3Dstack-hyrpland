// Minimal test plugin to check API compatibility
#include <src/plugins/PluginAPI.hpp>

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    HyprlandAPI::addNotification(handle, "[minimal-test] Plugin loaded!", CHyprColor{1.0f, 0.0f, 0.0f, 1.0f}, 3000);
    
    HyprlandAPI::addDispatcher(handle, "minimal_test", [](std::string arg) {
        // Do nothing, just test if dispatcher registration works
    });
    
    return {"Minimal Test", "Test plugin for API compatibility", "Test", "1.0.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    // Clean exit
}