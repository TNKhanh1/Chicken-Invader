#include "MeteorDiveMovement.h"
#include <cmath>

void MeteorDiveMovement::Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) {
    position.y += moveSpeed * 3.0f * deltaTime;
}
