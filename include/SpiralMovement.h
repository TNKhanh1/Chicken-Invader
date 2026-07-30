#ifndef SPIRALMOVEMENT_H
#define SPIRALMOVEMENT_H

#include "IMovementBehavior.h"

class SpiralMovement : public IMovementBehavior {
private:
    float time = 0.0f;
    float radius = 0.0f;
    float maxRadius = 250.0f;
    float rotationSpeed = 3.0f;
    Vector2 center;
    bool initialized = false;
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};

#endif
