#include "Enemy.h"
#include "raylib.h"

Enemy::Enemy(Vector2 pos, float hp, float dmg, float arm, float spd, int points)
    : Character(pos, hp, dmg, arm, spd), pointValue(points) {}

int Enemy::GetPointValue() const { return pointValue; }

void Enemy::Init() {
}

void Enemy::ResetEggTimer() {
    eggDropTimer = (GetRandomValue(20, 60)) / 10.0f;
}

void Enemy::Die() {
    isActive = false;
    DropItem();
}

void Enemy::SetMovementBehavior(std::unique_ptr<IMovementBehavior> behavior) {
    movementBehavior = std::move(behavior);
}

void Enemy::DropItem() {
}

Rectangle Enemy::GetHitbox() const {
    return {position.x - 25, position.y - 25, 50, 50};
}
