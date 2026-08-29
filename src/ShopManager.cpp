#include "ShopManager.h"
#include "CoinManager.h"
#include "json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

ShopManager* ShopManager::instance = nullptr;

ShopManager* ShopManager::GetInstance() {
    if (!instance) {
        instance = new ShopManager();
    }
    return instance;
}

void ShopManager::DestroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

ShopManager::ShopManager() : selectedWeapon("Hypergun"), selectedEggSkin(0), dataFilePath("data/shop.json") {
    BuildCatalog();
    unlockedWeapons.insert("Hypergun");
    unlockedEggSkins.insert(0);
}

void ShopManager::BuildCatalog() {
    // Vũ khí
    weaponItems.push_back({"Hypergun", "Hypergun", 0, ""});
    weaponItems.push_back({"Neutron_Gun", "Neutron Gun", 300, ""});
    weaponItems.push_back({"Riddler", "Riddler", 500, ""});
    weaponItems.push_back({"Ion_Blaster", "Ion Blaster", 700, ""});
    weaponItems.push_back({"Utensil_Poker", "Utensil Poker", 900, ""});
    weaponItems.push_back({"Lightning_Fryer", "Lightning Fryer", 1000, ""});
    weaponItems.push_back({"Plasma_Rifle", "Plasma Rifle", 1200, ""});
    weaponItems.push_back({"Laser_Cannon", "Laser Cannon", 1500, ""});

    // Egg Skins
    eggItems.push_back({"egg0", "Default Egg", 0, "assets/egg.png"});
    
    // Group 1: 100 coins
    for (int i = 1; i <= 5; ++i) {
        std::string id = "egg" + std::to_string(i);
        char path[64];
        snprintf(path, sizeof(path), "assets/egg/egg%02d.png", i);
        eggItems.push_back({id, "Egg Skin " + std::to_string(i), 100, path});
    }
    // Group 2: 200 coins
    for (int i = 6; i <= 10; ++i) {
        std::string id = "egg" + std::to_string(i);
        char path[64];
        snprintf(path, sizeof(path), "assets/egg/egg%02d.png", i);
        eggItems.push_back({id, "Egg Skin " + std::to_string(i), 200, path});
    }
    // Group 3: 300 coins
    for (int i = 11; i <= 15; ++i) {
        std::string id = "egg" + std::to_string(i);
        char path[64];
        snprintf(path, sizeof(path), "assets/egg/egg%02d.png", i);
        eggItems.push_back({id, "Egg Skin " + std::to_string(i), 300, path});
    }
    // Group 4: 500 coins
    for (int i = 16; i <= 20; ++i) {
        std::string id = "egg" + std::to_string(i);
        char path[64];
        snprintf(path, sizeof(path), "assets/egg/egg%02d.png", i);
        eggItems.push_back({id, "Egg Skin " + std::to_string(i), 500, path});
    }
}

void ShopManager::Load() {
    std::ifstream file(dataFilePath);
    if (!file.is_open()) return;

    try {
        json j;
        file >> j;
        
        if (j.contains("selectedWeapon")) selectedWeapon = j["selectedWeapon"];
        if (j.contains("selectedEggSkin")) selectedEggSkin = j["selectedEggSkin"];
        
        if (j.contains("unlockedWeapons")) {
            for (const auto& w : j["unlockedWeapons"]) {
                unlockedWeapons.insert(w.get<std::string>());
            }
        }
        if (j.contains("unlockedEggSkins")) {
            for (const auto& e : j["unlockedEggSkins"]) {
                unlockedEggSkins.insert(e.get<int>());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to load shop data: " << e.what() << std::endl;
    }
}

void ShopManager::Save() const {
    json j;
    j["selectedWeapon"] = selectedWeapon;
    j["selectedEggSkin"] = selectedEggSkin;
    
    j["unlockedWeapons"] = json::array();
    for (const auto& w : unlockedWeapons) {
        j["unlockedWeapons"].push_back(w);
    }
    
    j["unlockedEggSkins"] = json::array();
    for (const auto& e : unlockedEggSkins) {
        j["unlockedEggSkins"].push_back(e);
    }

    std::ofstream file(dataFilePath);
    if (file.is_open()) {
        file << j.dump(4);
    }
}

bool ShopManager::BuyWeapon(const std::string& weaponId) {
    if (IsWeaponUnlocked(weaponId)) return false;

    int price = 0;
    for (const auto& item : weaponItems) {
        if (item.id == weaponId) {
            price = item.price;
            break;
        }
    }

    if (CoinManager::GetInstance()->SpendCoins(price)) {
        unlockedWeapons.insert(weaponId);
        Save();
        return true;
    }
    return false;
}

bool ShopManager::BuyEggSkin(int skinIndex) {
    if (IsEggSkinUnlocked(skinIndex)) return false;
    
    int price = 0;
    for (const auto& item : eggItems) {
        if (item.id == "egg" + std::to_string(skinIndex)) {
            price = item.price;
            break;
        }
    }

    if (CoinManager::GetInstance()->SpendCoins(price)) {
        unlockedEggSkins.insert(skinIndex);
        Save();
        return true;
    }
    return false;
}

bool ShopManager::SelectWeapon(const std::string& weaponId) {
    if (IsWeaponUnlocked(weaponId)) {
        selectedWeapon = weaponId;
        Save();
        return true;
    }
    return false;
}

bool ShopManager::SelectEggSkin(int skinIndex) {
    if (IsEggSkinUnlocked(skinIndex)) {
        selectedEggSkin = skinIndex;
        Save();
        return true;
    }
    return false;
}

std::string ShopManager::GetSelectedWeapon() const {
    return selectedWeapon;
}

int ShopManager::GetSelectedEggSkin() const {
    return selectedEggSkin;
}

std::string ShopManager::GetSelectedEggTexturePath() const {
    if (selectedEggSkin == 0) return "assets/egg.png";
    char path[64];
    snprintf(path, sizeof(path), "assets/egg/egg%02d.png", selectedEggSkin);
    return std::string(path);
}

bool ShopManager::IsWeaponUnlocked(const std::string& weaponId) const {
    return unlockedWeapons.find(weaponId) != unlockedWeapons.end();
}

bool ShopManager::IsEggSkinUnlocked(int skinIndex) const {
    return unlockedEggSkins.find(skinIndex) != unlockedEggSkins.end();
}

const std::vector<ShopItem>& ShopManager::GetWeaponItems() const {
    return weaponItems;
}

const std::vector<ShopItem>& ShopManager::GetEggItems() const {
    return eggItems;
}
