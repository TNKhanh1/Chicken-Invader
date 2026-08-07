#ifndef ENEMY_H
#define ENEMY_H

#include "Character.h"
#include "IMovementBehavior.h"
#include <memory>


#include <vector>

enum class EnemyRole {
    NORMAL,
    SWARM,
    TANK,
    BOSS,
    ASTEROID
};

struct EnemyStats {
    float hp = 100.0f;
    float damage = 20.0f;
    float armor = 0.0f;
    float speed = 100.0f;
    float eggRate = 3.0f;
    int score = 10;
};


class Enemy : public Character {
private:
    std::unique_ptr<IMovementBehavior> movementBehavior;
    int pointValue; // Điểm số khi bị giết
    float baseSizeForType() const;

public:
    Enemy(int visualId, EnemyRole role, const EnemyStats& stats, Vector2 pos);

    int GetPointValue() const;

    void Init() override;

    int visualId = 1;
    EnemyRole role = EnemyRole::NORMAL;
    EnemyStats stats;
    int currentFrame = 0;
    float frameTimer = 0.0f;
    int asteroidVariant = 1; // Loại thiên thạch (1 hoặc 2)

    float hitFlashTimer = 0.0f;
    Vector2 prevPosition = {0, 0};
    
    // Animation Sprite Sheet variables
    int currentAnimFrame = 0;
    float animTimer = 0.0f;

    // Trail system for asteroid: stores fading trail positions
    struct TrailPoint {
        Vector2 pos;
        float alpha;   // 0.0f = invisible, 1.0f = fully visible
        int frame;     // animation frame for trail row
    };
    std::vector<TrailPoint> trailPoints;
    float trailSpawnTimer = 0.0f;
    // Interval và tốc độ fade được dùng trực tiếp trong GameManager.cpp

    float eggDropTimer = 0.0f;
    bool canShoot = true;

    void ResetEggTimer();

    void Update(float deltaTime) override;

    void Draw() override;

    void TakeDamage(float incomingDamage) override;

    void Die() override;

    void SetMovementBehavior(std::unique_ptr<IMovementBehavior> behavior);

    void DropItem();

    Rectangle GetHitbox() const;
};

#endif // ENEMY_H
