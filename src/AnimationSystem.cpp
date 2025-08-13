#include "../include/AnimationSystem.hpp"
#include <src/Compositor.hpp>
#include <algorithm>

AnimationSystem::AnimationSystem(float baseDuration, float maxStagger)
    : m_baseDuration(baseDuration), m_maxStagger(maxStagger), m_isAnimating(false) {}

void AnimationSystem::startTransition(const std::vector<WindowLayout>& startLayouts,
                                    const std::vector<WindowLayout>& endLayouts,
                                    TransitionStyle style,
                                    std::function<void()> onComplete) {
    if (m_isAnimating) {
        stopAnimation();
    }
    
    m_transitions.clear();
    m_onComplete = onComplete;
    
    size_t numWindows = std::min(startLayouts.size(), endLayouts.size());
    
    for (size_t i = 0; i < numWindows; i++) {
        WindowTransition transition;
        transition.window = startLayouts[i].window;
        
        // Set start state
        transition.startPos = startLayouts[i].position;
        transition.startSize = startLayouts[i].size;
        transition.startRotation = startLayouts[i].rotation;
        transition.startScale = startLayouts[i].scale;
        transition.startAlpha = startLayouts[i].alpha;
        
        // Set end state
        transition.endPos = endLayouts[i].position;
        transition.endSize = endLayouts[i].size;
        transition.endRotation = endLayouts[i].rotation;
        transition.endScale = endLayouts[i].scale;
        transition.endAlpha = endLayouts[i].alpha;
        
        // Calculate stagger delay
        transition.startDelay = (static_cast<float>(i) / numWindows) * m_maxStagger;
        
        // Set easing curve
        transition.easingCurve = createEasingCurve(style);
        transition.isComplete = false;
        
        m_transitions.push_back(transition);
    }
    
    m_animationStart = std::chrono::steady_clock::now();
    m_isAnimating = true;
}

void AnimationSystem::updateAnimation() {
    if (!m_isAnimating) return;
    
    auto now = std::chrono::steady_clock::now();
    float totalElapsed = std::chrono::duration<float>(now - m_animationStart).count();
    
    bool allComplete = true;
    
    for (auto& transition : m_transitions) {
        if (transition.isComplete) continue;
        
        float elapsed = totalElapsed - transition.startDelay;
        if (elapsed < 0) {
            allComplete = false;
            continue;
        }
        
        float progress = elapsed / m_baseDuration;
        
        if (progress >= 1.0f) {
            progress = 1.0f;
            transition.isComplete = true;
        } else {
            allComplete = false;
        }
        
        // Apply easing
        float easedProgress = transition.easingCurve.evaluate(progress);
        
        // Interpolate layout properties
        WindowLayout currentLayout;
        currentLayout.window = transition.window;
        currentLayout.position = lerp(transition.startPos, transition.endPos, easedProgress);
        currentLayout.size = lerp(transition.startSize, transition.endSize, easedProgress);
        currentLayout.rotation = lerp(transition.startRotation, transition.endRotation, easedProgress);
        currentLayout.scale = lerp(transition.startScale, transition.endScale, easedProgress);
        currentLayout.alpha = lerp(transition.startAlpha, transition.endAlpha, easedProgress);
        currentLayout.velocity = calculateVelocity(transition, progress);
        
        // Apply the transformation to the window
        applyWindowTransform(transition.window, currentLayout);
    }
    
    if (allComplete) {
        m_isAnimating = false;
        if (m_onComplete) {
            m_onComplete();
        }
    }
}

void AnimationSystem::stopAnimation() {
    m_isAnimating = false;
    m_transitions.clear();
}

float AnimationSystem::getProgress() const {
    if (!m_isAnimating || m_transitions.empty()) return 1.0f;
    
    auto now = std::chrono::steady_clock::now();
    float totalElapsed = std::chrono::duration<float>(now - m_animationStart).count();
    
    return std::min(1.0f, totalElapsed / (m_baseDuration + m_maxStagger));
}

BezierCurve AnimationSystem::createEasingCurve(TransitionStyle style) {
    switch (style) {
        case TransitionStyle::BOUNCE_IN:
            return BezierCurve::bounce();
        case TransitionStyle::ELASTIC_OUT:
            return BezierCurve::elastic();
        case TransitionStyle::CASCADE_WAVE:
            return BezierCurve::easeInOut();
        case TransitionStyle::SPIRAL_MOTION:
            return BezierCurve::easeOut();
        case TransitionStyle::MAGNETIC_ATTRACT:
            return BezierCurve::easeIn();
        case TransitionStyle::LIQUID_FLOW:
            return BezierCurve(0.25f, 0.46f, 0.45f, 0.94f);
        case TransitionStyle::SMOOTH_SLIDE:
        default:
            return BezierCurve::easeOut();
    }
}

void AnimationSystem::applyWindowTransform(CWindow* window, const WindowLayout& layout) {
    if (!window) return;
    
    // Apply position using animated variable setValueAndWarp
    window->m_realPosition->setValueAndWarp(layout.position);
    
    // Apply size using animated variable setValueAndWarp
    window->m_realSize->setValueAndWarp(layout.size);
    
    // For alpha, we would need to use Hyprland's rendering system
    // This is a simplified version - actual implementation would need
    // to integrate with Hyprland's renderer
    
    // Motion blur would be applied here if enabled
    if (layout.velocity.x != 0 || layout.velocity.y != 0) {
        // addMotionBlur(window, layout.velocity);
    }
}

void AnimationSystem::addMotionBlur(CWindow* window, Vector2D velocity) {
    // Motion blur implementation would integrate with Hyprland's rendering pipeline
    // This is a placeholder for the actual implementation
    (void)window; // Suppress unused parameter warning
    (void)velocity;
}

Vector2D AnimationSystem::calculateVelocity(const WindowTransition& transition, float progress) {
    if (progress <= 0.0f || progress >= 1.0f) {
        return {0, 0};
    }
    
    // Calculate velocity based on position change
    float dt = 0.016f; // Assume 60 FPS
    float futureProgress = std::min(1.0f, progress + dt / m_baseDuration);
    
    float currentEased = transition.easingCurve.evaluate(progress);
    float futureEased = transition.easingCurve.evaluate(futureProgress);
    
    Vector2D currentPos = lerp(transition.startPos, transition.endPos, currentEased);
    Vector2D futurePos = lerp(transition.startPos, transition.endPos, futureEased);
    
    return (futurePos - currentPos) / dt;
}