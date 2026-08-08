#include "EnemyFactory.h"
#include "Bosses.h"

std::unique_ptr<Enemy> EnemyFactory::CreateEnemy(int visualId, EnemyRole role, const EnemyStats& stats, Vector2 position) {
    if (role == EnemyRole::BOSS && visualId == 10) {
        return std::make_unique<FirePhoenixBoss>(visualId, stats, position, stats.hp, stats.hp * 1.5f);
    }
    auto enemy = std::make_unique<Enemy>(visualId, role, stats, position);
    return enemy;
}
