#ifndef ENEMY_FACTORY_H
#define ENEMY_FACTORY_H

#include "Enemy.h"
#include <memory>

// Factory Method Pattern để tạo ra các loại gà
class EnemyFactory {
public:
    static std::unique_ptr<Enemy> CreateEnemy(int visualId, EnemyRole role, const EnemyStats& stats, Vector2 position);
};

#endif // ENEMY_FACTORY_H
