#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/SharedDefs.hpp>

// Global plugin handle
inline HANDLE PHANDLE = nullptr;

// Constants for 3D stack configuration
namespace Stack3DConstants {
    constexpr int WINDOWS_PER_STACK = 6;
    constexpr float STACK_SPACING = 400.0f;
    constexpr float STANDARD_WINDOW_WIDTH = 800.0f;
    constexpr float STANDARD_WINDOW_HEIGHT = 600.0f;
    constexpr float DEPTH_OFFSET_X = 20.0f;
    constexpr float DEPTH_OFFSET_Y = 15.0f;
    constexpr float TRANSPARENCY_STEP = 0.15f;
    constexpr float MIN_ALPHA = 0.4f;
    constexpr float FALLBACK_CENTER_X = 960.0f;
    constexpr float FALLBACK_CENTER_Y = 540.0f;
}

// State tracking for 3D stack mode
static bool g_isStackMode = false;
static int g_currentFrontWindow = 0;
static std::vector<Vector2D> g_originalPositions;
static std::vector<Vector2D> g_originalSizes;

// Helper function to get filtered workspace windows
std::vector<CWindow*> getWorkspaceWindows() {
    std::vector<CWindow*> workspaceWindows;
    for (auto &window : g_pCompositor->m_windows) {
        if (!window || !window->m_isMapped || window->isHidden() || window->isFullscreen()) {
            continue;
        }
        workspaceWindows.push_back(window.get());
    }
    return workspaceWindows;
}

// Helper function to calculate screen center
Vector2D getScreenCenter() {
    if (g_pCompositor->m_lastMonitor) {
        return Vector2D(
            g_pCompositor->m_lastMonitor->m_transformedSize.x / 2.0f,
            g_pCompositor->m_lastMonitor->m_transformedSize.y / 2.0f
        );
    }
    return Vector2D(Stack3DConstants::FALLBACK_CENTER_X, Stack3DConstants::FALLBACK_CENTER_Y);
}

// Helper function to calculate transparency based on position
float calculateAlpha(int positionInStack, int frontWindow) {
    if (positionInStack == frontWindow) {
        return 1.0f;
    }
    return std::max(Stack3DConstants::MIN_ALPHA, 
                   1.0f - (positionInStack * Stack3DConstants::TRANSPARENCY_STEP));
}

// Function to handle toggle command
SDispatchResult handleToggleCommand() {
    auto workspaceWindows = getWorkspaceWindows();
    
    HyprlandAPI::addNotification(PHANDLE, 
        "Found " + std::to_string(workspaceWindows.size()) + " windows", 
        CHyprColor{0.0, 0.5, 1.0, 1.0}, 2000);
    
    if (workspaceWindows.empty()) {
        HyprlandAPI::addNotification(PHANDLE, "No windows to stack", 
                                     CHyprColor{1.0, 0.5, 0.0, 1.0}, 2000);
        return SDispatchResult{.success = true, .error = ""};
    }
    
    Vector2D screenCenter = getScreenCenter();
    g_isStackMode = !g_isStackMode;
    
    if (g_isStackMode) {
        // ENTERING 3D STACK MODE
        g_originalPositions.clear();
        g_originalSizes.clear();
        
        // Store original positions and sizes
        for (auto* window : workspaceWindows) {
            g_originalPositions.push_back(window->m_realPosition->goal());
            g_originalSizes.push_back(window->m_realSize->goal());
        }
        
        // Calculate stack layout
        const int numStacks = (workspaceWindows.size() + Stack3DConstants::WINDOWS_PER_STACK - 1) / 
                             Stack3DConstants::WINDOWS_PER_STACK;
        
        for (size_t i = 0; i < workspaceWindows.size(); ++i) {
            auto* window = workspaceWindows[i];
            const int stackIndex = i / Stack3DConstants::WINDOWS_PER_STACK;
            const int positionInStack = i % Stack3DConstants::WINDOWS_PER_STACK;
            
            // Calculate stack center position
            Vector2D stackCenter = Vector2D(
                screenCenter.x + (stackIndex - (numStacks - 1) / 2.0f) * Stack3DConstants::STACK_SPACING,
                screenCenter.y
            );
            
            // Calculate window position with 3D depth effect
            Vector2D windowSize = Vector2D(Stack3DConstants::STANDARD_WINDOW_WIDTH, 
                                          Stack3DConstants::STANDARD_WINDOW_HEIGHT);
            Vector2D stackPos = Vector2D(
                stackCenter.x - windowSize.x / 2.0f - (positionInStack * Stack3DConstants::DEPTH_OFFSET_X),
                stackCenter.y - windowSize.y / 2.0f - (positionInStack * Stack3DConstants::DEPTH_OFFSET_Y)
            );
            
            // Apply transformations
            window->m_realPosition->setValueAndWarp(stackPos);
            window->m_realSize->setValueAndWarp(windowSize);
            
            // Apply transparency effect
            float alpha = calculateAlpha(positionInStack, 0);
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
        for (size_t i = 0; i < workspaceWindows.size() && i < g_originalPositions.size(); ++i) {
            auto* window = workspaceWindows[i];
            
            // Restore original position and size
            window->m_realPosition->setValueAndWarp(g_originalPositions[i]);
            if (i < g_originalSizes.size()) {
                window->m_realSize->setValueAndWarp(g_originalSizes[i]);
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
}

// Function to handle cycle command
SDispatchResult handleCycleCommand() {
    auto workspaceWindows = getWorkspaceWindows();
    
    if (workspaceWindows.empty()) {
        HyprlandAPI::addNotification(PHANDLE, "No windows to cycle", 
                                     CHyprColor{1.0, 0.5, 0.0, 1.0}, 2000);
        return SDispatchResult{.success = true, .error = ""};
    }
    
    if (!g_isStackMode) {
        HyprlandAPI::addNotification(PHANDLE, "Must be in 3D stack mode to cycle windows", 
                                     CHyprColor{1.0, 0.5, 0.0, 1.0}, 2000);
        return SDispatchResult{.success = true, .error = ""};
    }
    
    // Advance to next front window
    g_currentFrontWindow = (g_currentFrontWindow + 1) % Stack3DConstants::WINDOWS_PER_STACK;
    
    // Update window transparencies
    for (size_t i = 0; i < workspaceWindows.size(); ++i) {
        auto* window = workspaceWindows[i];
        const int positionInStack = i % Stack3DConstants::WINDOWS_PER_STACK;
        
        float alpha = calculateAlpha(positionInStack, g_currentFrontWindow);
        if (window->m_activeInactiveAlpha) {
            window->m_activeInactiveAlpha->setValueAndWarp(alpha);
        }
    }
    
    HyprlandAPI::addNotification(PHANDLE, 
        "Cycled to window layer " + std::to_string(g_currentFrontWindow), 
        CHyprColor{0.0, 1.0, 1.0, 1.0}, 1500);
    
    return SDispatchResult{.success = true, .error = ""};
}

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

    // Register configuration values with defaults
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:enabled", Hyprlang::INT{1});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:transition_duration", Hyprlang::FLOAT{0.8});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:stagger_delay", Hyprlang::FLOAT{0.05});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:transition_style", Hyprlang::INT{0});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:stack_depth_step", Hyprlang::FLOAT{100.0});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:spread_padding", Hyprlang::FLOAT{20.0});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:default_layout", Hyprlang::INT{0});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:spring_strength", Hyprlang::FLOAT{0.8});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:damping", Hyprlang::FLOAT{0.92});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:motion_blur", Hyprlang::INT{1});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:perspective", Hyprlang::FLOAT{800.0});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:stack3d:eye_distance", Hyprlang::FLOAT{1000.0});

    // Register 3D stack dispatcher
    HyprlandAPI::addDispatcherV2(PHANDLE, "stack3d", [](std::string arg) -> SDispatchResult {
        HyprlandAPI::addNotification(PHANDLE, "[3DStack] Command: " + arg, 
                                     CHyprColor{0.0, 1.0, 0.0, 1.0}, 2000);
        
        if (arg == "toggle") {
            return handleToggleCommand();
        } else if (arg == "cycle") {
            return handleCycleCommand();
        } else {
            HyprlandAPI::addNotification(PHANDLE, "Unknown command: " + arg, 
                                         CHyprColor{1.0, 0.5, 0.0, 1.0}, 2000);
            return SDispatchResult{.success = true, .error = ""};
        }
    });

    HyprlandAPI::addNotification(PHANDLE, "[Stack3D] Plugin loaded successfully!", 
                                 CHyprColor{0.2, 1.0, 0.2, 1.0}, 3000);

    return {"Hyprland 3D Stack", 
            "3D window stacking plugin", 
            "3D Stack Team", 
            "1.0.0"};
}

APICALL EXPORT void pluginExit() {
    // Plugin cleanup handled automatically by Hyprland
}

} // extern "C"