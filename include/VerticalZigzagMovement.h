#ifndef VERTICALZIGZAGMOVEMENT_H
#define VERTICALZIGZAGMOVEMENT_H

#include "IMovementBehavior.h"

class VerticalZigzagMovement : public IMovementBehavior {
private:
    float startX;
    bool initialized = false;
    float dx = 1.0f;
    bool wrapAround;
    float downwardSpeedMult;
    float horizontalSpeedMult;
public:
    VerticalZigzagMovement(bool wrap = true, float downMult = 0.3f, float horizMult = 0.5f) 
        : wrapAround(wrap), downwardSpeedMult(downMult), horizontalSpeedMult(horizMult) {}
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};

#endif
