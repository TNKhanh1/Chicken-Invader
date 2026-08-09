#ifndef VERTICALZIGZAGMOVEMENT_H
#define VERTICALZIGZAGMOVEMENT_H

#include "IMovementBehavior.h"

class VerticalZigzagMovement : public IMovementBehavior {
private:
    float startX;
    bool initialized = false;
    float dx = 1.0f;
    bool wrapAround;
public:
    VerticalZigzagMovement(bool wrap = true) : wrapAround(wrap) {}
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};

#endif
