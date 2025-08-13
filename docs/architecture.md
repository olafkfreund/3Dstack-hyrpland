# Architecture Overview

Comprehensive architecture documentation for the Hyprland Stack3D plugin system.

## 🏗️ System Architecture

### High-Level Overview

```
┌─────────────────────────────────────────────────────────────────────┐
│                           Hyprland                                  │
│  ┌─────────────────────┐ ┌─────────────────────┐ ┌─────────────────┐│
│  │   Event System      │ │   Window Manager    │ │  Render Pipeline││
│  │                     │ │                     │ │                 ││
│  └──────────┬──────────┘ └──────────┬──────────┘ └─────────┬───────┘│
└─────────────┼─────────────────────────┼────────────────────────┼────┘
              │                         │                        │
              ▼                         ▼                        ▼
    ┌─────────────────┐       ┌─────────────────┐      ┌─────────────────┐
    │   Stack3D       │       │   Window        │      │   Transform     │
    │   Plugin        │◄──────┤   Events        │─────►│   Application   │
    │   (Coordinator) │       │                 │      │                 │
    └─────────┬───────┘       └─────────────────┘      └─────────────────┘
              │
              ▼
    ┌─────────────────────────────────────────────────────────────────────┐
    │                    Plugin Subsystems                                │
    │  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐  │
    │  │   Layout    │ │ Animation   │ │  Physics    │ │   Bezier    │  │
    │  │ Calculator  │ │   System    │ │   Motion    │ │   Curve     │  │
    │  └─────────────┘ └─────────────┘ └─────────────┘ └─────────────┘  │
    └─────────────────────────────────────────────────────────────────────┘
```

### Component Relationships

```
Stack3DPlugin (Main Coordinator)
├── Configuration Management
├── State Machine (STACKED_3D ↔ TRANSITIONING ↔ SPREAD_LAYOUT)
├── Event Handling (Window/Workspace events)
└── Subsystem Orchestration
    ├── LayoutCalculator (Geometric calculations)
    │   ├── 3D Transformations
    │   ├── Layout Algorithms (Grid, Circular, Spiral, Fibonacci)
    │   └── Workspace Integration
    ├── AnimationSystem (Smooth transitions)
    │   ├── Timeline Management
    │   ├── Interpolation Engine
    │   └── BezierCurve (Easing functions)
    └── PhysicsMotion (Realistic movement)
        ├── Spring-Damper System
        ├── Force Calculations
        └── Velocity Integration
```

## 🔧 Core Components

### 1. Stack3DPlugin (Main Coordinator)

**Responsibilities:**
- Plugin lifecycle management
- State transition orchestration
- Event handling and distribution
- Configuration management
- Hyprland API integration

**Key Design Patterns:**
- **Coordinator Pattern**: Orchestrates all subsystems
- **State Machine**: Manages plugin state transitions
- **Observer Pattern**: Handles Hyprland events
- **Command Pattern**: Executes state changes

**State Machine:**
```
    [SPREAD_LAYOUT] ←──────────────→ [STACKED_3D]
           │                             │
           └───→ [TRANSITIONING] ←──────┘
                      │     ▲
                      └─────┘
```

### 2. LayoutCalculator (Geometric Engine)

**Responsibilities:**
- Window position calculations
- 3D perspective transformations
- Layout algorithm implementations
- Workspace bounds management

**Mathematical Foundation:**
```cpp
// 3D Perspective Projection
Vector2D project3DTo2D(Vector3D pos3D, Vector2D center) {
    float projX = (pos3D.x * perspective) / (eyeDistance + pos3D.z);
    float projY = (pos3D.y * perspective) / (eyeDistance + pos3D.z);
    return {center.x + projX, center.y + projY};
}

// Depth-based scaling
float scaleForDepth(float z) {
    return perspective / (eyeDistance + z);
}
```

**Layout Algorithms:**
- **Grid**: Optimal rectangular arrangement
- **Circular**: Radial distribution around center  
- **Spiral**: Logarithmic spiral with rotation
- **Fibonacci**: Golden ratio-based recursive splitting

### 3. AnimationSystem (Transition Engine)

**Responsibilities:**
- Timeline management
- Property interpolation
- Staggered animations
- Completion callbacks

**Animation Pipeline:**
```
Start Layouts → Timeline Setup → Interpolation Loop → Apply Transforms → Complete
     │               │                   │                 │              │
     ▼               ▼                   ▼                 ▼              ▼
  Calculate      Set Delays         Bezier Easing    Window Update    Callback
  Differences    Per Window        + Linear Interp    Positions       Execution
```

**Interpolation Formula:**
```cpp
template<typename T>
T lerp(const T& start, const T& end, float t) {
    return start + (end - start) * ease(t);
}
```

### 4. PhysicsMotion (Realism Engine)

**Responsibilities:**
- Spring-damper physics simulation
- Force accumulation and integration
- Velocity-based motion blur
- Natural movement patterns

**Physics Model:**
```
F_spring = k * (target_position - current_position)
F_damping = -c * velocity
F_total = F_spring + F_damping + F_external
acceleration = F_total / mass
velocity += acceleration * dt
position += velocity * dt
```

**Force Types:**
- **Spring Forces**: Attraction to target position
- **Damping Forces**: Velocity-based resistance
- **Gravity Forces**: Attraction to specific points
- **Repulsion Forces**: Avoidance between windows

### 5. BezierCurve (Easing Mathematics)

**Responsibilities:**
- Cubic Bezier curve evaluation
- Predefined easing presets
- Smooth animation transitions

**Mathematical Implementation:**
```cpp
// Cubic Bezier formula: B(t) = (1-t)³P₀ + 3(1-t)²tP₁ + 3(1-t)t²P₂ + t³P₃
float cubicBezier(float t, float p1, float p2) {
    float u = 1.0f - t;
    return 3 * u * u * t * p1 + 3 * u * t * t * p2 + t * t * t;
}
```

## 🔄 Data Flow Architecture

### Event-Driven Flow

```
Hyprland Event → Stack3DPlugin → State Analysis → Action Decision
                      │                              │
                      ▼                              ▼
              Update Windows List            Trigger Transition
                      │                              │
                      ▼                              ▼
                Layout Calculation          Animation Start
                      │                              │
                      ▼                              ▼
                Window Transform            Per-Frame Update
                      │                              │
                      ▼                              ▼
                Hyprland Render              Animation Complete
```

### Configuration Flow

```
Hyprland Config → Plugin Config Load → Validate Values → Update Subsystems
                                              │                    │
                                              ▼                    ▼
                                      Default Fallback    Component Refresh
                                              │                    │
                                              ▼                    ▼
                                      Log Warnings        Ready for Use
```

### Animation Loop

```
Frame Start → Check Animation State → Update Timeline → Interpolate Properties
     ▲                                        │                    │
     │                                        ▼                    ▼
  Schedule Next ← Apply Physics ← Calculate Easing ← Compute Layout
     │                │                              │
     ▼                ▼                              ▼
  60 FPS Timer   Window Transform                Output Values
```

## 🏛️ Design Patterns

### 1. Strategy Pattern (Layout Algorithms)

```cpp
class LayoutCalculator {
    std::vector<WindowLayout> calculateSpreadLayout(
        const std::vector<CWindow*>& windows, LayoutType type) {
        switch (type) {
            case GRID: return calculateGridLayout(windows);
            case CIRCULAR: return calculateCircularLayout(windows);
            case SPIRAL: return calculateSpiralLayout(windows);
            case FIBONACCI: return calculateFibonacciLayout(windows);
        }
    }
};
```

### 2. Observer Pattern (Event Handling)

```cpp
class Stack3DPlugin {
    void initializeHooks() {
        m_windowOpenHook = HyprlandAPI::registerCallbackDynamic(
            m_handle, "openWindow", [this](void*, std::any data) {
                onWindowOpen(std::any_cast<CWindow*>(data));
            });
    }
};
```

### 3. Command Pattern (State Operations)

```cpp
class Stack3DPlugin {
    void toggleState() {
        StackState newState = (m_currentState == STACKED_3D) ? 
                              SPREAD_LAYOUT : STACKED_3D;
        transitionToState(newState);
    }
};
```

### 4. Template Method (Animation Framework)

```cpp
class AnimationSystem {
    void updateAnimation() {
        for (auto& transition : m_transitions) {
            float progress = calculateProgress(transition);
            float easedProgress = applyEasing(progress, transition.curve);
            updateWindowProperties(transition, easedProgress);
        }
    }
};
```

## 🎯 Performance Architecture

### Memory Management

**RAII Principles:**
- Smart pointers for automatic cleanup
- Stack allocation for temporary objects
- Move semantics for large data transfers

**Resource Lifecycle:**
```cpp
// Plugin owns subsystems
std::unique_ptr<LayoutCalculator> m_layoutCalculator;
std::unique_ptr<AnimationSystem> m_animationSystem;

// Windows are referenced, not owned
std::vector<CWindow*> m_managedWindows;  // Raw pointers

// Event hooks are managed by Hyprland
SP<HOOK_CALLBACK_FN> m_windowOpenHook;   // Shared ownership
```

### Performance Optimizations

**Spatial Optimization:**
- Layout calculations only when windows change
- Animation updates only during transitions
- Physics simulation only for moving windows

**Temporal Optimization:**
- Frame rate limiting (60 FPS max)
- Early animation termination
- Predictive layout caching

**Algorithmic Optimization:**
- O(n) layout algorithms where possible
- Incremental physics updates
- Bezier curve lookup tables

### Threading Model

**Single-Threaded Design:**
- All operations on Hyprland's main thread
- No synchronization required
- Simple debugging and maintenance
- Event-driven, non-blocking architecture

```
Hyprland Main Thread
├── Event Processing
├── Layout Calculation  
├── Animation Updates
├── Physics Simulation
└── Window Transformation
```

## 🔒 Safety & Robustness

### Error Handling Strategy

**Defensive Programming:**
- Null pointer checks before window operations
- Bounds checking for array access
- Graceful degradation on invalid configuration

**Error Recovery:**
```cpp
bool Stack3DPlugin::isTransitionAllowed() {
    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastTransition = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - m_lastTransition).count();
    return timeSinceLastTransition > 100; // Rate limiting
}
```

### State Consistency

**Atomic Transitions:**
- State changes are all-or-nothing
- Rollback capability on failures
- Consistent window list management

**Validation:**
- Configuration value validation
- Window state verification
- Animation timeline consistency

### Resource Limits

**Memory Bounds:**
- Maximum number of managed windows
- Animation history limits
- Configuration value constraints

**Computation Limits:**
- Maximum animation duration
- Physics simulation complexity limits
- Layout calculation timeouts

## 🔌 Integration Architecture

### Hyprland Plugin API

**Plugin Interface:**
```cpp
// Required plugin exports
EXPORT std::string PLUGIN_API_VERSION();
EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle);
EXPORT void PLUGIN_EXIT();
```

**Event Hooks:**
- Window lifecycle events
- Workspace change events  
- Configuration reload events
- Focus change events

### Configuration Integration

**Config Namespace:**
```bash
plugin {
    stack3d {
        # All plugin configuration
        enable = true
        transition_duration = 0.8
        # ... additional settings
    }
}
```

**Dynamic Updates:**
- Configuration hot-reload support
- Runtime parameter adjustment
- Validation and fallback handling

### Rendering Integration

**Transform Application:**
```cpp
void applyWindowTransform(CWindow* window, const WindowLayout& layout) {
    window->m_vRealPosition = layout.position;
    window->m_vRealSize = layout.size;
    // Additional transformations through Hyprland's rendering pipeline
}
```

## 📊 Metrics & Monitoring

### Performance Metrics

**Animation Performance:**
- Frame timing consistency
- Animation completion rates
- CPU usage during transitions

**Memory Metrics:**
- Plugin memory footprint
- Window reference counts
- Animation state memory usage

### Debug Architecture

**Logging Levels:**
- Error: Critical failures
- Warning: Degraded functionality  
- Info: State transitions
- Debug: Detailed execution flow

**Debug Interfaces:**
```cpp
#ifdef DEBUG
    void dumpAnimationState();
    void validateWindowLayouts();
    void performanceProfile();
#endif
```

## 🚀 Scalability Considerations

### Window Count Scaling

**Algorithm Complexity:**
- Grid Layout: O(n)
- Circular Layout: O(n)  
- Spiral Layout: O(n)
- Fibonacci Layout: O(n)

**Memory Scaling:**
- Linear with window count
- Constant animation memory overhead
- Bounded physics state per window

### Multi-Monitor Support

**Per-Monitor State:**
- Independent plugin state per monitor
- Synchronized or independent animations
- Monitor-specific configurations

### Extension Points

**Plugin Extensions:**
- Custom layout algorithms
- Additional animation styles
- External state notifications
- Custom physics models

---

This architecture balances performance, maintainability, and extensibility while integrating seamlessly with Hyprland's existing systems. The modular design allows for easy testing, debugging, and future enhancements.