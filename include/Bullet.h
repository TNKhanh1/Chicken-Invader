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

    int bulletType; // 0: Normal, 1: Strong, 2: Weak

public:
    Bullet(Vector2 startPos, float dmg, float spd, bool playerBullet = true, int type = 0);
    
    void SetVelocity(Vector2 vel);
    
    void Init() override;

    void Update(float deltaTime) override;
    
    void Draw() override;
    
    float GetDamage() const;
    bool IsPlayerBullet() const;
    
    int GetBulletType() const { return bulletType; }
    
    Rectangle GetHitbox() const;
};

#endif // BULLET_H
