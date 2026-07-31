#include "SpaceshipDataManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

SpaceshipDataManager* SpaceshipDataManager::instance = nullptr;

SpaceshipDataManager::SpaceshipDataManager() {}

SpaceshipDataManager* SpaceshipDataManager::GetInstance() {
    if (instance == nullptr) {
        instance = new SpaceshipDataManager();
    }
    return instance;
}

void SpaceshipDataManager::LoadCSV(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filepath << std::endl;
        return;
    }

    std::string line;
    // Bỏ qua dòng tiêu đề
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string token;
        SpaceshipStats stats;

        std::getline(ss, stats.name, ',');
        
        std::getline(ss, token, ','); stats.level = std::stoi(token);
        std::getline(ss, token, ','); stats.hp = std::stof(token);
        std::getline(ss, token, ','); stats.damage = std::stof(token);
        std::getline(ss, token, ','); stats.armor = std::stof(token);
        std::getline(ss, token, ','); stats.attackSpeed = std::stof(token);
        std::getline(ss, token, ','); stats.moveSpeed = std::stof(token);
        std::getline(ss, token, ','); stats.maxMana = std::stof(token);
        std::getline(ss, token, ','); stats.critChance = std::stof(token);
        std::getline(ss, token, ','); stats.critDamage = std::stof(token);

        statsMap[{stats.name, stats.level}] = stats;
    }
    
    file.close();
}

SpaceshipStats SpaceshipDataManager::GetStats(const std::string& name, int level) const {
    auto it = statsMap.find({name, level});
    if (it != statsMap.end()) {
        return it->second;
    }
    
    std::cerr << "Stats not found for " << name << " level " << level << std::endl;
    return SpaceshipStats{"", 0, 100, 10, 10, 1.0f, 200, 100, 0, 150};
}

void SpaceshipDataManager::LoadJSON(const std::string& name, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filepath << std::endl;
        return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    jsonCache[name] = buffer.str();
    file.close();
}

std::string SpaceshipDataManager::GetJSON(const std::string& name) const {
    auto it = jsonCache.find(name);
    if (it != jsonCache.end()) {
        return it->second;
    }
    return "{}";
}
