#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "Character.h"
#include "Observer.h"
#include "WeaponStrategy.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <string>

// Lớp Phi Thuyền chính
class Spaceship : public Character, public ISubject {
private:
    // Các chỉ số riêng của phi thuyền
    float critChance;
    float critDamage;
    float maxMana;
    float currentMana;
    float attackSpeed;
    
    int level;
    float currentExp;
    float maxExp;
    std::string name;
    std::string currentWeapon = "Hypergun";

    // Strategy Pattern
    std::unique_ptr<IShootingBehavior> shootingBehavior;

    float fireTimer = 0.0f; // Bộ đếm hồi chiêu

    // Observer Pattern
    std::vector<IObserver*> observers;

public:
    Spaceship(std::string name, Vector2 pos, float hp, float dmg, float arm, float spd, 
              float critC, float critD, float mana, float atkSpd);

    virtual ~Spaceship() = default;

    // ... [Bỏ qua các hàm observer, Getters không đổi] ...
    virtual void AddObserver(IObserver* observer) override;
    virtual void RemoveObserver(IObserver* observer) override;
    virtual void Notify(EventType event, const std::string& data) override;

    virtual float GetCritChance() const;
    virtual float GetCritDamage() const;
    virtual float GetMaxMana() const;
    virtual float GetCurrentMana() const;
    virtual float GetAttackSpeed() const;
    virtual int GetLevel() const;
    virtual void SetLevel(int newLevel);
    virtual float GetCurrentExp() const;
    virtual float GetMaxExp() const;
    virtual std::string GetName() const;
    
    virtual Rectangle GetHitbox() const;

    void Init() override;

    void Update(float deltaTime) override;

    // --- ARGUMENT SYSTEM ---
    std::vector<int> activeArguments;
    virtual void AddArgument(int argId);
    virtual bool HasArgument(int argId) const;

    float permanentDamageBonus = 0.0f;
    virtual void AddPermanentDamage(float amt) { permanentDamageBonus += amt; }
    virtual float GetPermanentDamageBonus() const { return permanentDamageBonus; }

    void Draw() override;

    void Die() override;

    virtual void SetShootingBehavior(std::unique_ptr<IShootingBehavior> behavior);
    virtual void SetWeapon(const std::string& weaponName);
    virtual std::string GetWeapon() const { return currentWeapon; }

    virtual bool CanFire() const;

    virtual void Fire();

    // Cơ chế Exp và Mana
    virtual void GainExp(float amount);

    virtual void GainMana(float amount);

    virtual void LevelUp();

    virtual void Heal(float amount);
};

#endif // SPACESHIP_H
