#include <src/plugins/PluginAPI.hpp>

// Minimal plugin to test basic functionality
inline HANDLE PHANDLE = nullptr;

extern "C" {

APICALL EXPORT const char* PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;
    
    // Simple notification without hash check to isolate the issue
    HyprlandAPI::addNotification(PHANDLE, "[Stack3D] Minimal plugin loaded!", 
                                 CHyprColor{0.2, 1.0, 0.2, 1.0}, 3000);

    return {"Stack3D Minimal", 
            "Minimal test version", 
            "Stack3D Team", 
            "1.0.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    // Clean exit
}

} // extern "C"