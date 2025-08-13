#include <hyprland/src/plugins/PluginAPI.hpp>

// Ultra-minimal plugin using official header path
inline HANDLE PHANDLE = nullptr;

extern "C" {

APICALL EXPORT const char* pluginAPIVersion() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO pluginInit(HANDLE handle) {
    PHANDLE = handle;
    
    // Minimal notification test
    HyprlandAPI::addNotification(PHANDLE, "[Minimal] Plugin loaded!", 
                                 CHyprColor{0.2, 1.0, 0.2, 1.0}, 3000);

    return {"Minimal Test", 
            "CMake minimal version", 
            "Test", 
            "1.0.0"};
}

APICALL EXPORT void pluginExit() {
    // Clean exit
}

} // extern "C"