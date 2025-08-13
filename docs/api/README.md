# API Documentation

Complete API documentation for the Hyprland Stack3D plugin components.

## 📚 API Overview

The Stack3D plugin provides a modular C++ API with five main components:

- [**Stack3DPlugin**](./Stack3DPlugin.md) - Main plugin class and coordinator
- [**LayoutCalculator**](./LayoutCalculator.md) - Layout mathematics and 3D transformations  
- [**AnimationSystem**](./AnimationSystem.md) - Animation management and transitions
- [**PhysicsMotion**](./PhysicsMotion.md) - Physics-based motion calculations
- [**BezierCurve**](./BezierCurve.md) - Easing curve implementations

## 🏗️ Architecture Diagram

```
┌─────────────────┐
│   Stack3DPlugin │ ◄─── Main Plugin Entry Point
│   (Coordinator) │      • State Management
└─────────┬───────┘      • Event Handling
          │               • Configuration
          │
    ┌─────▼─────┐ ┌─────────────────┐ ┌─────────────────┐
    │ Animation │ │ LayoutCalculator│ │  PhysicsMotion  │
    │  System   │ │                 │ │                 │
    └─────┬─────┘ └─────────────────┘ └─────────────────┘
          │
    ┌─────▼─────┐
    │ BezierCurve│
    │           │
    └───────────┘
```

## 🚀 Quick Start

### Basic Plugin Usage

```cpp
#include "Stack3DPlugin.hpp"

// Initialize plugin
std::unique_ptr<Stack3DPlugin> plugin;
plugin = std::make_unique<Stack3DPlugin>(handle);

// Register keybinds
plugin->registerKeybinds();

// Toggle state
plugin->toggleState();
```

### Layout Calculation

```cpp
#include "LayoutCalculator.hpp"

// Create calculator
LayoutCalculator calculator(800.0f, 1000.0f, 100.0f, 20.0f);

// Calculate layouts
std::vector<CWindow*> windows = getCurrentWindows();
auto stackLayout = calculator.calculateStackLayout(windows);
auto gridLayout = calculator.calculateGridLayout(windows);
```

### Animation System

```cpp
#include "AnimationSystem.hpp"

// Create animation system
AnimationSystem animation(0.8f, 0.3f);

// Start transition
animation.startTransition(
    startLayouts, 
    endLayouts, 
    TransitionStyle::SMOOTH_SLIDE,
    []() { /* completion callback */ }
);
```

## 🎯 Core Concepts

### State Management

The plugin manages three main states:

```cpp
enum class StackState {
    STACKED_3D,      // Windows in 3D stack formation
    TRANSITIONING,   // Animation in progress  
    SPREAD_LAYOUT    // Windows in spread layout
};
```

### Layout Types

Four layout algorithms are supported:

```cpp
enum class LayoutType {
    GRID,        // Optimal grid arrangement
    CIRCULAR,    // Circular arrangement
    SPIRAL,      // Spiral pattern
    FIBONACCI    // Golden ratio splits
};
```

### Animation Styles

Seven animation styles with different easing curves:

```cpp
enum class TransitionStyle {
    SMOOTH_SLIDE,     // Linear transitions
    BOUNCE_IN,        // Bounce effect
    ELASTIC_OUT,      // Elastic overshoot
    CASCADE_WAVE,     // Staggered animation
    SPIRAL_MOTION,    // Rotating motion
    MAGNETIC_ATTRACT, // Snap-to-place
    LIQUID_FLOW       // Fluid movement
};
```

## 📊 Data Structures

### WindowLayout

Primary data structure for window positioning:

```cpp
struct WindowLayout {
    CWindow* window;        // Window reference
    Vector2D position;      // Screen position
    Vector2D size;          // Window dimensions
    float rotation;         // Rotation angle (degrees)
    float scale;            // Scale factor
    float alpha;            // Transparency
    int zIndex;             // Depth order
    Vector2D velocity;      // Motion velocity
};
```

### PluginConfig

Configuration structure:

```cpp
struct PluginConfig {
    bool enabled;
    float transitionDuration;
    float staggerDelay;
    TransitionStyle transitionStyle;
    float stackDepthStep;
    float spreadPadding;
    LayoutType defaultLayout;
    float springStrength;
    float damping;
    bool motionBlur;
    float perspective;
    float eyeDistance;
    std::string toggleKey;
    std::string peekKey;
    std::string cycleLayoutKey;
};
```

## 🔧 Plugin API

### Hyprland Integration

The plugin implements standard Hyprland plugin API:

```cpp
// Plugin metadata
APICALL EXPORT std::string PLUGIN_API_VERSION();

// Plugin lifecycle
APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle);
APICALL EXPORT void PLUGIN_EXIT();
```

### Event Hooks

The plugin registers for Hyprland events:

```cpp
// Window lifecycle events
void onWindowOpen(CWindow* window);
void onWindowClose(CWindow* window);
void onWindowFocus(CWindow* window);

// Workspace events  
void onWorkspaceChange();

// Configuration events
void onConfigReload();
```

## 📐 Mathematics

### 3D Transformations

The plugin uses perspective projection for 3D effects:

```cpp
struct Transform3D {
    float perspective;
    float eyeDistance;
    
    Vector2D project3DTo2D(Vector3D pos3D, Vector2D screenCenter) const;
    float getScaleForDepth(float zPos) const;
};
```

### Physics Calculations

Spring-damping physics for natural motion:

```cpp
// Force equations
F_spring = k * displacement
F_damping = -c * velocity
acceleration = (F_spring + F_damping) / mass
```

### Bezier Mathematics

Cubic Bezier curves for smooth easing:

```cpp
// Cubic Bezier formula
B(t) = (1-t)³P₀ + 3(1-t)²tP₁ + 3(1-t)t²P₂ + t³P₃
```

## 🎨 Rendering Integration

### Window Transformation

```cpp
void applyWindowTransform(CWindow* window, const WindowLayout& layout) {
    window->m_vRealPosition = layout.position;
    window->m_vRealSize = layout.size;
    // Additional transformations...
}
```

### Motion Blur

```cpp
void addMotionBlur(CWindow* window, Vector2D velocity) {
    // Integration with Hyprland's rendering pipeline
    // Blur strength based on velocity magnitude
}
```

## 🔍 Debugging API

### Debug Information

```cpp
// Get current state
StackState getCurrentState() const;
const PluginConfig& getConfig() const;

// Animation status
bool isAnimating() const;
float getProgress() const;

// Window management
std::vector<CWindow*> getCurrentWorkspaceWindows();
void updateManagedWindows();
```

### Performance Monitoring

```cpp
// Timing information
std::chrono::steady_clock::time_point getLastTransitionTime();
bool isTransitionAllowed();

// Physics state
Vector2D getVelocity(CWindow* window) const;
void getMotionState(CWindow* window);
```

## 📚 Detailed Documentation

### Component APIs

- [**Stack3DPlugin API**](./Stack3DPlugin.md) - Main plugin interface
- [**LayoutCalculator API**](./LayoutCalculator.md) - Layout calculation methods
- [**AnimationSystem API**](./AnimationSystem.md) - Animation control interface
- [**PhysicsMotion API**](./PhysicsMotion.md) - Physics simulation interface  
- [**BezierCurve API**](./BezierCurve.md) - Easing curve mathematics

### Integration Guides

- [**Hyprland Integration**](./hyprland-integration.md) - Plugin API usage
- [**Event System**](./event-system.md) - Event handling and hooks
- [**Configuration System**](./configuration-system.md) - Config management
- [**Rendering Integration**](./rendering-integration.md) - Graphics pipeline

### Advanced Topics

- [**Performance Optimization**](./performance.md) - Optimization techniques
- [**Memory Management**](./memory.md) - Resource management
- [**Thread Safety**](./threading.md) - Concurrency considerations
- [**Error Handling**](./error-handling.md) - Robust error management

## 🔬 Testing API

### Unit Testing Interface

```cpp
// Test helpers (debug builds only)
#ifdef DEBUG
bool validateWindowLayout(const WindowLayout& layout);
bool verifyPhysicsCalculations();
void simulateTransition(float deltaTime);
#endif
```

### Mock Interfaces

```cpp
// Mock objects for testing
class MockWindow : public CWindow { /* ... */ };
class MockCompositor : public Compositor { /* ... */ };
```

## 📈 Version Compatibility

| Plugin Version | Hyprland API | C++ Standard | Notes |
|----------------|--------------|--------------|-------|
| 1.0.0 | Latest | C++23 | Initial release |
| 1.1.0 | Latest | C++23 | Performance improvements |

## 🤝 Contributing to API

When contributing to the API:

1. **Maintain compatibility** - Don't break existing interfaces
2. **Document thoroughly** - Include detailed comments
3. **Add tests** - Cover new functionality
4. **Follow patterns** - Use consistent naming and structure
5. **Performance focused** - Consider real-time constraints

## 📝 API Examples

### Complete Integration Example

```cpp
#include "Stack3DPlugin.hpp"

class MyHyprlandPlugin {
private:
    std::unique_ptr<Stack3DPlugin> m_stack3d;
    
public:
    void initialize() {
        // Create plugin instance
        m_stack3d = std::make_unique<Stack3DPlugin>(m_handle);
        
        // Configure animation preferences
        PluginConfig config;
        config.transitionStyle = TransitionStyle::ELASTIC_OUT;
        config.defaultLayout = LayoutType::FIBONACCI;
        
        // Register custom event handlers
        m_stack3d->registerKeybinds();
        
        // Setup custom keybinds
        HyprlandAPI::addKeybind(m_handle, "SUPER, t", [this](const std::string&) {
            m_stack3d->transitionToState(StackState::STACKED_3D);
        });
    }
    
    void update() {
        // Update animations each frame
        if (m_stack3d->getCurrentState() == StackState::TRANSITIONING) {
            // Plugin handles updates internally
        }
    }
};
```

---

For complete implementation details, see the individual component documentation pages.