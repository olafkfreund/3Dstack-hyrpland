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