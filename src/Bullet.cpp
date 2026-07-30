#include "Bullet.h"

Bullet::Bullet(Vector2 startPos, float dmg, float spd, bool playerBullet, int type) 
    : GameObject(startPos), speed(spd), damage(dmg), isPlayerBullet(playerBullet), velocity({0, 0}), hasCustomVelocity(false), bulletType(type) {}

void Bullet::SetVelocity(Vector2 vel) {
    velocity = vel;
    hasCustomVelocity = true;
}

void Bullet::Init() {
}

void Bullet::Update(float deltaTime) {
    if (!isActive) return;

    if (hasCustomVelocity) {
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
    } else {
        if (isPlayerBullet) {
            position.y -= speed * deltaTime;
        } else {
            position.y += speed * deltaTime;
        }
    }

    if (position.y < -50 || position.y > 800) {
        isActive = false;
    }
}

float Bullet::GetDamage() const { return damage; }
bool Bullet::IsPlayerBullet() const { return isPlayerBullet; }

Rectangle Bullet::GetHitbox() const {
    return {position.x - 5, position.y - 5, 10, 10};
}
