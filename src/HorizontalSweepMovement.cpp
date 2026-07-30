#include "HorizontalSweepMovement.h"
#include <cmath>

HorizontalSweepMovement::HorizontalSweepMovement(float startDir) : direction(startDir) {}

void HorizontalSweepMovement::Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) {
    position.x += direction * moveSpeed * deltaTime;
    if (direction > 0 && position.x > screenWidth + 100) {
        position.x = -100;
    } else if (direction < 0 && position.x < -100) {
        position.x = screenWidth + 100;
    }
}
