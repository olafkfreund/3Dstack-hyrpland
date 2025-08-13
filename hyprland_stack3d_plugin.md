# Hyprland 3D Stack Sliding Animation Plugin

A comprehensive plugin for Hyprland that provides smooth 3D stack animations with sliding transitions between stacked and spread layouts.

## Project Structure

```
hyprland-stack3d/
├── Makefile
├── main.cpp
├── include/
│   ├── Stack3DPlugin.hpp
│   ├── LayoutCalculator.hpp
│   ├── AnimationSystem.hpp
│   ├── PhysicsMotion.hpp
│   └── BezierCurve.hpp
├── src/
│   ├── Stack3DPlugin.cpp
│   ├── LayoutCalculator.cpp
│   ├── AnimationSystem.cpp
│   ├── PhysicsMotion.cpp
│   └── BezierCurve.cpp
└── README.md
```

## Makefile

```makefile
PLUGIN_NAME = stack3d
CXX = g++
CXXFLAGS = -shared -fPIC -g -std=c++23 -Wall -Wextra
INCLUDES = -Iinclude `pkg-config --cflags pixman-1 libdrm hyprland`
LIBS = `pkg-config --libs pixman-1 libdrm hyprland`

SRCDIR = src
INCDIR = include
SOURCES = $(wildcard $(SRCDIR)/*.cpp) main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(PLUGIN_NAME).so

$(PLUGIN_NAME).so: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(PLUGIN_NAME).so

install: $(PLUGIN_NAME).so
	cp $(PLUGIN_NAME).so ~/.config/hypr/plugins/

.PHONY: all clean install
```

## main.cpp

```cpp
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/Window.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#include <hyprland/src/managers/AnimationManager.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

#include "include/Stack3DPlugin.hpp"

// Global plugin instance
std::unique_ptr<Stack3DPlugin> g_pStack3DPlugin;

// Plugin metadata
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    HyprlandAPI::registerCallbackDynamic(handle, "configReloaded", [&](void* self, std::any data) {
        if (g_pStack3DPlugin) {
            g_pStack3DPlugin->onConfigReload();
        }
    });

    // Initialize plugin
    g_pStack3DPlugin = std::make_unique<Stack3DPlugin>(handle);
    
    // Register keybinds
    g_pStack3DPlugin->registerKeybinds();

    HyprlandAPI::addNotification(handle, "[stack3d] Plugin loaded successfully!", 
                                 CColor{0.2f, 0.8f, 0.3f, 1.0f}, 3000);

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
```

## include/Stack3DPlugin.hpp

```cpp
#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/Window.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

#include "LayoutCalculator.hpp"
#include "AnimationSystem.hpp"
#include "PhysicsMotion.hpp"

enum class StackState {
    STACKED_3D,
    TRANSITIONING,
    SPREAD_LAYOUT
};

enum class TransitionStyle {
    SMOOTH_SLIDE,
    BOUNCE_IN,
    ELASTIC_OUT,
    CASCADE_WAVE,
    SPIRAL_MOTION,
    MAGNETIC_ATTRACT,
    LIQUID_FLOW
};

enum class LayoutType {
    GRID,
    CIRCULAR,
    SPIRAL,
    FIBONACCI
};

struct PluginConfig {
    bool enabled = true;
    float transitionDuration = 0.8f;
    float staggerDelay = 0.05f;
    TransitionStyle transitionStyle = TransitionStyle::SMOOTH_SLIDE;
    float stackDepthStep = 100.0f;
    float spreadPadding = 20.0f;
    LayoutType defaultLayout = LayoutType::GRID;
    float springStrength = 0.8f;
    float damping = 0.92f;
    bool motionBlur = true;
    float perspective = 800.0f;
    float eyeDistance = 1000.0f;
    std::string toggleKey = "SUPER, grave";
    std::string peekKey = "SUPER, space";
    std::string cycleLayoutKey = "SUPER SHIFT, grave";
};

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
    StackState getCurrentState() const { return m_currentState; }
    const PluginConfig& getConfig() const { return m_config; }
    
    // Window management
    std::vector<CWindow*> getCurrentWorkspaceWindows();
    void updateManagedWindows();
    
private:
    void initializeHooks();
    void cleanupHooks();
    bool isTransitionAllowed();
};
```

## include/LayoutCalculator.hpp

```cpp
#pragma once

#include <vector>
#include <cmath>
#include <hyprland/src/Window.hpp>
#include <hyprland/src/helpers/Vector2D.hpp>

struct WindowLayout {
    CWindow* window;
    Vector2D position;
    Vector2D size;
    float rotation;
    float scale;
    float alpha;
    int zIndex;
    Vector2D velocity; // For physics
};

struct GridLayout {
    int rows;
    int cols;
    Vector2D cellSize;
};

struct Transform3D {
    float perspective;
    float eyeDistance;
    
    Vector2D project3DTo2D(Vector3D pos3D, Vector2D screenCenter) const;
    float getScaleForDepth(float zPos) const;
};

class LayoutCalculator {
private:
    Transform3D m_transform;
    float m_depthStep;
    float m_padding;
    
public:
    LayoutCalculator(float perspective = 800.0f, float eyeDistance = 1000.0f, 
                    float depthStep = 100.0f, float padding = 20.0f);
    
    // Layout calculations
    std::vector<WindowLayout> calculateStackLayout(const std::vector<CWindow*>& windows);
    std::vector<WindowLayout> calculateSpreadLayout(const std::vector<CWindow*>& windows, 
                                                   LayoutType layoutType);
    
    // Specific layout types
    std::vector<WindowLayout> calculateGridLayout(const std::vector<CWindow*>& windows);
    std::vector<WindowLayout> calculateCircularLayout(const std::vector<CWindow*>& windows);
    std::vector<WindowLayout> calculateSpiralLayout(const std::vector<CWindow*>& windows);
    std::vector<WindowLayout> calculateFibonacciLayout(const std::vector<CWindow*>& windows);
    
    // Utility functions
    GridLayout calculateOptimalGrid(int windowCount);
    Vector2D getWorkspaceCenter();
    Vector2D getWorkspaceSize();
    float getWorkspaceAspectRatio();
    
    // Configuration
    void updateTransform(float perspective, float eyeDistance);
    void setDepthStep(float depthStep) { m_depthStep = depthStep; }
    void setPadding(float padding) { m_padding = padding; }
};
```

## include/AnimationSystem.hpp

```cpp
#pragma once

#include <vector>
#include <functional>
#include <memory>
#include <chrono>

#include "LayoutCalculator.hpp"
#include "BezierCurve.hpp"

enum class StackState;
enum class TransitionStyle;

struct WindowTransition {
    CWindow* window;
    
    // Start state
    Vector2D startPos;
    Vector2D startSize;
    float startRotation;
    float startScale;
    float startAlpha;
    
    // End state
    Vector2D endPos;
    Vector2D endSize;
    float endRotation;
    float endScale;
    float endAlpha;
    
    // Animation properties
    float startDelay;
    BezierCurve easingCurve;
    bool isComplete;
};

class AnimationSystem {
private:
    std::vector<WindowTransition> m_transitions;
    std::chrono::steady_clock::time_point m_animationStart;
    float m_baseDuration;
    float m_maxStagger;
    bool m_isAnimating;
    
    std::function<void()> m_onComplete;
    
public:
    AnimationSystem(float baseDuration = 0.8f, float maxStagger = 0.3f);
    
    // Animation control
    void startTransition(const std::vector<WindowLayout>& startLayouts,
                        const std::vector<WindowLayout>& endLayouts,
                        TransitionStyle style,
                        std::function<void()> onComplete = nullptr);
    
    void updateAnimation();
    void stopAnimation();
    
    // State queries
    bool isAnimating() const { return m_isAnimating; }
    float getProgress() const;
    
    // Configuration
    void setDuration(float duration) { m_baseDuration = duration; }
    void setMaxStagger(float stagger) { m_maxStagger = stagger; }
    
private:
    BezierCurve createEasingCurve(TransitionStyle style);
    void applyWindowTransform(CWindow* window, const WindowLayout& layout);
    void addMotionBlur(CWindow* window, Vector2D velocity);
    Vector2D calculateVelocity(const WindowTransition& transition, float progress);
    
    // Interpolation helpers
    template<typename T>
    T lerp(const T& start, const T& end, float t) {
        return start + (end - start) * t;
    }
};
```

## include/PhysicsMotion.hpp

```cpp
#pragma once

#include <unordered_map>
#include <hyprland/src/Window.hpp>
#include <hyprland/src/helpers/Vector2D.hpp>

struct MotionState {
    Vector2D velocity;
    Vector2D acceleration;
    float angularVelocity;
    float angularAcceleration;
    float spring;
    float damping;
    float mass;
    
    MotionState() : velocity{0, 0}, acceleration{0, 0}, 
                   angularVelocity(0), angularAcceleration(0),
                   spring(0.8f), damping(0.92f), mass(1.0f) {}
};

class PhysicsMotion {
private:
    std::unordered_map<CWindow*, MotionState> m_windowMotions;
    float m_globalDamping;
    float m_globalSpring;
    
public:
    PhysicsMotion(float globalSpring = 0.8f, float globalDamping = 0.92f);
    
    // Force application
    void addSpringForce(CWindow* window, Vector2D targetPos, Vector2D currentPos);
    void addDampingForce(CWindow* window);
    void addGravityForce(CWindow* window, Vector2D gravityCenter, float strength);
    void addRepulsionForce(CWindow* window, const std::vector<CWindow*>& otherWindows, float strength);
    
    // Motion calculation
    Vector2D getNextPosition(CWindow* window, Vector2D currentPos, float deltaTime);
    float getNextRotation(CWindow* window, float currentRotation, float deltaTime);
    Vector2D getVelocity(CWindow* window) const;
    
    // State management
    void resetMotion(CWindow* window);
    void setMotionProperties(CWindow* window, float spring, float damping, float mass = 1.0f);
    void updateMotion(float deltaTime);
    
    // Configuration
    void setGlobalSpring(float spring) { m_globalSpring = spring; }
    void setGlobalDamping(float damping) { m_globalDamping = damping; }
    
    // Cleanup
    void removeWindow(CWindow* window);
    void clear();
    
private:
    MotionState& getOrCreateMotionState(CWindow* window);
    void applyForces(MotionState& motion, float deltaTime);
};
```

## include/BezierCurve.hpp

```cpp
#pragma once

class BezierCurve {
private:
    float m_x1, m_y1, m_x2, m_y2;
    
public:
    BezierCurve(float x1, float y1, float x2, float y2);
    
    float evaluate(float t) const;
    
    // Predefined curves
    static BezierCurve easeOut() { return BezierCurve(0.25f, 0.1f, 0.25f, 1.0f); }
    static BezierCurve easeIn() { return BezierCurve(0.42f, 0.0f, 1.0f, 1.0f); }
    static BezierCurve easeInOut() { return BezierCurve(0.42f, 0.0f, 0.58f, 1.0f); }
    static BezierCurve bounce() { return BezierCurve(0.68f, -0.55f, 0.265f, 1.55f); }
    static BezierCurve elastic() { return BezierCurve(0.175f, 0.885f, 0.32f, 1.275f); }
    
private:
    float cubicBezier(float t, float p1, float p2) const;
    float solveCubicBezier(float x) const;
};
```

## src/Stack3DPlugin.cpp

```cpp
#include "../include/Stack3DPlugin.hpp"
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#include <hyprland/src/Compositor.hpp>

Stack3DPlugin::Stack3DPlugin(HANDLE handle) 
    : m_handle(handle), m_currentState(StackState::SPREAD_LAYOUT) {
    
    loadConfig();
    
    m_layoutCalculator = std::make_unique<LayoutCalculator>(
        m_config.perspective, m_config.eyeDistance, 
        m_config.stackDepthStep, m_config.spreadPadding);
    
    m_animationSystem = std::make_unique<AnimationSystem>(
        m_config.transitionDuration, m_config.staggerDelay);
    
    m_physicsMotion = std::make_unique<PhysicsMotion>(
        m_config.springStrength, m_config.damping);
    
    initializeHooks();
    updateManagedWindows();
}

Stack3DPlugin::~Stack3DPlugin() {
    cleanupHooks();
}

void Stack3DPlugin::loadConfig() {
    // Load configuration from Hyprland config
    auto* const PCONFIG = g_pConfigManager;
    
    m_config.enabled = PCONFIG->getConfigValuePtr("plugin:stack3d:enable")->intValue;
    m_config.transitionDuration = PCONFIG->getConfigValuePtr("plugin:stack3d:transition_duration")->floatValue;
    m_config.staggerDelay = PCONFIG->getConfigValuePtr("plugin:stack3d:stagger_delay")->floatValue;
    m_config.stackDepthStep = PCONFIG->getConfigValuePtr("plugin:stack3d:stack_depth_step")->floatValue;
    m_config.spreadPadding = PCONFIG->getConfigValuePtr("plugin:stack3d:spread_padding")->floatValue;
    m_config.springStrength = PCONFIG->getConfigValuePtr("plugin:stack3d:spring_strength")->floatValue;
    m_config.damping = PCONFIG->getConfigValuePtr("plugin:stack3d:damping")->floatValue;
    m_config.motionBlur = PCONFIG->getConfigValuePtr("plugin:stack3d:motion_blur")->intValue;
    m_config.perspective = PCONFIG->getConfigValuePtr("plugin:stack3d:perspective")->floatValue;
    m_config.eyeDistance = PCONFIG->getConfigValuePtr("plugin:stack3d:eye_distance")->floatValue;
    
    // Parse transition style
    std::string styleStr = PCONFIG->getConfigValuePtr("plugin:stack3d:transition_style")->strValue;
    if (styleStr == "bounce_in") m_config.transitionStyle = TransitionStyle::BOUNCE_IN;
    else if (styleStr == "elastic_out") m_config.transitionStyle = TransitionStyle::ELASTIC_OUT;
    else if (styleStr == "cascade_wave") m_config.transitionStyle = TransitionStyle::CASCADE_WAVE;
    else m_config.transitionStyle = TransitionStyle::SMOOTH_SLIDE;
    
    // Parse layout type
    std::string layoutStr = PCONFIG->getConfigValuePtr("plugin:stack3d:default_layout")->strValue;
    if (layoutStr == "circular") m_config.defaultLayout = LayoutType::CIRCULAR;
    else if (layoutStr == "spiral") m_config.defaultLayout = LayoutType::SPIRAL;
    else if (layoutStr == "fibonacci") m_config.defaultLayout = LayoutType::FIBONACCI;
    else m_config.defaultLayout = LayoutType::GRID;
}

void Stack3DPlugin::toggleState() {
    if (!m_config.enabled || !isTransitionAllowed()) return;
    
    StackState newState = (m_currentState == StackState::STACKED_3D) ? 
                         StackState::SPREAD_LAYOUT : StackState::STACKED_3D;
    
    transitionToState(newState);
}

void Stack3DPlugin::transitionToState(StackState newState) {
    if (m_currentState == newState || m_currentState == StackState::TRANSITIONING) return;
    
    updateManagedWindows();
    if (m_managedWindows.empty()) return;
    
    // Calculate current and target layouts
    std::vector<WindowLayout> currentLayouts;
    std::vector<WindowLayout> targetLayouts;
    
    if (m_currentState == StackState::STACKED_3D) {
        currentLayouts = m_layoutCalculator->calculateStackLayout(m_managedWindows);
    } else {
        currentLayouts = m_layoutCalculator->calculateSpreadLayout(m_managedWindows, m_config.defaultLayout);
    }
    
    if (newState == StackState::STACKED_3D) {
        targetLayouts = m_layoutCalculator->calculateStackLayout(m_managedWindows);
    } else {
        targetLayouts = m_layoutCalculator->calculateSpreadLayout(m_managedWindows, m_config.defaultLayout);
    }
    
    // Start transition
    m_currentState = StackState::TRANSITIONING;
    m_lastTransition = std::chrono::steady_clock::now();
    
    m_animationSystem->startTransition(currentLayouts, targetLayouts, m_config.transitionStyle,
        [this, newState]() {
            m_currentState = newState;
            HyprlandAPI::addNotification(m_handle, 
                newState == StackState::STACKED_3D ? "Entered 3D Stack Mode" : "Entered Spread Mode",
                CColor{0.2f, 0.8f, 0.3f, 1.0f}, 1500);
        });
}

void Stack3DPlugin::registerKeybinds() {
    HyprlandAPI::addKeybind(m_handle, m_config.toggleKey, [this](const std::string&) {
        toggleState();
    });
    
    HyprlandAPI::addKeybind(m_handle, m_config.peekKey, [this](const std::string&) {
        if (m_currentState == StackState::STACKED_3D) {
            temporaryPeek(1.0f);
        }
    });
    
    HyprlandAPI::addKeybind(m_handle, m_config.cycleLayoutKey, [this](const std::string&) {
        cycleLayoutType();
    });
}

std::vector<CWindow*> Stack3DPlugin::getCurrentWorkspaceWindows() {
    std::vector<CWindow*> windows;
    
    auto* workspace = g_pCompositor->getWorkspaceByID(g_pCompositor->m_pLastMonitor->activeWorkspace);
    if (!workspace) return windows;
    
    for (auto& window : g_pCompositor->m_vWindows) {
        if (window->m_iWorkspaceID == workspace->m_iID && 
            window->m_bIsMapped && 
            !window->isHidden() && 
            !window->m_bIsFloating) {
            windows.push_back(window.get());
        }
    }
    
    return windows;
}

void Stack3DPlugin::updateManagedWindows() {
    m_managedWindows = getCurrentWorkspaceWindows();
}

bool Stack3DPlugin::isTransitionAllowed() {
    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastTransition = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - m_lastTransition).count();
    
    return timeSinceLastTransition > 100; // Minimum 100ms between transitions
}

void Stack3DPlugin::initializeHooks() {
    m_windowOpenHook = HyprlandAPI::registerCallbackDynamic(m_handle, "openWindow", 
        [this](void*, std::any data) {
            auto* window = std::any_cast<CWindow*>(data);
            onWindowOpen(window);
        });
    
    m_windowCloseHook = HyprlandAPI::registerCallbackDynamic(m_handle, "closeWindow",
        [this](void*, std::any data) {
            auto* window = std::any_cast<CWindow*>(data);
            onWindowClose(window);
        });
    
    m_windowFocusHook = HyprlandAPI::registerCallbackDynamic(m_handle, "activeWindow",
        [this](void*, std::any data) {
            auto* window = std::any_cast<CWindow*>(data);
            onWindowFocus(window);
        });
    
    m_workspaceChangeHook = HyprlandAPI::registerCallbackDynamic(m_handle, "workspace",
        [this](void*, std::any) {
            onWorkspaceChange();
        });
}

void Stack3DPlugin::cleanupHooks() {
    // Hooks are automatically cleaned up by Hyprland when plugin unloads
}

void Stack3DPlugin::onWindowOpen(CWindow* window) {
    updateManagedWindows();
    // Optionally trigger re-layout if in stack mode
}

void Stack3DPlugin::onWindowClose(CWindow* window) {
    m_physicsMotion->removeWindow(window);
    updateManagedWindows();
}

void Stack3DPlugin::onWindowFocus(CWindow* window) {
    // Optionally bring focused window to front in stack mode
}

void Stack3DPlugin::onWorkspaceChange() {
    updateManagedWindows();
    // Reset to spread layout on workspace change
    if (m_currentState == StackState::STACKED_3D) {
        m_currentState = StackState::SPREAD_LAYOUT;
    }
}

void Stack3DPlugin::onConfigReload() {
    loadConfig();
    
    // Update subsystems with new config
    m_layoutCalculator->updateTransform(m_config.perspective, m_config.eyeDistance);
    m_layoutCalculator->setDepthStep(m_config.stackDepthStep);
    m_layoutCalculator->setPadding(m_config.spreadPadding);
    
    m_animationSystem->setDuration(m_config.transitionDuration);
    m_animationSystem->setMaxStagger(m_config.staggerDelay);
    
    m_physicsMotion->setGlobalSpring(m_config.springStrength);
    m_physicsMotion->setGlobalDamping(m_config.damping);
}

void Stack3DPlugin::cycleLayoutType() {
    int currentLayout = static_cast<int>(m_config.defaultLayout);
    currentLayout = (currentLayout + 1) % 4; // 4 layout types
    m_config.defaultLayout = static_cast<LayoutType>(currentLayout);
    
    // If currently in spread mode, update immediately
    if (m_currentState == StackState::SPREAD_LAYOUT) {
        transitionToState(StackState::SPREAD_LAYOUT); // Re-transition with new layout
    }
}

void Stack3DPlugin::temporaryPeek(float duration) {
    if (m_currentState != StackState::STACKED_3D) return;
    
    // TODO: Implement temporary peek functionality
    // This would briefly show spread layout then return to stack
}
```

## src/LayoutCalculator.cpp

```cpp
#include "../include/LayoutCalculator.hpp"
#include <hyprland/src/Compositor.hpp>
#include <algorithm>

LayoutCalculator::LayoutCalculator(float perspective, float eyeDistance, 
                                 float depthStep, float padding)
    : m_transform{perspective, eyeDistance}, m_depthStep(depthStep), m_padding(padding) {}

Vector2D Transform3D::project3DTo2D(Vector3D pos3D, Vector2D screenCenter) const {
    float projectedX = (pos3D.x * perspective) / (eyeDistance + pos3D.z);
    float projectedY = (pos3D.y * perspective) / (eyeDistance + pos3D.z);
    
    return {screenCenter.x + projectedX, screenCenter.y + projectedY};
}

float Transform3D::getScaleForDepth(float zPos) const {
    return perspective / (eyeDistance + zPos);
}

std::vector<WindowLayout> LayoutCalculator::calculateStackLayout(const std::vector<CWindow*>& windows) {
    std::vector<WindowLayout> layouts;
    Vector2D stackCenter = getWorkspaceCenter();
    
    for (size_t i = 0; i < windows.size(); i++) {
        WindowLayout layout;
        layout.window = windows[i];
        
        // 3D perspective calculations
        float depth = i * m_depthStep;
        float scale = m_transform.getScaleForDepth(depth);
        
        layout.position = stackCenter + Vector2D{static_cast<float>(i) * 15.0f, static_cast<float>(i) * 10.0f};
        layout.size = windows[i]->m_vRealSize.goal() * scale;
        layout.rotation = i * 2.5f;
        layout.scale = scale;
        layout.alpha = std::max(0.4f, 1.0f - (i * 0.15f));
        layout.zIndex = static_cast<int>(windows.size() - i);
        layout.velocity = {0, 0};
        
        layouts.push_back(layout);
    }
    
    return layouts;
}

std::vector<WindowLayout> LayoutCalculator::calculateSpreadLayout(const std::vector<CWindow*>& windows, 
                                                                 LayoutType layoutType) {
    switch (layoutType) {
        case LayoutType::CIRCULAR:
            return calculateCircularLayout(windows);
        case LayoutType::SPIRAL:
            return calculateSpiralLayout(windows);
        case LayoutType::FIBONACCI:
            return calculateFibonacciLayout(windows);
        case LayoutType::GRID:
        default:
            return calculateGridLayout(windows);
    }
}

std::vector<WindowLayout> LayoutCalculator::calculateGridLayout(const std::vector<CWindow*>& windows) {
    std::vector<WindowLayout> layouts;
    if (windows.empty()) return layouts;
    
    GridLayout grid = calculateOptimalGrid(static_cast<int>(windows.size()));
    Vector2D workspaceSize = getWorkspaceSize();
    Vector2D cellSize = {
        workspaceSize.x / grid.cols,
        workspaceSize.y / grid.rows
    };
    
    for (size_t i = 0; i < windows.size(); i++) {
        WindowLayout layout;
        layout.window = windows[i];
        
        int row = static_cast<int>(i) / grid.cols;
        int col = static_cast<int>(i) % grid.cols;
        
        Vector2D cellPos = {
            col * cellSize.x + m_padding,
            row * cellSize.y + m_padding
        };
        
        layout.position = cellPos;
        layout.size = {
            cellSize.x - (m_padding * 2),
            cellSize.y - (m_padding * 2)
        };
        layout.rotation = 0.0f;
        layout.scale = 1.0f;
        layout.alpha = 1.0f;
        layout.zIndex = 1;
        layout.velocity = {0, 0};
        
        layouts.push_back(layout);
    }
    
    return layouts;
}

std::vector<WindowLayout> LayoutCalculator::calculateCircularLayout(const std::vector<CWindow*>& windows) {
    std::vector<WindowLayout> layouts;
    if (windows.empty()) return layouts;
    
    Vector2D center = getWorkspaceCenter();
    float radius = std::min(getWorkspaceSize().x, getWorkspaceSize().y) * 0.3f;
    
    for (size_t i = 0; i < windows.size(); i++) {
        WindowLayout layout;
        layout.window = windows[i];
        
        float angle = (2.0f * M_PI * i) / windows.size();
        
        layout.position = center + Vector2D{
            radius * cos(angle),
            radius * sin(angle)
        };
        
        Vector2D defaultSize = {300, 200}; // Default window size for circular layout
        layout.size = defaultSize;
        layout.rotation = angle * 180.0f / M_PI + 90.0f;
        layout.scale = 1.0f;
        layout.alpha = 1.0f;
        layout.zIndex = 1;
        layout.velocity = {0, 0};
        
        layouts.push_back(layout);
    }
    
    return layouts;
}

std::vector<WindowLayout> LayoutCalculator::calculateSpiralLayout(const std::vector<CWindow*>& windows) {
    std::vector<WindowLayout> layouts;
    if (windows.empty()) return layouts;
    
    Vector2D center = getWorkspaceCenter();
    
    for (size_t i = 0; i < windows.size(); i++) {
        WindowLayout layout;
        layout.window = windows[i];
        
        float t = static_cast<float>(i) / windows.size();
        float angle = t * 4.0f * M_PI; // 2 full rotations
        float radius = t * 300.0f;     // Expanding radius
        
        layout.position = center + Vector2D{
            radius * cos(angle),
            radius * sin(angle)
        };
        
        Vector2D defaultSize = {250, 180};
        layout.size = defaultSize;
        layout.rotation = angle * 180.0f / M_PI;
        layout.scale = 1.0f - (t * 0.3f); // Gradually smaller
        layout.alpha = 1.0f;
        layout.zIndex = 1;
        layout.velocity = {0, 0};
        
        layouts.push_back(layout);
    }
    
    return layouts;
}

std::vector<WindowLayout> LayoutCalculator::calculateFibonacciLayout(const std::vector<CWindow*>& windows) {
    std::vector<WindowLayout> layouts;
    if (windows.empty()) return layouts;
    
    // Golden ratio based layout
    const float phi = 1.618033988749f;
    Vector2D workspaceSize = getWorkspaceSize();
    Vector2D currentPos = {0, 0};
    Vector2D currentSize = workspaceSize;
    
    for (size_t i = 0; i < windows.size(); i++) {
        WindowLayout layout;
        layout.window = windows[i];
        
        bool splitVertically = (i % 2 == 0);
        
        if (splitVertically) {
            float splitRatio = 1.0f / phi;
            layout.size = {currentSize.x * splitRatio, currentSize.y};
            layout.position = currentPos;
            
            currentPos.x += layout.size.x;
            currentSize.x -= layout.size.x;
        } else {
            float splitRatio = 1.0f / phi;
            layout.size = {currentSize.x, currentSize.y * splitRatio};
            layout.position = currentPos;
            
            currentPos.y += layout.size.y;
            currentSize.y -= layout.size.y;
        }
        
        layout.rotation = 0.0f;
        layout.scale = 1.0f;
        layout.alpha = 1.0f;
        layout.zIndex = 1;
        layout.velocity = {0, 0};
        
        layouts.push_back(layout);
    }
    
    return layouts;
}

GridLayout LayoutCalculator::calculateOptimalGrid(int windowCount) {
    if (windowCount <= 1) return {1, 1};
    if (windowCount <= 4) return {2, 2};
    if (windowCount <= 6) return {2, 3};
    if (windowCount <= 9) return {3, 3};
    if (windowCount <= 12) return {3, 4};
    
    float aspectRatio = getWorkspaceAspectRatio();
    int cols = static_cast<int>(std::ceil(std::sqrt(windowCount * aspectRatio)));
    int rows = static_cast<int>(std::ceil(static_cast<float>(windowCount) / cols));
    
    return {rows, cols};
}

Vector2D LayoutCalculator::getWorkspaceCenter() {
    auto* monitor = g_pCompositor->m_pLastMonitor;
    if (!monitor) return {0, 0};
    
    return {
        monitor->vecSize.x / 2.0f,
        monitor->vecSize.y / 2.0f
    };
}

Vector2D LayoutCalculator::getWorkspaceSize() {
    auto* monitor = g_pCompositor->m_pLastMonitor;
    if (!monitor) return {1920, 1080};
    
    return monitor->vecSize;
}

float LayoutCalculator::getWorkspaceAspectRatio() {
    Vector2D size = getWorkspaceSize();
    return size.x / size.y;
}

void LayoutCalculator::updateTransform(float perspective, float eyeDistance) {
    m_transform.perspective = perspective;
    m_transform.eyeDistance = eyeDistance;
}
```

## Build Instructions

1. **Prerequisites:**
```bash
# Install dependencies (Arch Linux)
sudo pacman -S hyprland-headers gcc make pkg-config

# For other distributions, install equivalent packages
```

2. **Clone and Build:**
```bash
git clone <your-repo-url> hyprland-stack3d
cd hyprland-stack3d
make
```

3. **Install:**
```bash
make install
# Or manually:
# cp stack3d.so ~/.config/hypr/plugins/
```

4. **Configure Hyprland:**
Add to your `~/.config/hypr/hyprland.conf`:

```bash
# Load the plugin
plugin = ~/.config/hypr/plugins/stack3d.so

# Plugin configuration
plugin {
    stack3d {
        enable = true
        transition_duration = 0.8
        stagger_delay = 0.05
        transition_style = "smooth_slide"
        stack_depth_step = 100
        spread_padding = 20
        default_layout = "grid"
        spring_strength = 0.8
        damping = 0.92
        motion_blur = true
        perspective = 800
        eye_distance = 1000
        toggle_key = "SUPER, grave"
        peek_key = "SUPER, space"
        cycle_layout_key = "SUPER SHIFT, grave"
    }
}
```

## Usage

- **`SUPER + grave`**: Toggle between 3D stack and spread layout
- **`SUPER + space`**: Temporary peek (when in stack mode)
- **`SUPER + SHIFT + grave`**: Cycle through layout types (grid, circular, spiral, fibonacci)

## Configuration Options

- `enable`: Enable/disable the plugin
- `transition_duration`: Animation duration in seconds
- `stagger_delay`: Delay between window animations
- `transition_style`: Animation style (smooth_slide, bounce_in, elastic_out, cascade_wave)
- `stack_depth_step`: Distance between stack layers
- `spread_padding`: Padding between windows in spread mode
- `default_layout`: Layout type for spread mode
- `spring_strength`: Physics spring strength
- `damping`: Physics damping factor
- `motion_blur`: Enable motion blur effects
- `perspective`: 3D perspective strength
- `eye_distance`: Camera distance for 3D effect

This plugin provides a complete 3D stack sliding animation system for Hyprland with smooth transitions, multiple layout options, and extensive customization capabilities.