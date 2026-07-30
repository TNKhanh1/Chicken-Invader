#ifndef DAMAGE_STONE_DECORATOR_H
#define DAMAGE_STONE_DECORATOR_H

#include "SpaceshipDecorator.h"

class DamageStoneDecorator : public SpaceshipDecorator {
private:
    float bonusDamage;
public:
    DamageStoneDecorator(std::shared_ptr<Spaceship> ship, float bonusDmg);
        
    float GetDamage() const override;
};

#endif // DAMAGE_STONE_DECORATOR_H
