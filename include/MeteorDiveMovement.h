#ifndef METEORDIVEMOVEMENT_H
#define METEORDIVEMOVEMENT_H

#include "IMovementBehavior.h"

class MeteorDiveMovement : public IMovementBehavior {
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};

#endif
