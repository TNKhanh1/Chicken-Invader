#ifndef HORIZONTALBOUNCEMOVEMENT_H
#define HORIZONTALBOUNCEMOVEMENT_H

#include "IMovementBehavior.h"

class HorizontalBounceMovement : public IMovementBehavior {
private:
    float startX;
    float drift;
    float direction;
    float targetY;
    bool initialized = false;
public:
    HorizontalBounceMovement(float targetY, float drift = 300.0f, float startDir = 1.0f);
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};

#endif
