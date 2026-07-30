#ifndef SINEZIGZAGMOVEMENT_H
#define SINEZIGZAGMOVEMENT_H

#include "IMovementBehavior.h"

class SineZigzagMovement : public IMovementBehavior {
private:
    float startX;
    float time = 0.0f;
    float amplitude = 150.0f;
    float frequency = 3.0f;
    bool initialized = false;
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};

#endif
