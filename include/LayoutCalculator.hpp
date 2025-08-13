#pragma once

#include <vector>
#include <cmath>
#include <hyprland/src/Window.hpp>
#include <hyprland/src/helpers/Vector2D.hpp>

// Forward declaration for Vector3D (we'll define it here since it's not in Hyprland)
struct Vector3D {
    float x, y, z;
    Vector3D(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
};

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

enum class LayoutType {
    GRID,
    CIRCULAR,
    SPIRAL,
    FIBONACCI
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