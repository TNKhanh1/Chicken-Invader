#include "HorizontalBounceMovement.h"
#include <cmath>

HorizontalBounceMovement::HorizontalBounceMovement(float targetY, float drift, float startDir) 
    : targetY(targetY), drift(drift), direction(startDir), startX(0.0f), initialized(false) {}

void HorizontalBounceMovement::Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) {
    if (!initialized) {
        startX = position.x;
        initialized = true;
    }
    if (position.y < targetY) {
        position.y += moveSpeed * deltaTime;
        if (position.y > targetY) position.y = targetY; // Don't overshoot
    } else {
        if (drift > 0.0f) {
            position.x += direction * moveSpeed * deltaTime;
            if (direction > 0 && position.x > startX + drift) {
                direction = -1.0f;
            } else if (direction < 0 && position.x < startX - drift) {
                direction = 1.0f;
            }
        }
    }
}
