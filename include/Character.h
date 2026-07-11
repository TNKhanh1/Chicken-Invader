#ifndef CHARACTER_H
#define CHARACTER_H

#include "GameObject.h"
#include <cmath>

// Lớp cơ sở cho các thực thể sống (Spaceship, Enemy)
class Character : public GameObject {
protected:
    // Chỉ số nền tảng (Base Stats)
    float maxHp;
    float currentHp;
    float damage;
    float armor;
    float moveSpeed;

public:
    Character(Vector2 pos, float hp, float dmg, float arm, float spd) 
        : GameObject(pos), maxHp(hp), currentHp(hp), damage(dmg), armor(arm), moveSpeed(spd) {}

    virtual ~Character() = default;

    // Hàm nhận sát thương áp dụng công thức giảm trừ theo Giáp (Armor)
    virtual void TakeDamage(float incomingDamage) {
        // Công thức f(X, Y): Sát thương thực tế = incomingDamage * (100 / (100 + armor))
        // Đây là công thức phổ biến trong các game (như LMHT/Dota)
        float damageMultiplier = 100.0f / (100.0f + armor);
        float actualDamage = incomingDamage * damageMultiplier;
        
        currentHp -= actualDamage;
        if (currentHp <= 0) {
            currentHp = 0;
            Die();
        }
    }

    // Hàm xử lý khi máu <= 0
    virtual void Die() = 0;

    // Getters (Sử dụng virtual để Decorator Pattern có thể Override)
    virtual float GetHp() const { return currentHp; }
    virtual float GetMaxHp() const { return maxHp; }
    virtual float GetDamage() const { return damage; }
    virtual float GetArmor() const { return armor; }
    virtual float GetMoveSpeed() const { return moveSpeed; }
};

#endif // CHARACTER_H
