#pragma once

#include <vector>
#include <string>
#include <functional>
#include <any>
#include <memory>
#include <unordered_map>
#include <iostream>

// Mock implementation of Hyprland API for testing
namespace HyprlandMocks {

// Forward declarations
class MockWindow;
class MockWorkspace;
class MockMonitor;

// Mock Vector2D
struct Vector2D {
    double x, y;
    
    Vector2D() : x(0), y(0) {}
    Vector2D(double x_, double y_) : x(x_), y(y_) {}
    
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }
    
    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }
    
    Vector2D operator*(double scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }
    
    bool operator==(const Vector2D& other) const {
        return std::abs(x - other.x) < 1e-6 && std::abs(y - other.y) < 1e-6;
    }
};

// Mock CBox (geometry)
struct CBox {
    double x, y, width, height;
    
    CBox() : x(0), y(0), width(0), height(0) {}
    CBox(double x_, double y_, double w_, double h_) : x(x_), y(y_), width(w_), height(h_) {}
    
    Vector2D pos() const { return Vector2D(x, y); }
    Vector2D size() const { return Vector2D(width, height); }
};

// Mock Window class
class MockWindow {
public:
    MockWindow(const std::string& title = "Test Window", const std::string& className = "TestClass") 
        : m_title(title), m_className(className), m_workspace(nullptr), m_monitor(nullptr),
          m_position(0, 0), m_size(800, 600), m_floating(false), m_fullscreen(false) {}
    
    // Properties
    Vector2D m_vRealPosition;
    Vector2D m_vRealSize;
    bool m_bMapped = true;
    bool m_bHidden = false;
    bool m_bFakeFullscreen = false;
    
    // Getters
    std::string getTitle() const { return m_title; }
    std::string getClass() const { return m_className; }
    Vector2D getPosition() const { return m_position; }
    Vector2D getSize() const { return m_size; }
    bool isFloating() const { return m_floating; }
    bool isFullscreen() const { return m_fullscreen; }
    MockWorkspace* getWorkspace() const { return m_workspace; }
    MockMonitor* getMonitor() const { return m_monitor; }
    
    // Setters
    void setPosition(const Vector2D& pos) { m_position = pos; m_vRealPosition = pos; }
    void setSize(const Vector2D& size) { m_size = size; m_vRealSize = size; }
    void setFloating(bool floating) { m_floating = floating; }
    void setFullscreen(bool fullscreen) { m_fullscreen = fullscreen; }
    void setWorkspace(MockWorkspace* workspace) { m_workspace = workspace; }
    void setMonitor(MockMonitor* monitor) { m_monitor = monitor; }
    
    // Geometry
    CBox getGeometry() const {
        return CBox(m_position.x, m_position.y, m_size.x, m_size.y);
    }
    
private:
    std::string m_title;
    std::string m_className;
    Vector2D m_position;
    Vector2D m_size;
    bool m_floating;
    bool m_fullscreen;
    MockWorkspace* m_workspace;
    MockMonitor* m_monitor;
};

// Mock Workspace class
class MockWorkspace {
public:
    MockWorkspace(int id, const std::string& name = "") 
        : m_id(id), m_name(name.empty() ? std::to_string(id) : name) {}
    
    int getID() const { return m_id; }
    std::string getName() const { return m_name; }
    
    void addWindow(MockWindow* window) {
        m_windows.push_back(window);
        window->setWorkspace(this);
    }
    
    void removeWindow(MockWindow* window) {
        m_windows.erase(std::remove(m_windows.begin(), m_windows.end(), window), m_windows.end());
    }
    
    const std::vector<MockWindow*>& getWindows() const { return m_windows; }
    
private:
    int m_id;
    std::string m_name;
    std::vector<MockWindow*> m_windows;
};

// Mock Monitor class
class MockMonitor {
public:
    MockMonitor(const std::string& name, int width = 1920, int height = 1080)
        : m_name(name), m_size(width, height), m_position(0, 0) {}
    
    std::string getName() const { return m_name; }
    Vector2D getSize() const { return m_size; }
    Vector2D getPosition() const { return m_position; }
    
    void setPosition(const Vector2D& pos) { m_position = pos; }
    
    CBox getGeometry() const {
        return CBox(m_position.x, m_position.y, m_size.x, m_size.y);
    }
    
private:
    std::string m_name;
    Vector2D m_size;
    Vector2D m_position;
};

// Mock Hyprland API
class MockHyprlandAPI {
public:
    static MockHyprlandAPI& getInstance() {
        static MockHyprlandAPI instance;
        return instance;
    }
    
    // Window management
    void addWindow(MockWindow* window) {
        m_windows.push_back(window);
    }
    
    void removeWindow(MockWindow* window) {
        m_windows.erase(std::remove(m_windows.begin(), m_windows.end(), window), m_windows.end());
    }
    
    std::vector<MockWindow*> getWindows() const {
        return m_windows;
    }
    
    std::vector<MockWindow*> getWindowsOnWorkspace(int workspaceId) const {
        std::vector<MockWindow*> result;
        for (auto* window : m_windows) {
            if (window->getWorkspace() && window->getWorkspace()->getID() == workspaceId) {
                result.push_back(window);
            }
        }
        return result;
    }
    
    // Workspace management
    void addWorkspace(MockWorkspace* workspace) {
        m_workspaces.push_back(workspace);
    }
    
    MockWorkspace* getActiveWorkspace() const {
        return m_activeWorkspace;
    }
    
    void setActiveWorkspace(MockWorkspace* workspace) {
        m_activeWorkspace = workspace;
    }
    
    // Monitor management
    void addMonitor(MockMonitor* monitor) {
        m_monitors.push_back(monitor);
    }
    
    MockMonitor* getActiveMonitor() const {
        return m_activeMonitor;
    }
    
    void setActiveMonitor(MockMonitor* monitor) {
        m_activeMonitor = monitor;
    }
    
    // Event callbacks
    using CallbackFn = std::function<void(void*, std::any)>;
    
    void registerCallback(const std::string& event, CallbackFn callback) {
        m_callbacks[event].push_back(callback);
    }
    
    void triggerEvent(const std::string& event, std::any data) {
        if (m_callbacks.find(event) != m_callbacks.end()) {
            for (const auto& callback : m_callbacks[event]) {
                callback(nullptr, data);
            }
        }
    }
    
    // Configuration
    void setConfig(const std::string& key, const std::string& value) {
        m_config[key] = value;
    }
    
    std::string getConfig(const std::string& key) const {
        auto it = m_config.find(key);
        return it != m_config.end() ? it->second : "";
    }
    
    // Cleanup
    void reset() {
        m_windows.clear();
        m_workspaces.clear();
        m_monitors.clear();
        m_callbacks.clear();
        m_config.clear();
        m_activeWorkspace = nullptr;
        m_activeMonitor = nullptr;
    }
    
private:
    std::vector<MockWindow*> m_windows;
    std::vector<MockWorkspace*> m_workspaces;
    std::vector<MockMonitor*> m_monitors;
    std::unordered_map<std::string, std::vector<CallbackFn>> m_callbacks;
    std::unordered_map<std::string, std::string> m_config;
    MockWorkspace* m_activeWorkspace = nullptr;
    MockMonitor* m_activeMonitor = nullptr;
};

// Type aliases to match Hyprland API
using CWindow = MockWindow;
using CWorkspace = MockWorkspace;
using CMonitor = MockMonitor;

// Template for shared pointer (simplified)
template<typename T>
using SP = std::shared_ptr<T>;

// Mock HOOK_CALLBACK_FN (simplified)
struct HOOK_CALLBACK_FN {
    std::function<void(void*, std::any)> fn;
};

} // namespace HyprlandMocks

// Make types available globally for tests
using HyprlandMocks::Vector2D;
using HyprlandMocks::CBox;
using HyprlandMocks::CWindow;
using HyprlandMocks::CWorkspace;
using HyprlandMocks::CMonitor;
using HyprlandMocks::SP;
using HyprlandMocks::HOOK_CALLBACK_FN;