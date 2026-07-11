#ifndef WEAPON_STRATEGY_H
#define WEAPON_STRATEGY_H

#include "raylib.h"

// Interface chung cho các loại súng (Strategy Pattern)
class IShootingBehavior {
public:
    virtual ~IShootingBehavior() = default;
    
    // Hàm bắn, nhận vào vị trí hiện tại của phi thuyền
    virtual void Shoot(Vector2 position) = 0;
};

// Các class cụ thể triển khai IShootingBehavior sẽ được thêm vào sau (VD: SingleShot, SpreadShot, LaserShot)

#endif // WEAPON_STRATEGY_H
