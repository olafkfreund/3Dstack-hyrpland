#include "../include/LayoutCalculator.hpp"
#include <src/Compositor.hpp>
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
        layout.size = windows[i]->m_realSize->goal() * scale;
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
    if (windowCount <= 1) return {1, 1, {0, 0}};
    if (windowCount <= 4) return {2, 2, {0, 0}};
    if (windowCount <= 6) return {2, 3, {0, 0}};
    if (windowCount <= 9) return {3, 3, {0, 0}};
    if (windowCount <= 12) return {3, 4, {0, 0}};
    
    float aspectRatio = getWorkspaceAspectRatio();
    int cols = static_cast<int>(std::ceil(std::sqrt(windowCount * aspectRatio)));
    int rows = static_cast<int>(std::ceil(static_cast<float>(windowCount) / cols));
    
    return {rows, cols, {0, 0}};
}

Vector2D LayoutCalculator::getWorkspaceCenter() {
    // For now, use default screen size - in a real implementation,
    // we would access monitor information through proper Hyprland APIs
    Vector2D size = getWorkspaceSize();
    return {size.x / 2.0f, size.y / 2.0f};
}

Vector2D LayoutCalculator::getWorkspaceSize() {
    // For now, return a default resolution
    // In a real implementation, we would access the monitor through proper APIs
    return {1920, 1080};
}

float LayoutCalculator::getWorkspaceAspectRatio() {
    Vector2D size = getWorkspaceSize();
    return size.x / size.y;
}

void LayoutCalculator::updateTransform(float perspective, float eyeDistance) {
    m_transform.perspective = perspective;
    m_transform.eyeDistance = eyeDistance;
}