#ifndef SPREAD_SHOT_H
#define SPREAD_SHOT_H

#include "WeaponStrategy.h"

class SpreadShot : public IShootingBehavior {
public:
    void Shoot(Spaceship* ship) override;
};

#endif // SPREAD_SHOT_H
