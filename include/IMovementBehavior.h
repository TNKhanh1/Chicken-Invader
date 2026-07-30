#ifndef IMOVEMENTBEHAVIOR_H
#define IMOVEMENTBEHAVIOR_H

#include "raylib.h"

class IMovementBehavior {
public:
    virtual ~IMovementBehavior() = default;
    virtual void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) = 0;
};

#endif
