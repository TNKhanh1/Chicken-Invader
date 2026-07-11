#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "Character.h"
#include "Observer.h"
#include "WeaponStrategy.h"
#include <vector>
#include <memory>
#include <algorithm>

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

    // Strategy Pattern
    std::unique_ptr<IShootingBehavior> shootingBehavior;

    // Observer Pattern
    std::vector<IObserver*> observers;

public:
    Spaceship(Vector2 pos, float hp, float dmg, float arm, float spd, 
              float critC, float critD, float mana, float atkSpd)
        : Character(pos, hp, dmg, arm, spd), 
          critChance(critC), critDamage(critD), maxMana(mana), currentMana(0), attackSpeed(atkSpd),
          level(1), currentExp(0), maxExp(100) {}

    // Implement ISubject
    void AddObserver(IObserver* observer) override {
        observers.push_back(observer);
    }

    void RemoveObserver(IObserver* observer) override {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }

    void Notify(EventType event, const std::string& data) override {
        for (auto observer : observers) {
            observer->OnNotify(event, data);
        }
    }

    // Các hàm chính
    void Init() override {
        // Tải texture, set active...
    }

    void Update(float deltaTime) override {
        // Xử lý Input di chuyển (WASD / Mouse)
        // Cập nhật vị trí
    }

    void Draw() override {
        // Vẽ phi thuyền
    }

    void Die() override {
        // Xử lý GameOver
    }

    // Hành vi thay đổi súng (Strategy Pattern)
    void SetShootingBehavior(std::unique_ptr<IShootingBehavior> behavior) {
        shootingBehavior = std::move(behavior);
    }

    void Fire() {
        if (shootingBehavior) {
            shootingBehavior->Shoot(position);
            
            // Mỗi lần bắn tích 1 lượng mana (Theo doc)
            GainMana(10.0f);
        }
    }

    // Cơ chế Exp và Mana
    void GainExp(float amount) {
        currentExp += amount;
        Notify(EventType::PLAYER_EXP_GAINED, std::to_string(currentExp));
        if (currentExp >= maxExp) {
            LevelUp();
        }
    }

    void GainMana(float amount) {
        if (currentMana < maxMana) {
            currentMana += amount;
            if (currentMana > maxMana) currentMana = maxMana;
            Notify(EventType::PLAYER_MANA_CHANGED, std::to_string(currentMana));
        }
    }

    void LevelUp() {
        level++;
        currentExp -= maxExp;
        maxExp *= 1.2f; // Tăng yêu cầu exp cho level tiếp theo
        
        // Tăng chỉ số khi lên cấp
        maxHp += 10.0f;
        currentHp = maxHp; // Hồi đầy máu
        damage += 2.0f;
        
        Notify(EventType::PLAYER_LEVEL_UP, std::to_string(level));
    }
};

#endif // SPACESHIP_H
