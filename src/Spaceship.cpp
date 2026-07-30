#include "Spaceship.h"

#include "SpaceshipDataManager.h"

Spaceship::Spaceship(std::string n, Vector2 pos, float hp, float dmg, float arm, float spd, 
                     float critC, float critD, float mana, float atkSpd)
    : Character(pos, hp, dmg, arm, spd), 
      critChance(critC), critDamage(critD), maxMana(mana), currentMana(0), attackSpeed(atkSpd),
      level(1), currentExp(0), maxExp(100), name(n) {}

void Spaceship::AddObserver(IObserver* observer) {
    observers.push_back(observer);
}

void Spaceship::RemoveObserver(IObserver* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Spaceship::Notify(EventType event, const std::string& data) {
    for (auto observer : observers) {
        observer->OnNotify(event, data);
    }
}

float Spaceship::GetCritChance() const { return critChance; }
float Spaceship::GetCritDamage() const { return critDamage; }
float Spaceship::GetMaxMana() const { return maxMana; }
float Spaceship::GetCurrentMana() const { return currentMana; }
float Spaceship::GetAttackSpeed() const { return attackSpeed; }
int Spaceship::GetLevel() const { return level; }
float Spaceship::GetCurrentExp() const { return currentExp; }
float Spaceship::GetMaxExp() const { return maxExp; }
std::string Spaceship::GetName() const { return name; }

Rectangle Spaceship::GetHitbox() const {
    return {position.x - 20, position.y - 20, 40, 40};
}

void Spaceship::Init() {}

void Spaceship::Update(float deltaTime) {
    if (fireTimer > 0.0f) {
        fireTimer -= deltaTime;
    }
}

void Spaceship::Die() {}

void Spaceship::SetShootingBehavior(std::unique_ptr<IShootingBehavior> behavior) {
    shootingBehavior = std::move(behavior);
}

bool Spaceship::CanFire() const {
    return fireTimer <= 0.0f;
}

void Spaceship::Fire() {
    if (shootingBehavior) {
        // Change from shoot(position, damage) to shoot(this) as requested in task
        // We will update IShootingBehavior shortly. For now, since IShootingBehavior still has (Vector2, float), 
        // I will wait to change it or change it now.
        // The user asked to change IShootingBehavior interface.
        // shootingBehavior->Shoot(this); 
        // I'll update it along with WeaponStrategy.h
        shootingBehavior->Shoot(this);
        
        if (attackSpeed > 0) {
            fireTimer = 1.0f / attackSpeed;
        }
        GainMana(10.0f);
    }
}

void Spaceship::GainExp(float amount) {
    currentExp += amount;
    Notify(EventType::PLAYER_EXP_GAINED, std::to_string(currentExp));
    if (currentExp >= maxExp) {
        LevelUp();
    }
}

void Spaceship::GainMana(float amount) {
    if (currentMana < maxMana) {
        currentMana += amount;
        if (currentMana > maxMana) currentMana = maxMana;
        Notify(EventType::PLAYER_MANA_CHANGED, std::to_string(currentMana));
    }
}

void Spaceship::LevelUp() {
    level++;
    currentExp -= maxExp;
    maxExp *= 1.2f; 
    
    // Đọc chỉ số mới từ CSV
    SpaceshipStats stats = SpaceshipDataManager::GetInstance()->GetStats(name, level);
    maxHp = stats.hp;
    currentHp = maxHp; 
    damage = stats.damage;
    armor = stats.armor;
    moveSpeed = stats.moveSpeed;
    critChance = stats.critChance;
    critDamage = stats.critDamage;
    maxMana = stats.maxMana;
    attackSpeed = stats.attackSpeed;
    
    Notify(EventType::PLAYER_LEVEL_UP, std::to_string(level));
}
