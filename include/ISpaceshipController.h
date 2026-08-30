#ifndef ISPACESHIP_CONTROLLER_H
#define ISPACESHIP_CONTROLLER_H

#include <vector>
#include <memory>

class Spaceship;
class Bullet;
class Enemy;

class ISpaceshipController {
public:
    virtual ~ISpaceshipController() = default;
    virtual void Update(
        Spaceship* ship,
        float deltaTime,
        const std::vector<std::shared_ptr<Bullet>>& activeBullets,
        const std::vector<std::shared_ptr<Enemy>>& activeEnemies,
        bool isBossCutscene,
        int screenWidth,
        int screenHeight
    ) = 0;
};

#endif // ISPACESHIP_CONTROLLER_H
