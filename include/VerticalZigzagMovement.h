#ifndef VERTICALZIGZAGMOVEMENT_H
#define VERTICALZIGZAGMOVEMENT_H

#include "IMovementBehavior.h"

class VerticalZigzagMovement : public IMovementBehavior {
private:
    float startX;
    bool initialized = false;
    float dx = 1.0f;
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};

#endif
