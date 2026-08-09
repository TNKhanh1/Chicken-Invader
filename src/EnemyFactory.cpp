#include "EnemyFactory.h"
#include "Bosses.h"

std::unique_ptr<Enemy> EnemyFactory::CreateEnemy(int visualId, EnemyRole role, const EnemyStats& stats, Vector2 position) {
    if (role == EnemyRole::BOSS) {
        if (visualId == 10) {
            return std::make_unique<FirePhoenixBoss>(visualId, stats, position, stats.hp, stats.hp * 1.5f);
        } else if (visualId == 11) {
            return std::make_unique<EggsecutionerBoss>(visualId, stats, position);
        }
    }
    auto enemy = std::make_unique<Enemy>(visualId, role, stats, position);
    return enemy;
}
