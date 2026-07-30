#ifndef SPACESHIP_FACTORY_H
#define SPACESHIP_FACTORY_H

#include "Spaceship.h"
#include <memory>

// Factory Method Pattern để tạo ra các loại phi thuyền khác nhau
class SpaceshipFactory {
public:
    static std::unique_ptr<Spaceship> CreateSpaceship(const std::string& name, int level, Vector2 startPos);
};

#endif // SPACESHIP_FACTORY_H
