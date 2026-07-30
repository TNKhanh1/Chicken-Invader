#include "SpaceshipFactory.h"
#include "SpaceshipDataManager.h"

std::unique_ptr<Spaceship> SpaceshipFactory::CreateSpaceship(const std::string& name, int level, Vector2 startPos) {
    SpaceshipStats stats = SpaceshipDataManager::GetInstance()->GetStats(name, level);
    
    return std::make_unique<Spaceship>(
        name,
        startPos, 
        stats.hp, 
        stats.damage, 
        stats.armor, 
        stats.moveSpeed, 
        stats.critChance, 
        stats.critDamage, 
        stats.maxMana, 
        stats.attackSpeed
    );
}
