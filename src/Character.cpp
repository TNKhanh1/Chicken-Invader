#include "Character.h"

Character::Character(Vector2 pos, float hp, float dmg, float arm, float spd) 
    : GameObject(pos), maxHp(hp), currentHp(hp), damage(dmg), armor(arm), moveSpeed(spd) {}

void Character::TakeDamage(float incomingDamage) {
    float damageMultiplier = 100.0f / (100.0f + armor);
    float actualDamage = incomingDamage * damageMultiplier;
    
    currentHp -= actualDamage;
    if (currentHp <= 0) {
        currentHp = 0;
        Die();
    }
}

float Character::GetHp() const { return currentHp; }
float Character::GetMaxHp() const { return maxHp; }
float Character::GetDamage() const { return damage; }
float Character::GetArmor() const { return armor; }
float Character::GetMoveSpeed() const { return moveSpeed; }
