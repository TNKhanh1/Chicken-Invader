#ifndef BEAM_WEAPON_H
#define BEAM_WEAPON_H

#include "WeaponStrategy.h"
#include "Spaceship.h"
#include "Enemy.h"
#include <vector>
#include <memory>
#include <cmath>

// Lớp cơ sở cho các vũ khí đạn tia (Beam/Laser) gắn liền với nòng tàu
class BeamShootingBehavior : public IShootingBehavior {
protected:
    float baseWidth;
    float lockRadius;
    bool isAutoLocking;

public:
    BeamShootingBehavior(float width, float radius, bool autoLock);
    virtual ~BeamShootingBehavior() = default;

    // Thuật toán Tự Động Nhắm (Auto-locking) tìm quái vật gần nhất trong bán kính
    bool FindNearestTarget(Vector2 podPos, const std::vector<std::shared_ptr<Enemy>>& enemies, Vector2& outTargetPos, float& outAngle);

    void Shoot(Spaceship* ship) override;
};

// Lớp chiến thuật bắn riêng biệt cho Lightning Fryer (Luồng Sét Tự Động Nhắm)
class LightningFryerBehavior : public BeamShootingBehavior {
public:
    LightningFryerBehavior();
    void Shoot(Spaceship* ship) override;
};

#endif // BEAM_WEAPON_H
