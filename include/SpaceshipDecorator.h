#ifndef SPACESHIP_DECORATOR_H
#define SPACESHIP_DECORATOR_H

#include "Spaceship.h"
#include <memory>

class SpaceshipDecorator : public Spaceship {
protected:
    std::shared_ptr<Spaceship> coreShip;

public:
    SpaceshipDecorator(std::shared_ptr<Spaceship> ship);

    virtual ~SpaceshipDecorator() = default;

    virtual float GetHp() const override;
    virtual float GetMaxHp() const override;
    virtual float GetDamage() const override;
    virtual float GetArmor() const override;
    virtual float GetMoveSpeed() const override;
    virtual float GetCritChance() const override;
    virtual float GetCritDamage() const override;
    virtual float GetMaxMana() const override;
    virtual float GetCurrentMana() const override;
    virtual float GetAttackSpeed() const override;
    virtual int GetLevel() const override;
    virtual void SetLevel(int newLevel) override;
    virtual float GetCurrentExp() const override;
    virtual float GetMaxExp() const override;
    std::string GetName() const override;
    virtual Rectangle GetHitbox() const override;

    virtual void TakeDamage(float incomingDamage) override;

    virtual void AddObserver(IObserver* observer) override;
    virtual void RemoveObserver(IObserver* observer) override;
    virtual void Notify(EventType event, const std::string& data) override;

    virtual void SetShootingBehavior(std::unique_ptr<IShootingBehavior> behavior) override;
    virtual void SetWeapon(const std::string& weaponName) override;
    virtual std::string GetWeapon() const override;
    virtual void ReloadStatsFromCSV() override;
    virtual void ResetToBaseStats() override;
    virtual void TriggerRecoil() override;
    virtual float GetRecoilOffset() const override;
    virtual float GetThrusterIntensity() const override;
    virtual float GetThrusterTiltX() const override;
    virtual float GetThrusterLengthMult() const override;
    virtual void Fire() override;
    virtual void GainExp(float amount) override;
    virtual void GainMana(float amount) override;
    virtual void LevelUp() override;
    virtual void Heal(float amount) override;

    virtual void AddArgument(int argId) override;
    virtual bool HasArgument(int argId) const override;
    virtual const std::vector<int>& GetActiveArguments() const override;
    
    virtual void AddPermanentDamage(float amt) override;
    virtual float GetPermanentDamageBonus() const override;
    virtual void AddPermanentMaxHp(float amt) override;
    virtual void AddPermanentArmor(float amt) override;
    virtual void AddPermanentFireRate(float pct) override;
    virtual void AddPermanentCritChance(float amt) override;
    virtual void AddPermanentCritDamage(float amt) override;

    void Init() override;
    void Update(float deltaTime) override;
    void Draw() override;
    void Die() override;
};

#endif // SPACESHIP_DECORATOR_H
