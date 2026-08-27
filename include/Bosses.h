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

class BouncingRedBullet : public Bullet {
private:
    int bounceCount;
    int maxBounces;
public:
    BouncingRedBullet(Vector2 startPos, Vector2 velocity);
    void Update(float deltaTime) override;
    void Draw() override;
};

// --- Red Boss Bullet (Straight) ---
class RedBossBullet : public Bullet {
public:
    RedBossBullet(Vector2 startPos, Vector2 velocity);
    void Update(float deltaTime) override;
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
    float drawScale;
    
    
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

class FireBullet : public Bullet {
public:
    enum class Type { NORMAL, LARGE, EXPLOSIVE };
private:
    Type type;
    float glowPulse;
    float lifeTimer;
    Vector2 initialVelocity;

public:
    FireBullet(Vector2 startPos, Vector2 velocity, float damage, Type type = Type::NORMAL);
    void Update(float deltaTime) override;
    void Draw() override;
};

// --- Fire Phoenix Boss (Stage 1, Wave 5) ---
class FirePhoenixBoss : public Boss {
public:
    enum class Phase { PHASE_1, TRANSITIONING, PHASE_2 };

private:
    Phase currentPhase;
    float phase1MaxHp;
    float phase2MaxHp;

    // Phase transition
    float transitionTimer;
    bool isInvulnerable;
    int transitionFlashCount;
    float transitionFlashTimer;

    // Attack system
    float attackTimer;
    int normalAttackCount;
    int normalAttacksBeforeSkill; // Phase1: 4, Phase2: 3
    float attackCooldown;         // Phase1: 2.5s, Phase2: 1.8s
    int nextSkillType;            // 0: Fireball, 1: Fire Rain (alternating)

    // Wing spark particle system
    struct SparkParticle {
        Vector2 pos;
        Vector2 velocity;
        float alpha;
        float size;
        float rotation;
        float rotationSpeed;
        Color color;
        bool isTexture;
    };
    std::vector<SparkParticle> sparks;
    float sparkSpawnTimer;

    // Attack methods
    void FireNormalAttack();
    void FireLargeFireball();
    void FireRain();

    // Particle methods
    void SpawnWingSparks(float deltaTime);
    void UpdateSparks(float deltaTime);
    void DrawSparks();

    // Phase transition
    void StartTransition();
    void UpdateTransition(float deltaTime);
    void EnterPhase2();

    // HP Bar
    void DrawBossHPBar();

public:
    FirePhoenixBoss(int visualId, const EnemyStats& stats, Vector2 pos,
                    float p1Hp, float p2Hp);

    void Update(float deltaTime) override;
    void Draw() override;
    void Die() override;
    void TakeDamage(float incomingDamage) override;

    Phase GetPhase() const { return currentPhase; }
};

// --- Eggsecutioner Boss (Stage 2, Wave 5) ---
class EggsecutionerBoss : public Boss {
private:
    float attackTimer;
    int nextSkillType;
    float attackCooldown;

    // Movement
    float moveTimer;
    bool isDashing;
    bool isYielding;
    Vector2 dashTarget;
    float dashSpeed;
    float normalSpeed;
    Vector2 currentVelocity;
    Vector2 intendedVel;

    // HP Bar
    void DrawBossHPBar();

    // Attack methods
    void FireBouncingBullets();
    void FireRedDarts();
    void FireRedNova();

public:
    EggsecutionerBoss(int visualId, const EnemyStats& stats, Vector2 pos);

    void Update(float deltaTime) override;
    void Draw() override;
    void Die() override;
};
class BlueRoundBullet : public Bullet {
private:
    float glowPulse;
public:
    BlueRoundBullet(Vector2 startPos, Vector2 velocity, float damage);
    void Update(float deltaTime) override;
    void Draw() override;
};

class SnowballBullet : public Bullet {
private:
    float age;
    float maxAge;
    float startRadius;
    float endRadius;
    
    struct Flake {
        Vector2 offset;
        float angle;
        float speed;
        float size;
    };
    std::vector<Flake> flakes;

public:
    SnowballBullet(Vector2 startPos, Vector2 velocity, float damage);
    void Update(float deltaTime) override;
    void Draw() override;
};

// --- Eskimo Boss (Stage 4, Wave 10) ---
class EskimoBoss : public Boss {
private:
    // Movement system
    float moveTimer;
    Vector2 targetPos;
    bool isMoving;

    // Attack system
    float attackTimer;
    int normalAttackCount;
    int normalAttacksBeforeSkill;
    float attackCooldown;
    
    // Snow flakes particle system
    struct SnowParticle {
        Vector2 pos;
        Vector2 velocity;
        float alpha;
        float size;
    };
    std::vector<SnowParticle> flakes;
    float flakeSpawnTimer;

    // Internal helpers
    void SpawnSnowFlakes(float deltaTime);
    void UpdateFlakes(float deltaTime);
    void DrawBossHPBar();

    // Attack methods
    void FireBlueBurst();
    void FireSnowballs();

public:
    EskimoBoss(int visualId, const EnemyStats& stats, Vector2 pos);

    void Update(float deltaTime) override;
    void Draw() override;
    void Die() override;
    void TakeDamage(float incomingDamage) override;
};
// --- Bomber Boss (Stage 5, Wave 10) ---
class BomberBoss : public Boss {
private:
    float attackTimer;
    int attackType; // 0: V-Shape, 1: 3-Way, 2: 7-Way
    
    // Sub-attack for V-Shape combo
    float subAttackTimer;
    int subAttackCount;

    // HP Bar
    void DrawBossHPBar();

    // Attack methods
    void FireVShape();
    void Fire3Way();
    void Fire7Way();

public:
    BomberBoss(int visualId, const EnemyStats& stats, Vector2 pos);

    void Update(float deltaTime) override;
    void Draw() override;
    void Die() override;
};

// --- VoidChickenBoss (Stage 5 Wave 15) ---
class VoidChickenBoss : public Boss {
private:
    enum class State {
        IDLE_MOVE,
        ATTACK_NORMAL,
        SKILL_LASER,
        SKILL_HOMING
    } state;

    float stateTimer;
    float attackTimer;
    float shootAngle;
    int shotsFired;

    void TransitionTo(State newState);
    void DrawBossHPBar();
    void FireNormalAttack();
    void FireLaserSkill();
    void FireHomingSkill();
    
public:
    VoidChickenBoss(int visualId, const EnemyStats& stats, Vector2 startPos);
    void Update(float deltaTime) override;
    void Draw() override;
    void Die() override;
};

#endif // BOSSES_H
