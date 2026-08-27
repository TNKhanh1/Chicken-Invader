#ifndef BULLET_TRAJECTORY_H
#define BULLET_TRAJECTORY_H

#include "raylib.h"

// Strategy Interface cho quỹ đạo bay của đạn
class IBulletTrajectory {
public:
    virtual ~IBulletTrajectory() = default;
    
    // Hàm cập nhật vị trí và góc bay của đạn
    virtual void UpdatePosition(Vector2& pos, float& angle, float speed, float deltaTime) = 0;
};

// Quỹ đạo bay thẳng (Neutron Gun, v.v.)
class StraightTrajectory : public IBulletTrajectory {
private:
    Vector2 direction;
public:
    StraightTrajectory(Vector2 dir);
    void UpdatePosition(Vector2& pos, float& angle, float speed, float deltaTime) override;
};

// Quỹ đạo xòe quạt theo góc (Riddler, Ion Blaster)
class SpreadTrajectory : public IBulletTrajectory {
private:
    float angleDegrees;
public:
    SpreadTrajectory(float angleDeg);
    void UpdatePosition(Vector2& pos, float& angle, float speed, float deltaTime) override;
};

// Quỹ đạo dao động hình sin (Hypergun, Utensil Poker)
class OscillatingTrajectory : public IBulletTrajectory {
private:
    float baseAngle;
    float frequency;
    float amplitude;
    float elapsedTime;
public:
    OscillatingTrajectory(float baseAngleDeg, float freq, float amp);
    void UpdatePosition(Vector2& pos, float& angle, float speed, float deltaTime) override;
};

// Quỹ đạo tự tìm mục tiêu (Homing)
class HomingTrajectory : public IBulletTrajectory {
private:
    float turnSpeed; // Radian per second
    float currentAngle;
    bool isInit;
public:
    HomingTrajectory(float initialAngleDeg, float turnSpeedRadSec);
    void UpdatePosition(Vector2& pos, float& angle, float speed, float deltaTime) override;
};

#endif // BULLET_TRAJECTORY_H
