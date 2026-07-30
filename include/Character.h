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
    Character(Vector2 pos, float hp, float dmg, float arm, float spd);

    virtual ~Character() = default;

    // Hàm nhận sát thương áp dụng công thức giảm trừ theo Giáp (Armor)
    virtual void TakeDamage(float incomingDamage);

    // Hàm xử lý khi máu <= 0
    virtual void Die() = 0;

    // Getters (Sử dụng virtual để Decorator Pattern có thể Override)
    virtual float GetHp() const;
    virtual float GetMaxHp() const;
    virtual float GetDamage() const;
    virtual float GetArmor() const;
    virtual float GetMoveSpeed() const;
};

#endif // CHARACTER_H
