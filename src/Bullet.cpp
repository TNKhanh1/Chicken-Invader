#include "Bullet.h"
#include <cmath>

Bullet::Bullet(Vector2 startPos, float dmg, float spd, bool playerBullet, int type, float r) 
    : GameObject(startPos), speed(spd), damage(dmg), isPlayerBullet(playerBullet), velocity({0, 0}), hasCustomVelocity(false), prevPosition(startPos), bulletType(type), radius(r), angle(0.0f), trajectory(nullptr) {}

void Bullet::SetVelocity(Vector2 vel) {
    velocity = vel;
    hasCustomVelocity = true;
}

void Bullet::SetTrajectory(std::shared_ptr<IBulletTrajectory> traj) {
    trajectory = traj;
}

void Bullet::Reset(Vector2 startPos, float dmg, float spd, bool playerBullet, int type, float r) {
    position = startPos;
    prevPosition = startPos;
    damage = dmg;
    speed = spd;
    isPlayerBullet = playerBullet;
    bulletType = type;
    radius = r;
    hasCustomVelocity = false;
    velocity = {0, 0};
    trajectory = nullptr;
    angle = 0.0f;
    isActive = true;
}

void Bullet::Init() {
}

void Bullet::Update(float deltaTime) {
    if (!isActive) return;

    prevPosition = position;

    if (trajectory) {
        trajectory->UpdatePosition(position, angle, speed, deltaTime);
    } else if (hasCustomVelocity) {
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
    } else {
        if (isPlayerBullet) {
            position.y -= speed * deltaTime;
        } else {
            position.y += speed * deltaTime;
        }
    }

    if (position.y < -100 || position.y > 1100 || position.x < -100 || position.x > 2000) {
        isActive = false;
    }
}

float Bullet::GetDamage() const { return damage; }
bool Bullet::IsPlayerBullet() const { return isPlayerBullet; }

float Bullet::GetRadius() const { return radius; }
Vector2 Bullet::GetCenter() const { return position; }
Vector2 Bullet::GetPrevCenter() const { return prevPosition; }
Vector2 Bullet::GetVelocity() const {
    if (hasCustomVelocity) return velocity;
    float rad = (angle - 90.0f) * (3.14159265f / 180.0f);
    return { (float)cos(rad) * speed, (float)sin(rad) * speed };
}

Rectangle Bullet::GetHitbox() const {
    return {position.x - radius, position.y - radius, radius * 2.0f, radius * 2.0f};
}

