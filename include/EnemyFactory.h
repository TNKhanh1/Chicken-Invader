#ifndef ENEMY_FACTORY_H
#define ENEMY_FACTORY_H

#include "Enemy.h"
#include <memory>

// Factory Method Pattern để tạo ra các loại gà
class EnemyFactory {
public:
    enum class EnemyType {
        NORMAL_CHICKEN,
        SWARM_CHICKEN,
        TANK_CHICKEN,
        BOSS,
        ASTEROID
    };

    static std::unique_ptr<Enemy> CreateEnemy(EnemyType type, Vector2 startPos);
};

#endif // ENEMY_FACTORY_H
