#ifndef MOVEMENT_STRATEGY_H
#define MOVEMENT_STRATEGY_H

#include "raylib.h"

// Interface cho Strategy Pattern (Cơ chế di chuyển của gà)
class IMovementBehavior {
public:
    virtual ~IMovementBehavior() = default;
    
    // Hàm di chuyển, thay đổi position trực tiếp
    virtual void Move(Vector2& position, float moveSpeed, float deltaTime) = 0;
};

// Các class cụ thể (VD: WaveMovement, StraightMovement, ZigzagMovement) sẽ triển khai sau
class StraightMovement : public IMovementBehavior {
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime) override {
        // Gà bay thẳng xuống dưới
        position.y += moveSpeed * deltaTime;
    }
};

#endif // MOVEMENT_STRATEGY_H
