#include "EnemyFactory.h"

std::unique_ptr<Enemy> EnemyFactory::CreateEnemy(EnemyType type, Vector2 startPos) {
    std::unique_ptr<Enemy> enemy;
    switch (type) {
        case EnemyType::NORMAL_CHICKEN:
            enemy = std::make_unique<Enemy>(startPos, 50.0f, 10.0f, 5.0f, 100.0f, 10);
            break;
        case EnemyType::SWARM_CHICKEN:
            enemy = std::make_unique<Enemy>(startPos, 10.0f, 5.0f, 0.0f, 200.0f, 5);
            break;
        case EnemyType::TANK_CHICKEN:
            enemy = std::make_unique<Enemy>(startPos, 200.0f, 30.0f, 20.0f, 50.0f, 50);
            break;
        case EnemyType::BOSS:
            enemy = std::make_unique<Enemy>(startPos, 5000.0f, 100.0f, 50.0f, 80.0f, 1000);
            break;
        case EnemyType::ASTEROID:
            enemy = std::make_unique<Enemy>(startPos, 300.0f, 50.0f, 20.0f, 250.0f, 50);
            break;
        default:
            enemy = std::make_unique<Enemy>(startPos, 50.0f, 10.0f, 5.0f, 100.0f, 10);
            break;
    }
    enemy->enemyType = static_cast<int>(type);
    return enemy;
}
