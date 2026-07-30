#ifndef HORIZONTALSWEEPMOVEMENT_H
#define HORIZONTALSWEEPMOVEMENT_H

#include "IMovementBehavior.h"

class HorizontalSweepMovement : public IMovementBehavior {
private:
    float direction;
public:
    HorizontalSweepMovement(float startDir);
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};

#endif
