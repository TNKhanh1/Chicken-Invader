#ifndef ENEMY_FACTORY_H
#define ENEMY_FACTORY_H

#include "Enemy.h"
#include <memory>

// Factory Method Pattern để tạo ra các loại gà
class EnemyFactory {
public:
    enum class EnemyType {
        NORMAL_CHICKEN,
        SWARM_CHICKEN, // Máu ít, xuất hiện nhiều
        TANK_CHICKEN,  // Máu nhiều, đi chậm
        BOSS
    };

    static std::unique_ptr<Enemy> CreateEnemy(EnemyType type, Vector2 startPos) {
        switch (type) {
            case EnemyType::NORMAL_CHICKEN:
                // pos, hp, dmg, armor, spd, points
                return std::make_unique<Enemy>(startPos, 50.0f, 10.0f, 5.0f, 100.0f, 10);
                
            case EnemyType::SWARM_CHICKEN:
                return std::make_unique<Enemy>(startPos, 10.0f, 5.0f, 0.0f, 200.0f, 5);
                
            case EnemyType::TANK_CHICKEN:
                return std::make_unique<Enemy>(startPos, 200.0f, 30.0f, 20.0f, 50.0f, 50);
                
            case EnemyType::BOSS:
                return std::make_unique<Enemy>(startPos, 5000.0f, 100.0f, 50.0f, 80.0f, 1000);
                
            default:
                return std::make_unique<Enemy>(startPos, 50.0f, 10.0f, 5.0f, 100.0f, 10);
        }
    }
};

#endif // ENEMY_FACTORY_H
