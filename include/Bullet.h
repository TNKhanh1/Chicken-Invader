#ifndef BULLET_H
#define BULLET_H

#include "GameObject.h"
#include "BulletTrajectory.h"
#include <memory>

class Bullet : public GameObject {
private:
    float speed;
    float damage;
    bool isPlayerBullet;
    Vector2 velocity;
    bool hasCustomVelocity;
    Vector2 prevPosition;

    int bulletType; // 0: Normal, 1: Strong, 2: Weak
    float radius;   // Bán kính Circle Hitbox
    float angle;    // Góc quay hiện tại của đạn
    
    std::shared_ptr<IBulletTrajectory> trajectory; // Strategy Pattern cho quỹ đạo

public:
    Bullet(Vector2 startPos, float dmg, float spd, bool playerBullet = true, int type = 0, float r = 12.0f);
    
    void SetVelocity(Vector2 vel);
    void SetTrajectory(std::shared_ptr<IBulletTrajectory> traj);
    void Reset(Vector2 startPos, float dmg, float spd, bool playerBullet, int type, float r = 12.0f);

    void Init() override;
    void Update(float deltaTime) override;
    void Draw() override;
    
    float GetDamage() const;
    bool IsPlayerBullet() const;
    int GetBulletType() const { return bulletType; }
    
    // Circle Hitbox & CCD Getters
    float GetRadius() const;
    Vector2 GetCenter() const;
    Vector2 GetPrevCenter() const;
    Vector2 GetVelocity() const;
    
    // Legacy Rectangle Hitbox
    Rectangle GetHitbox() const;
};

#endif // BULLET_H
