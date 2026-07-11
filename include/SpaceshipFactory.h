#ifndef SPACESHIP_FACTORY_H
#define SPACESHIP_FACTORY_H

#include "Spaceship.h"
#include <memory>

// Factory Method Pattern để tạo ra các loại phi thuyền khác nhau
class SpaceshipFactory {
public:
    enum class ShipType {
        TANKER, // Máu trâu, chậm
        SNIPER, // Dame to, bắn chậm
        FIGHTER // Bắn nhanh, cân bằng
    };

    static std::unique_ptr<Spaceship> CreateSpaceship(ShipType type, Vector2 startPos) {
        switch (type) {
            case ShipType::TANKER:
                // hp, dmg, armor, speed, critC, critD, mana, atkSpd
                return std::make_unique<Spaceship>(startPos, 500.0f, 10.0f, 20.0f, 200.0f, 5.0f, 1.5f, 100.0f, 1.0f);
            
            case ShipType::SNIPER:
                return std::make_unique<Spaceship>(startPos, 200.0f, 50.0f, 5.0f, 250.0f, 20.0f, 2.0f, 100.0f, 0.5f);
            
            case ShipType::FIGHTER:
                return std::make_unique<Spaceship>(startPos, 300.0f, 20.0f, 10.0f, 300.0f, 10.0f, 1.5f, 100.0f, 2.5f);
            
            default:
                return std::make_unique<Spaceship>(startPos, 250.0f, 15.0f, 10.0f, 250.0f, 5.0f, 1.5f, 100.0f, 1.0f);
        }
    }
};

#endif // SPACESHIP_FACTORY_H
