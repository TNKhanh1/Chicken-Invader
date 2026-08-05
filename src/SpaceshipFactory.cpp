#include "SpaceshipFactory.h"
#include "SpaceshipDataManager.h"

std::unique_ptr<Spaceship> SpaceshipFactory::CreateSpaceship(const std::string& name, int level, Vector2 startPos) {
    // Tự động đọc lại file CSV mỗi khi khởi dựng phi thuyền (Game Startup / Init Session)
    SpaceshipDataManager::GetInstance()->LoadCSV("assets/spaceship/spaceship.csv");
    SpaceshipStats stats = SpaceshipDataManager::GetInstance()->GetStats(name, level);
    
    auto ship = std::make_unique<Spaceship>(
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
    ship->SetLevel(level);
    ship->ReloadStatsFromCSV(); // Khảo nghiệm chỉ số hợp lệ và in log xác nhận nghiệm thu
    return ship;
}
