# Stack3DPlugin API Reference

The `Stack3DPlugin` class is the main coordinator for the 3D stack animation plugin. It manages state transitions, handles events, and orchestrates the interaction between all other components.

## 🏗️ Class Overview

```cpp
class Stack3DPlugin {
private:
    HANDLE m_handle;
    StackState m_currentState;
    PluginConfig m_config;
    
    std::unique_ptr<LayoutCalculator> m_layoutCalculator;
    std::unique_ptr<AnimationSystem> m_animationSystem;
    std::unique_ptr<PhysicsMotion> m_physicsMotion;
    
    std::vector<CWindow*> m_managedWindows;
    std::chrono::steady_clock::time_point m_lastTransition;
    
    // Event hooks
    SP<HOOK_CALLBACK_FN> m_windowOpenHook;
    SP<HOOK_CALLBACK_FN> m_windowCloseHook;
    SP<HOOK_CALLBACK_FN> m_windowFocusHook;
    SP<HOOK_CALLBACK_FN> m_workspaceChangeHook;

public:
    explicit Stack3DPlugin(HANDLE handle);
    ~Stack3DPlugin();
    
    // Core functionality
    void toggleState();
    void transitionToState(StackState newState);
    void cycleLayoutType();
    void temporaryPeek(float duration);
    
    // Configuration
    void loadConfig();
    void onConfigReload();
    
    // Event handlers
    void onWindowOpen(CWindow* window);
    void onWindowClose(CWindow* window);
    void onWindowFocus(CWindow* window);
    void onWorkspaceChange();
    
    // Keybinds
    void registerKeybinds();
    
    // Getters
    StackState getCurrentState() const;
    const PluginConfig& getConfig() const;
    
    // Window management
    std::vector<CWindow*> getCurrentWorkspaceWindows();
    void updateManagedWindows();
    
private:
    void initializeHooks();
    void cleanupHooks();
    bool isTransitionAllowed();
};
```

## 🚀 Constructor & Destructor

### Constructor

```cpp
explicit Stack3DPlugin(HANDLE handle);
```

**Parameters:**
- `handle` - Hyprland plugin handle for API calls

**Description:**
Initializes the plugin with the provided Hyprland handle. Sets up all subsystems including layout calculator, animation system, and physics motion. Loads configuration and initializes event hooks.

**Example:**
```cpp
// Initialize plugin
std::unique_ptr<Stack3DPlugin> plugin;
plugin = std::make_unique<Stack3DPlugin>(handle);
```

### Destructor

```cpp
~Stack3DPlugin();
```

**Description:**
Cleans up all resources, removes event hooks, and properly shuts down all subsystems.

## 🎯 Core Functionality

### toggleState()

```cpp
void toggleState();
```

**Description:**
Toggles between `STACKED_3D` and `SPREAD_LAYOUT` states. Ignores calls when plugin is disabled or in transition.

**Example:**
```cpp
// Toggle between stack and spread modes
plugin->toggleState();
```

### transitionToState()

```cpp
void transitionToState(StackState newState);
```

**Parameters:**
- `newState` - Target state to transition to

**Description:**
Initiates a state transition with full animation. Calculates current and target layouts, then starts the animation system.

**States:**
- `StackState::STACKED_3D` - Windows stacked in 3D perspective
- `StackState::SPREAD_LAYOUT` - Windows in spread layout
- `StackState::TRANSITIONING` - Animation in progress (automatic)

**Example:**
```cpp
// Force transition to 3D stack mode
plugin->transitionToState(StackState::STACKED_3D);

// Force transition to spread layout
plugin->transitionToState(StackState::SPREAD_LAYOUT);
```

### cycleLayoutType()

```cpp
void cycleLayoutType();
```

**Description:**
Cycles through all available layout types (Grid → Circular → Spiral → Fibonacci → Grid). If currently in spread mode, immediately re-transitions with the new layout.

**Example:**
```cpp
// Cycle to next layout type
plugin->cycleLayoutType();
```

### temporaryPeek()

```cpp
void temporaryPeek(float duration);
```

**Parameters:**
- `duration` - Duration in seconds for the peek effect

**Description:**
Shows a temporary preview of spread layout while in stack mode, then returns to stack. Only works when in `STACKED_3D` state.

**Example:**
```cpp
// Show 1-second peek of spread layout
plugin->temporaryPeek(1.0f);
```

## ⚙️ Configuration

### loadConfig()

```cpp
void loadConfig();
```

**Description:**
Loads configuration from Hyprland's config system. Called automatically during initialization and config reloads.

**Configuration Keys:**
- `plugin:stack3d:enable`
- `plugin:stack3d:transition_duration`
- `plugin:stack3d:stagger_delay`
- `plugin:stack3d:transition_style`
- `plugin:stack3d:stack_depth_step`
- `plugin:stack3d:spread_padding`
- `plugin:stack3d:default_layout`
- `plugin:stack3d:spring_strength`
- `plugin:stack3d:damping`
- `plugin:stack3d:motion_blur`
- `plugin:stack3d:perspective`
- `plugin:stack3d:eye_distance`

### onConfigReload()

```cpp
void onConfigReload();
```

**Description:**
Called when Hyprland configuration is reloaded. Updates all subsystems with new configuration values.

**Example:**
```cpp
// Manually trigger config reload
plugin->onConfigReload();
```

## 🎪 Event Handlers

### onWindowOpen()

```cpp
void onWindowOpen(CWindow* window);
```

**Parameters:**
- `window` - Newly opened window

**Description:**
Handles new window creation. Updates managed windows list and optionally triggers re-layout.

### onWindowClose()

```cpp
void onWindowClose(CWindow* window);
```

**Parameters:**
- `window` - Window being closed

**Description:**
Handles window closure. Removes window from physics system and updates managed windows.

### onWindowFocus()

```cpp
void onWindowFocus(CWindow* window);
```

**Parameters:**
- `window` - Newly focused window

**Description:**
Handles window focus changes. Can optionally bring focused window to front in stack mode.

### onWorkspaceChange()

```cpp
void onWorkspaceChange();
```

**Description:**
Handles workspace switching. Updates managed windows and resets to spread layout.

## 🎹 Keybindings

### registerKeybinds()

```cpp
void registerKeybinds();
```

**Description:**
Registers all plugin keybindings with Hyprland based on current configuration.

**Default Keybindings:**
- `SUPER + grave` - Toggle stack/spread
- `SUPER + space` - Temporary peek
- `SUPER + SHIFT + grave` - Cycle layouts

**Example:**
```cpp
// Register keybindings
plugin->registerKeybinds();
```

## 📊 State Queries

### getCurrentState()

```cpp
StackState getCurrentState() const;
```

**Returns:**
- Current plugin state

**Example:**
```cpp
if (plugin->getCurrentState() == StackState::STACKED_3D) {
    // Handle stack mode
}
```

### getConfig()

```cpp
const PluginConfig& getConfig() const;
```

**Returns:**
- Reference to current configuration

**Example:**
```cpp
const auto& config = plugin->getConfig();
std::cout << "Duration: " << config.transitionDuration << std::endl;
```

## 🪟 Window Management

### getCurrentWorkspaceWindows()

```cpp
std::vector<CWindow*> getCurrentWorkspaceWindows();
```

**Returns:**
- Vector of windows in current workspace

**Description:**
Gets all non-floating, mapped windows in the current workspace that should be managed by the plugin.

**Filter Criteria:**
- `window->m_bIsMapped == true`
- `window->isHidden() == false`
- `window->m_bIsFloating == false`
- `window->m_iWorkspaceID == current_workspace`

### updateManagedWindows()

```cpp
void updateManagedWindows();
```

**Description:**
Refreshes the list of managed windows. Called automatically on window and workspace events.

## 🔒 Private Methods

### initializeHooks()

```cpp
void initializeHooks();
```

**Description:**
Sets up Hyprland event hooks for window and workspace events.

### cleanupHooks()

```cpp
void cleanupHooks();
```

**Description:**
Removes all registered event hooks. Called automatically in destructor.

### isTransitionAllowed()

```cpp
bool isTransitionAllowed();
```

**Returns:**
- `true` if a transition can be started

**Description:**
Prevents rapid state changes by enforcing a minimum time between transitions (100ms).

## 🎨 Usage Examples

### Basic Plugin Setup

```cpp
#include "Stack3DPlugin.hpp"

// Global plugin instance
std::unique_ptr<Stack3DPlugin> g_pStack3DPlugin;

// Plugin initialization
APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    // Initialize plugin
    g_pStack3DPlugin = std::make_unique<Stack3DPlugin>(handle);
    
    // Register keybinds
    g_pStack3DPlugin->registerKeybinds();

    // Success notification
    HyprlandAPI::addNotification(handle, 
        "[stack3d] Plugin loaded successfully!", 
        CColor{0.2f, 0.8f, 0.3f, 1.0f}, 3000);

    return {"Stack3D Animation", 
            "3D stack sliding animations", 
            "Stack3D Team", 
            "1.0.0"};
}
```

### Custom State Management

```cpp
class CustomController {
private:
    Stack3DPlugin* m_plugin;
    
public:
    void smartToggle() {
        // Custom logic for toggling
        if (m_plugin->getCurrentState() == StackState::STACKED_3D) {
            // Show peek first, then transition
            m_plugin->temporaryPeek(0.5f);
            std::this_thread::sleep_for(std::chrono::milliseconds(600));
            m_plugin->transitionToState(StackState::SPREAD_LAYOUT);
        } else {
            m_plugin->transitionToState(StackState::STACKED_3D);
        }
    }
    
    void cycleWithNotification() {
        m_plugin->cycleLayoutType();
        
        // Show current layout
        const auto& config = m_plugin->getConfig();
        std::string layoutName = getLayoutName(config.defaultLayout);
        
        HyprlandAPI::addNotification(m_handle,
            "Layout: " + layoutName,
            CColor{0.3f, 0.7f, 1.0f, 1.0f}, 1500);
    }
};
```

### Event-Driven Behavior

```cpp
class EventHandler {
public:
    void onWorkspaceSwitch(int workspaceId) {
        // Automatically enter spread mode on workspace switch
        if (g_pStack3DPlugin) {
            g_pStack3DPlugin->transitionToState(StackState::SPREAD_LAYOUT);
        }
    }
    
    void onWindowCount(int count) {
        // Auto-layout based on window count
        if (count <= 2) {
            // Use circular for few windows
            // Custom implementation to change layout
        } else if (count >= 8) {
            // Use grid for many windows
        }
    }
};
```

## ⚠️ Important Notes

### Thread Safety
- The plugin is designed for single-threaded use within Hyprland's main thread
- All methods should be called from Hyprland's event loop
- Animation updates happen automatically

### Memory Management
- Plugin automatically manages all internal resources
- Windows are referenced by pointer but not owned
- Proper cleanup happens in destructor

### Performance Considerations
- State transitions are rate-limited (100ms minimum)
- Animation updates are frame-synced with Hyprland
- Physics calculations are optimized for real-time performance

### Error Handling
- Invalid configuration values fall back to defaults
- Missing windows are safely ignored
- State transitions are atomic (all-or-nothing)

## 📚 Related APIs

- [LayoutCalculator API](./LayoutCalculator.md) - Layout calculation system
- [AnimationSystem API](./AnimationSystem.md) - Animation management
- [PhysicsMotion API](./PhysicsMotion.md) - Physics simulation
- [Configuration Guide](../configuration.md) - Configuration options

---

For complete integration examples, see [Examples Directory](../examples/).