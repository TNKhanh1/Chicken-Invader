#ifndef BULLET_H
#define BULLET_H

#include "GameObject.h"

class Bullet : public GameObject {
private:
    float speed;
    float damage;
    bool isPlayerBullet;
    Vector2 velocity;
    bool hasCustomVelocity;

public:
    Bullet(Vector2 startPos, float dmg, float spd, bool playerBullet = true) 
        : GameObject(startPos), speed(spd), damage(dmg), isPlayerBullet(playerBullet), velocity({0, 0}), hasCustomVelocity(false) {}
    
    void SetVelocity(Vector2 vel) {
        velocity = vel;
        hasCustomVelocity = true;
    }
    
    void Init() override {
        // Init properties if needed
    }

    void Update(float deltaTime) override {
        if (!isActive) return;

        // Move bullet
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

        // Deactivate if off-screen
        if (position.y < -50 || position.y > 800) {
            isActive = false;
        }
    }
    
    void Draw() override;
    
    float GetDamage() const { return damage; }
    bool IsPlayerBullet() const { return isPlayerBullet; }
    
    Rectangle GetHitbox() const {
        return {position.x - 5, position.y - 5, 10, 10};
    }
};

#endif // BULLET_H
