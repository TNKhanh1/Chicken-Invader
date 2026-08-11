#ifndef SINEZIGZAGMOVEMENT_H
#define SINEZIGZAGMOVEMENT_H

#include "IMovementBehavior.h"

class SineZigzagMovement : public IMovementBehavior {
private:
    float startX;
    float time = 0.0f;
    float amplitude = 150.0f;
    float frequency = 1.5f;
    bool initialized = false;
    bool wrapAround;
public:
    SineZigzagMovement(bool wrap = true) : wrapAround(wrap) {}
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};

#endif
