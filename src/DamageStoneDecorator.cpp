#include "DamageStoneDecorator.h"

DamageStoneDecorator::DamageStoneDecorator(std::shared_ptr<Spaceship> ship, float bonusDmg) 
    : SpaceshipDecorator(ship), bonusDamage(bonusDmg) {}
    
float DamageStoneDecorator::GetDamage() const {
    return SpaceshipDecorator::GetDamage() + bonusDamage;
}
