#include "SineZigzagMovement.h"
#include <cmath>

void SineZigzagMovement::Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) {
    if (!initialized) {
        startX = position.x;
        initialized = true;
    }
    time += deltaTime;
    position.y += moveSpeed * 0.5f * deltaTime;
    position.x = startX + std::sin(time * frequency) * amplitude;
    if (wrapAround && position.y > screenHeight + 100) {
        position.y = -100;
    }
}
