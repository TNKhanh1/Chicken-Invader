#include "EnemyFactory.h"

std::unique_ptr<Enemy> EnemyFactory::CreateEnemy(EnemyType type, Vector2 startPos, int wave) {
    std::unique_ptr<Enemy> enemy;
    
    float hpMult = 1.0f + (wave - 1) * 0.25f;
    float dmgMult = 1.0f + (wave - 1) * 0.15f;
    float scoreMult = 1.0f + (wave - 1) * 0.10f;
    
    switch (type) {
        case EnemyType::NORMAL_CHICKEN:
            enemy = std::make_unique<Enemy>(startPos, 50.0f * hpMult, 10.0f * dmgMult, 5.0f, 100.0f, (int)(10 * scoreMult));
            break;
        case EnemyType::SWARM_CHICKEN:
            enemy = std::make_unique<Enemy>(startPos, 10.0f * hpMult, 5.0f * dmgMult, 0.0f, 200.0f, (int)(5 * scoreMult));
            break;
        case EnemyType::TANK_CHICKEN:
            enemy = std::make_unique<Enemy>(startPos, 200.0f * hpMult, 30.0f * dmgMult, 20.0f, 50.0f, (int)(50 * scoreMult));
            break;
        case EnemyType::BOSS:
            enemy = std::make_unique<Enemy>(startPos, 5000.0f * hpMult, 100.0f * dmgMult, 50.0f, 80.0f, (int)(1000 * scoreMult));
            break;
        case EnemyType::ASTEROID:
            enemy = std::make_unique<Enemy>(startPos, 300.0f * hpMult, 50.0f * dmgMult, 20.0f, 250.0f, (int)(50 * scoreMult));
            break;
        default:
            enemy = std::make_unique<Enemy>(startPos, 50.0f * hpMult, 10.0f * dmgMult, 5.0f, 100.0f, (int)(10 * scoreMult));
            break;
    }
    enemy->enemyType = static_cast<int>(type);
    return enemy;
}
