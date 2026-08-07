#include "EnemyFactory.h"

std::unique_ptr<Enemy> EnemyFactory::CreateEnemy(int visualId, EnemyRole role, const EnemyStats& stats, Vector2 position) {
    auto enemy = std::make_unique<Enemy>(visualId, role, stats, position);
    return enemy;
}
