#ifndef ENEMY_H
#define ENEMY_H

#include "Character.h"
#include "IMovementBehavior.h"
#include <memory>

#include <vector>

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

    void Die() override;

    void SetMovementBehavior(std::unique_ptr<IMovementBehavior> behavior);

    void DropItem();

    Rectangle GetHitbox() const;
};

#endif // ENEMY_H
