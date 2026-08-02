#ifndef ALL_WEAPON_BEHAVIORS_H
#define ALL_WEAPON_BEHAVIORS_H

#include "WeaponStrategy.h"
#include "Spaceship.h"
#include <memory>
#include <string>

class NeutronGunBehavior : public IShootingBehavior {
public:
    void Shoot(Spaceship* ship) override;
};

class RiddlerBehavior : public IShootingBehavior {
public:
    void Shoot(Spaceship* ship) override;
};

class IonBlasterBehavior : public IShootingBehavior {
public:
    void Shoot(Spaceship* ship) override;
};

class UtensilPokerBehavior : public IShootingBehavior {
public:
    void Shoot(Spaceship* ship) override;
};

class PlasmaRifleBehavior : public IShootingBehavior {
public:
    void Shoot(Spaceship* ship) override;
};

class AbsolverBeamBehavior : public IShootingBehavior {
public:
    void Shoot(Spaceship* ship) override;
};

// Hàm Factory giúp tạo chiến thuật bắn tương ứng từ tên vũ khí
std::unique_ptr<IShootingBehavior> CreateWeaponBehavior(const std::string& weaponName);

#endif // ALL_WEAPON_BEHAVIORS_H
