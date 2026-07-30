#include "SpiralMovement.h"
#include <cmath>

void SpiralMovement::Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) {
    if (!initialized) {
        center = position;
        initialized = true;
    }
    time += deltaTime;
    center.y += moveSpeed * 0.4f * deltaTime;
    if (radius < maxRadius) {
        radius += 50.0f * deltaTime;
    }
    position.x = center.x + std::cos(time * rotationSpeed) * radius;
    position.y = center.y + std::sin(time * rotationSpeed) * radius;
}
