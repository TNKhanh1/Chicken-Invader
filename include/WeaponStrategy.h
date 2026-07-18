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

#include "Bullet.h"
#include "GameManager.h"
#include <memory>

// Các class cụ thể triển khai IShootingBehavior
class SingleShot : public IShootingBehavior {
public:
    void Shoot(Vector2 position) override {
        // Tạo 1 viên đạn bay thẳng lên trên (isPlayerBullet = true)
        // Dame = 10, Speed = 400
        auto bullet = std::make_shared<Bullet>(position, 10.0f, 400.0f, true);
        GameManager::GetInstance()->AddBullet(bullet);
    }
};

#endif // WEAPON_STRATEGY_H
