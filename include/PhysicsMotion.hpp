#pragma once

#include <unordered_map>
#include <src/desktop/Window.hpp>
#include <src/helpers/math/Math.hpp>

struct MotionState
{
    Vector2D velocity;
    Vector2D acceleration;
    float angularVelocity;
    float angularAcceleration;
    float spring;
    float damping;
    float mass;

    MotionState() :
        velocity{0, 0},
        acceleration{0, 0},
        angularVelocity(0),
        angularAcceleration(0),
        spring(0.8f),
        damping(0.92f),
        mass(1.0f) {
    }
};

class PhysicsMotion
{
private:
    std::unordered_map<CWindow *, MotionState> m_windowMotions;
    float m_globalDamping;
    float m_globalSpring;

public:
    explicit PhysicsMotion(float globalSpring = 0.8f,
                           float globalDamping = 0.92f);

    // Force application
    void addSpringForce(CWindow *window,
                        Vector2D targetPos,
                        Vector2D currentPos);
    void addDampingForce(CWindow *window);
    void addGravityForce(CWindow *window,
                         Vector2D gravityCenter,
                         float strength);
    void addRepulsionForce(CWindow *window,
                           const std::vector<CWindow *> &otherWindows,
                           float strength);

    // Motion calculation
    Vector2D getNextPosition(CWindow *window,
                             Vector2D currentPos,
                             float deltaTime);
    float getNextRotation(CWindow *window,
                          float currentRotation,
                          float deltaTime);
    Vector2D getVelocity(CWindow *window) const;

    // State management
    void resetMotion(CWindow *window);
    void setMotionProperties(CWindow *window,
                             float spring,
                             float damping,
                             float mass = 1.0f);
    void updateMotion(float deltaTime);

    // Configuration
    void setGlobalSpring(float spring) {
        m_globalSpring = spring;
    }

    void setGlobalDamping(float damping) {
        m_globalDamping = damping;
    }

    // Cleanup
    void removeWindow(CWindow *window);
    void clear();

private:
    MotionState &getOrCreateMotionState(CWindow *window);
    void applyForces(MotionState &motion,
                     float deltaTime);
}; // class PhysicsMotion
