#include "BulletTrajectory.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- StraightTrajectory ---
StraightTrajectory::StraightTrajectory(Vector2 dir) : direction(dir) {
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction.x /= length;
        direction.y /= length;
    } else {
        direction = {0, -1.0f};
    }
}

void StraightTrajectory::UpdatePosition(Vector2& pos, float& angle, float speed, float deltaTime) {
    pos.x += direction.x * speed * deltaTime;
    pos.y += direction.y * speed * deltaTime;
    angle = std::atan2(direction.y, direction.x) * (180.0f / M_PI) + 90.0f;
}

// --- SpreadTrajectory ---
SpreadTrajectory::SpreadTrajectory(float angleDeg) : angleDegrees(angleDeg) {}

void SpreadTrajectory::UpdatePosition(Vector2& pos, float& angle, float speed, float deltaTime) {
    float rad = angleDegrees * (M_PI / 180.0f);
    Vector2 vel = { std::sin(rad) * speed, -std::cos(rad) * speed };
    pos.x += vel.x * deltaTime;
    pos.y += vel.y * deltaTime;
    angle = angleDegrees;
}

// --- OscillatingTrajectory ---
OscillatingTrajectory::OscillatingTrajectory(float baseAngleDeg, float freq, float amp)
    : baseAngle(baseAngleDeg), frequency(freq), amplitude(amp), elapsedTime(0.0f) {}

void OscillatingTrajectory::UpdatePosition(Vector2& pos, float& angle, float speed, float deltaTime) {
    elapsedTime += deltaTime;
    float currentOffsetAngle = amplitude * std::sin(2.0f * M_PI * frequency * elapsedTime);
    float finalAngle = baseAngle + currentOffsetAngle;
    float rad = finalAngle * (M_PI / 180.0f);
    
    Vector2 vel = { std::sin(rad) * speed, -std::cos(rad) * speed };
    pos.x += vel.x * deltaTime;
    pos.y += vel.y * deltaTime;
    angle = finalAngle;
}
