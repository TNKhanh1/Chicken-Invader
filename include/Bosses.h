#ifndef BOSSES_H
#define BOSSES_H

#include "Enemy.h"
#include "Bullet.h"
#include "GameManager.h"
#include <vector>

// Forward declarations
class Spaceship;

// Custom Boss Bullets
class GrenadeBullet : public Bullet {
public:
    GrenadeBullet(Vector2 startPos, Vector2 velocity);
    void Draw() override;
};

class KnifeBullet : public Bullet {
public:
    KnifeBullet(Vector2 startPos, Vector2 velocity);
    void Draw() override;
};

class EggBullet : public Bullet {
public:
    EggBullet(Vector2 startPos, Vector2 velocity);
    void Draw() override;
};

// Boss Base Class
class Boss : public Enemy {
protected:
    float drumstickDropTimer;
    float wobbleAngle;
    float wobbleTimer;
    float battleTime;
    Vector2 targetPos;
    
public:
    Boss(int visualId, const EnemyStats& stats, Vector2 pos);
    
    virtual void Update(float deltaTime) override;
    virtual void Draw() override;
    virtual Rectangle GetHitbox() const override;
};

// Military Chicken Boss
class MilitaryChickenBoss : public Boss {
private:
    float grenadeTimer;
    float knifeTimer;
    int knifePhase;      // 0: wait, 1: first pair thrown, waiting for second, 2: second pair thrown
    float knifeDelay;    // Delay timer between first and second pair

    void DropGrenades();
    void ThrowKnives(Spaceship* player);

public:
    MilitaryChickenBoss(int visualId, const EnemyStats& stats, Vector2 pos);
    void Update(float deltaTime) override;
};

// Super Chick Boss
class SuperChickBoss : public Boss {
private:
    float eggBurstTimer;

    void FireEggBurst();

public:
    SuperChickBoss(int visualId, const EnemyStats& stats, Vector2 pos);
    void Update(float deltaTime) override;
};

#endif // BOSSES_H
