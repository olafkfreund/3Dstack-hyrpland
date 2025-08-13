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
    
    WindowTransition() : window(nullptr), startPos{0, 0}, startSize{0, 0}, 
                        startRotation(0), startScale(1), startAlpha(1),
                        endPos{0, 0}, endSize{0, 0}, endRotation(0), 
                        endScale(1), endAlpha(1), startDelay(0),
                        easingCurve(0.25f, 0.1f, 0.25f, 1.0f), isComplete(false) {}
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

class AnimationSystem {
private:
    std::vector<WindowTransition> m_transitions;
    std::chrono::steady_clock::time_point m_animationStart;
    float m_baseDuration;
    float m_maxStagger;
    bool m_isAnimating;
    
    std::function<void()> m_onComplete;
    
public:
    explicit AnimationSystem(float baseDuration = 0.8f, float maxStagger = 0.3f);
    
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
    // void addMotionBlur(CWindow* window, Vector2D velocity);  // TODO: Implement motion blur
    Vector2D calculateVelocity(const WindowTransition& transition, float progress);
    
    // Interpolation helpers
    template<typename T>
    T lerp(const T& start, const T& end, float t) {
        return start + (end - start) * t;
    }
};