#ifndef WEAPON_STRATEGY_H
#define WEAPON_STRATEGY_H

#include "raylib.h"

// Interface chung cho các loại súng (Strategy Pattern)
class IShootingBehavior {
public:
    virtual ~IShootingBehavior() = default;
    
    // Hàm bắn, nhận vào vị trí và sát thương
    virtual void Shoot(Vector2 position, float damage = 10.0f) = 0;
};

#include "Bullet.h"
#include "GameManager.h"
#include <memory>

class SingleShot : public IShootingBehavior {
public:
    void Shoot(Vector2 position, float damage = 10.0f) override {
        // Tạo 1 viên đạn bay thẳng lên trên (isPlayerBullet = true)
        // Tốc độ đạn tăng 20% (400 -> 480)
        auto bullet = std::make_shared<Bullet>(position, damage, 480.0f, true);
        GameManager::GetInstance()->AddBullet(bullet);
        GameManager::GetInstance()->PlayShootSound();
    }
};

#endif // WEAPON_STRATEGY_H
