#include "../include/PhysicsMotion.hpp"
#include <algorithm>
#include <cmath>

PhysicsMotion::PhysicsMotion(float globalSpring,
                             float globalDamping)
    : m_globalSpring(globalSpring),
    m_globalDamping(globalDamping) {
}

void PhysicsMotion::addSpringForce(CWindow *window,
                                   Vector2D targetPos,
                                   Vector2D currentPos) {
    MotionState &motion = getOrCreateMotionState(window);

    Vector2D     displacement = targetPos - currentPos;
    Vector2D     springForce = displacement * motion.spring;

    motion.acceleration = motion.acceleration + springForce;
}

void PhysicsMotion::addDampingForce(CWindow *window) {
    MotionState &motion = getOrCreateMotionState(window);

    Vector2D     dampingForce = motion.velocity * (-motion.damping);
    motion.acceleration = motion.acceleration + dampingForce;
}

void PhysicsMotion::addGravityForce(CWindow *window,
                                    Vector2D gravityCenter,
                                    float strength) {
    MotionState &motion = getOrCreateMotionState(window);

    // This would need window's current position - for now we'll store it in the motion state
    // In a real implementation, you'd get current position from the window
    Vector2D gravityDirection = gravityCenter; // Simplified
    Vector2D gravityForce = gravityDirection * strength;

    motion.acceleration = motion.acceleration + gravityForce;
}

void PhysicsMotion::addRepulsionForce(CWindow *window,
                                      const std::vector<CWindow *> &otherWindows,
                                      float strength) {
    MotionState &motion = getOrCreateMotionState(window);

    for (CWindow *other : otherWindows)
    {
        if (other == window)
        {
            continue;
        }

        // Simplified repulsion - would need actual positions
        Vector2D repulsionForce =
        { strength * 0.1f, strength * 0.1f };
        motion.acceleration = motion.acceleration + repulsionForce;
    }
}

Vector2D PhysicsMotion::getNextPosition(CWindow *window,
                                        Vector2D currentPos,
                                        float deltaTime) {
    MotionState &motion = getOrCreateMotionState(window);

    // Apply forces
    applyForces(motion,
                deltaTime);

    // Update velocity: v = v + a*dt
    motion.velocity = motion.velocity + motion.acceleration * deltaTime;

    // Update position: p = p + v*dt
    Vector2D nextPos = currentPos + motion.velocity * deltaTime;

    // Reset acceleration for next frame
    motion.acceleration =
    { 0, 0 };

    return nextPos;
}

float PhysicsMotion::getNextRotation(CWindow *window,
                                     float currentRotation,
                                     float deltaTime) {
    MotionState &motion = getOrCreateMotionState(window);

    // Update angular velocity
    motion.angularVelocity += motion.angularAcceleration * deltaTime;
    motion.angularVelocity *= motion.damping; // Apply damping

    // Update rotation
    float nextRotation = currentRotation + motion.angularVelocity * deltaTime;

    // Reset angular acceleration
    motion.angularAcceleration = 0;

    return nextRotation;
}

Vector2D PhysicsMotion::getVelocity(CWindow *window) const
{
    auto it = m_windowMotions.find(window);
    if (it != m_windowMotions.end())
    {
        return it->second.velocity;
    }
    return { 0, 0 };
}

void PhysicsMotion::resetMotion(CWindow *window) {
    MotionState &motion = getOrCreateMotionState(window);
    motion.velocity =
    { 0, 0 };
    motion.acceleration =
    { 0, 0 };
    motion.angularVelocity = 0;
    motion.angularAcceleration = 0;
}

void PhysicsMotion::setMotionProperties(CWindow *window,
                                        float spring,
                                        float damping,
                                        float mass) {
    MotionState &motion = getOrCreateMotionState(window);
    motion.spring = spring;
    motion.damping = damping;
    motion.mass = mass;
}

void PhysicsMotion::updateMotion(float deltaTime) {
    for (auto & [window, motion] : m_windowMotions)
    {
        applyForces(motion,
                    deltaTime);
    }
}

void PhysicsMotion::removeWindow(CWindow *window) {
    m_windowMotions.erase(window);
}

void PhysicsMotion::clear() {
    m_windowMotions.clear();
}

MotionState &PhysicsMotion::getOrCreateMotionState(CWindow *window) {
    auto it = m_windowMotions.find(window);
    if (it == m_windowMotions.end())
    {
        // Create new motion state with global defaults
        MotionState newMotion;
        newMotion.spring = m_globalSpring;
        newMotion.damping = m_globalDamping;
        m_windowMotions[window] = newMotion;
        return m_windowMotions[window];
    }
    return it->second;
}

void PhysicsMotion::applyForces(MotionState &motion,
                                float deltaTime) {
    // Apply global damping
    motion.velocity = motion.velocity * (1.0f - m_globalDamping * deltaTime);

    // Apply angular damping
    motion.angularVelocity *= (1.0f - m_globalDamping * deltaTime);
}
