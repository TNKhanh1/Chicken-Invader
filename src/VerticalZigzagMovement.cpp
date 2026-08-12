#include "VerticalZigzagMovement.h"
#include <cmath>

void VerticalZigzagMovement::Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) {
    if (!initialized) {
        startX = position.x;
        initialized = true;
    }
    position.y += moveSpeed * downwardSpeedMult * deltaTime;
    position.x += dx * moveSpeed * horizontalSpeedMult * deltaTime;
    if (position.x > startX + 250.0f) {
        dx = -1.0f;
    } else if (position.x < startX - 250.0f) {
        dx = 1.0f;
    }
    if (wrapAround && position.y > screenHeight + 100) {
        position.y = -100;
    }
}
