#ifndef SINGLE_SHOT_H
#define SINGLE_SHOT_H

#include "WeaponStrategy.h"

class SingleShot : public IShootingBehavior {
public:
    void Shoot(Spaceship* ship) override;
};

#endif // SINGLE_SHOT_H
