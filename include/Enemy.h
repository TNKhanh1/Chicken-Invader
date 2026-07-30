#ifndef ENEMY_H
#define ENEMY_H

#include "Character.h"
#include "IMovementBehavior.h"
#include <memory>

class Enemy : public Character {
private:
    std::unique_ptr<IMovementBehavior> movementBehavior;
    int pointValue; // Điểm số khi bị giết

public:
    Enemy(Vector2 pos, float hp, float dmg, float arm, float spd, int points);

    int GetPointValue() const;

    void Init() override;

    int enemyType = 0;
    int currentFrame = 0;
    float frameTimer = 0.0f;
    int asteroidVariant = 1; // Loại thiên thạch (1 hoặc 2)

    float eggDropTimer = 0.0f;
    bool canShoot = true;

    void ResetEggTimer();

    void Update(float deltaTime) override;

    void Draw() override;

    void Die() override;

    void SetMovementBehavior(std::unique_ptr<IMovementBehavior> behavior);

    void DropItem();

    Rectangle GetHitbox() const;
};

#endif // ENEMY_H
