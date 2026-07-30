#ifndef SPACESHIP_DATA_MANAGER_H
#define SPACESHIP_DATA_MANAGER_H

#include <string>
#include <map>
#include <utility>

struct SpaceshipStats {
    std::string name;
    int level;
    float hp;
    float damage;
    float armor;
    float attackSpeed;
    float moveSpeed;
    float maxMana;
    float critChance;
    float critDamage;
};

class SpaceshipDataManager {
private:
    static SpaceshipDataManager* instance;
    std::map<std::pair<std::string, int>, SpaceshipStats> statsMap;

    SpaceshipDataManager();
    ~SpaceshipDataManager() = default;
public:
    SpaceshipDataManager(const SpaceshipDataManager&) = delete;
    void operator=(const SpaceshipDataManager&) = delete;

    static SpaceshipDataManager* GetInstance();
    void LoadCSV(const std::string& filepath);
    SpaceshipStats GetStats(const std::string& name, int level) const;
};

#endif // SPACESHIP_DATA_MANAGER_H
