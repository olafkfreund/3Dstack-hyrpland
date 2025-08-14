#include "../include/BezierCurve.hpp"
#include <cmath>
#include <algorithm>

BezierCurve::BezierCurve(float x1,
                         float y1,
                         float x2,
                         float y2)
    : m_x1(x1),
    m_y1(y1),
    m_x2(x2),
    m_y2(y2) {
}

float BezierCurve::evaluate(float t) const
{
    // Clamp t to [0, 1]
    t = std::clamp(t,
                   0.0f,
                   1.0f);

    // For the Y value given X=t, we need to solve the cubic bezier
    return cubicBezier(t,
                       m_y1,
                       m_y2);
}

float BezierCurve::cubicBezier(float t,
                               float p1,
                               float p2) const
{
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    // Note: uuu would be used for more complex Bezier calculations
    // float uuu = uu * u;
    float ttt = tt * t;

    // Cubic Bezier formula: B(t) = (1-t)³P₀ + 3(1-t)²tP₁ + 3(1-t)t²P₂ + t³P₃
    // For standard easing curves P₀ = 0, P₃ = 1
    return 3 * uu * t * p1 + 3 * u * tt * p2 + ttt;
}

// Cubic solver implementation commented out for now - TODO: implement when needed
// float BezierCurve::solveCubicBezier(float x) const {
//// Newton-Raphson method to solve for t given x
//// This is a simplified version - for production use, consider a more robust implementation
// float t = x; // Initial guess
//
// for (int i = 0; i < 8; i++) {
// float currentX = cubicBezier(t, m_x1, m_x2);
// float derivative = 3 * (1 - t) * (1 - t) * m_x1 + 6 * (1 - t) * t * (m_x2 - m_x1) + 3 * t * t * (1 - m_x2);
//
// if (std::abs(derivative) < 1e-6f) break;
//
// t = t - (currentX - x) / derivative;
// t = std::clamp(t, 0.0f, 1.0f);
// }
//
// return t;
// }
