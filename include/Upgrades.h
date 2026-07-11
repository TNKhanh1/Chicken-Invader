#ifndef UPGRADES_H
#define UPGRADES_H

#include "Spaceship.h"
#include <memory>

// Áp dụng Decorator Pattern để bọc thêm sức mạnh cho phi thuyền

class SpaceshipDecorator : public Spaceship {
protected:
    std::shared_ptr<Spaceship> coreShip;

public:
    SpaceshipDecorator(std::shared_ptr<Spaceship> ship) 
        : Spaceship(ship->GetPosition(), ship->GetMaxHp(), ship->GetDamage(), ship->GetArmor(), ship->GetMoveSpeed(), 0, 0, 0, 0), // Base values
          coreShip(ship) {}

    virtual ~SpaceshipDecorator() = default;

    // Forward toàn bộ các hàm cốt lõi sang coreShip để tránh mất dữ liệu thực tế
    virtual float GetHp() const override { return coreShip->GetHp(); }
    virtual float GetMaxHp() const override { return coreShip->GetMaxHp(); }
    virtual float GetDamage() const override { return coreShip->GetDamage(); }
    virtual float GetArmor() const override { return coreShip->GetArmor(); }
    virtual float GetMoveSpeed() const override { return coreShip->GetMoveSpeed(); }

    virtual void TakeDamage(float incomingDamage) override {
        coreShip->TakeDamage(incomingDamage);
    }

    // Các hàm vòng đời
    void Init() override { coreShip->Init(); }
    void Update(float deltaTime) override { coreShip->Update(deltaTime); }
    void Draw() override { coreShip->Draw(); }
    void Die() override { coreShip->Die(); }
};

// --- Ví dụ Cụ thể ---
// 1. Ngọc tăng sát thương (Trang bị ngoài trận)
class DamageStoneDecorator : public SpaceshipDecorator {
private:
    float bonusDamage;
public:
    DamageStoneDecorator(std::shared_ptr<Spaceship> ship, float bonusDmg) 
        : SpaceshipDecorator(ship), bonusDamage(bonusDmg) {}
        
    float GetDamage() const {
        return SpaceshipDecorator::GetDamage() + bonusDamage;
    }
};

#endif // UPGRADES_H
