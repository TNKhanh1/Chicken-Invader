#include "HorizontalBounceMovement.h"
#include <cmath>

HorizontalBounceMovement::HorizontalBounceMovement(float targetY, float drift, float startDir) 
    : targetY(targetY), drift(drift), direction(startDir) {}

void HorizontalBounceMovement::Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) {
    if (!initialized) {
        startX = position.x;
        initialized = true;
    }
    if (position.y < targetY) {
        position.y += moveSpeed * deltaTime;
    } else {
        position.x += direction * moveSpeed * deltaTime;
        if (direction > 0 && position.x > startX + drift) {
            direction = -1.0f;
        } else if (direction < 0 && position.x < startX - drift) {
            direction = 1.0f;
        }
    }
}
