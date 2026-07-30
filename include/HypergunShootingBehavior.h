#ifndef HYPERGUN_SHOOTING_BEHAVIOR_H
#define HYPERGUN_SHOOTING_BEHAVIOR_H

#include "WeaponStrategy.h"
#include <map>
#include <string>
#include "raylib.h"

class HypergunShootingBehavior : public IShootingBehavior {
private:
    std::map<std::string, Vector2> weaponPods;
    int shotCount;
    int frontAngleIndex;
    int rearTimer;
    int wingTimer;
    
public:
    HypergunShootingBehavior();
    void LoadWeaponPods(const std::string& filepath);
    void Shoot(Spaceship* ship) override;
};

#endif // HYPERGUN_SHOOTING_BEHAVIOR_H
