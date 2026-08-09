#ifndef STRAIGHTMOVEMENT_H
#define STRAIGHTMOVEMENT_H

#include "IMovementBehavior.h"

class StraightMovement : public IMovementBehavior {
private:
    bool wrapAround;
public:
    StraightMovement(bool wrap = false) : wrapAround(wrap) {}
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};

#endif
