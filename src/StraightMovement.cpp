#include "StraightMovement.h"
#include <cmath>

void StraightMovement::Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) {
    position.y += moveSpeed * deltaTime;
}
