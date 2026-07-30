#ifndef WEAPON_STRATEGY_H
#define WEAPON_STRATEGY_H

#include "raylib.h"

class Spaceship; // Forward declaration

class IShootingBehavior {
public:
    virtual ~IShootingBehavior() = default;
    
    // Hàm bắn, nhận vào phi thuyền để lấy level và position
    virtual void Shoot(Spaceship* ship) = 0;
};

#endif // WEAPON_STRATEGY_H
