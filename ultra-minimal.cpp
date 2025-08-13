#include <src/plugins/PluginAPI.hpp>

// Ultra-minimal plugin that exports required functions but makes no API calls
inline HANDLE PHANDLE = nullptr;

extern "C" {

APICALL EXPORT const char* PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;
    
    // NO Hyprland API calls at all - just return the description
    return {"Ultra Minimal", 
            "No API calls version", 
            "Test", 
            "1.0.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    // Clean exit with no API calls
}

} // extern "C"