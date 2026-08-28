#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "Character.h"
#include "Observer.h"
#include "WeaponStrategy.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <string>
enum class BuffType { SWORD, SHIELD };

struct ActiveBuff {
    bool swordActive = false;
    float swordTimer = 0.0f;
    bool shieldActive = false;
    float shieldTimer = 0.0f;
};

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

    // --- BUFF SYSTEM ---
    ActiveBuff activeBuff;

    // Strategy Pattern
    std::unique_ptr<IShootingBehavior> shootingBehavior;

    float fireTimer = 0.0f; // Bộ đếm hồi chiêu

    // Thuộc tính hiệu ứng giật nhẹ (Recoil & Kickback Animation - OOP Encapsulation)
    float recoilOffset = 0.0f;
    float recoilTimer = 0.0f;
    float recoilDuration = 0.15f;
    float maxRecoilDistance = 6.0f;

    // Thuộc tính đuôi năng lượng plasma phía sau tàu (Thruster Flame & Trail)
    Vector2 prevPosition = {0.0f, 0.0f};
    float thrusterIntensity = 0.3f;
    float thrusterTiltX = 0.0f;
    float thrusterLengthMult = 1.0f;

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
    virtual const std::vector<int>& GetActiveArguments() const { return activeArguments; }
    virtual void AddArgument(int argId);
    virtual bool HasArgument(int argId) const;

    float permanentDamageBonus = 0.0f;
    virtual void AddPermanentDamage(float amt) { permanentDamageBonus += amt; }
    virtual float GetPermanentDamageBonus() const { return permanentDamageBonus; }
    
    virtual void AddPermanentMaxHp(float amt) { maxHp += amt; currentHp += amt; }
    virtual void AddPermanentArmor(float amt) { armor += amt; }
    virtual void AddPermanentFireRate(float pct) { attackSpeed *= (1.0f + pct); }
    virtual void AddPermanentCritChance(float amt) { critChance += amt; }
    virtual void AddPermanentCritDamage(float amt) { critDamage += amt; }
    void Draw() override;

    void Die() override;

    virtual void SetShootingBehavior(std::unique_ptr<IShootingBehavior> behavior);
    virtual void SetWeapon(const std::string& weaponName);
    virtual std::string GetWeapon() const { return currentWeapon; }
    virtual void ReloadStatsFromCSV();

    // Hiệu ứng giật súng
    virtual void TriggerRecoil();
    virtual float GetRecoilOffset() const;

    // Hiệu ứng đuôi năng lượng & động cơ
    virtual float GetThrusterIntensity() const;
    virtual float GetThrusterTiltX() const;
    virtual float GetThrusterLengthMult() const;

    virtual bool CanFire() const;

    virtual void Fire();

    // Cơ chế Exp và Mana
    virtual void GainExp(float amount);

    virtual void GainMana(float amount);
    
    // Trạng thái kích hoạt Mana (Ultimate/Overdrive)
    bool isManaActive = false;
    virtual bool IsManaActive() const { return isManaActive; }
    virtual void ActivateMana();
    virtual void DeactivateMana();

    virtual void LevelUp();

    virtual void Heal(float amount);

    // --- BUFF METHODS ---
    void ApplyBuff(BuffType type, float duration);
    void UpdateBuffs(float deltaTime);
    void ClearAllBuffs();
    bool HasSwordBuff() const { return activeBuff.swordActive; }
    bool HasShieldBuff() const { return activeBuff.shieldActive; }
    float GetSwordTimer() const { return activeBuff.swordTimer; }
    float GetShieldTimer() const { return activeBuff.shieldTimer; }

    virtual float GetDamage() const override;
    virtual void TakeDamage(float incomingDamage) override;
};

#endif // SPACESHIP_H
