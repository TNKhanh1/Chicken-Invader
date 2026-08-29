#ifndef SHOPMANAGER_H
#define SHOPMANAGER_H

#include <string>
#include <vector>
#include <set>

struct ShopItem {
    std::string id;
    std::string name;
    int price;
    std::string previewPath;
};

class ShopManager {
private:
    static ShopManager* instance;
    ShopManager();
    ~ShopManager() = default;

    std::set<std::string> unlockedWeapons;
    std::set<int> unlockedEggSkins;
    std::string selectedWeapon;
    int selectedEggSkin;

    std::vector<ShopItem> weaponItems;
    std::vector<ShopItem> eggItems;
    std::string dataFilePath;

    void BuildCatalog();

public:
    // Delete copy constructor and assignment operator
    ShopManager(const ShopManager&) = delete;
    void operator=(const ShopManager&) = delete;

    static ShopManager* GetInstance();
    static void DestroyInstance();

    void Load();
    void Save() const;

    bool BuyWeapon(const std::string& weaponId);
    bool BuyEggSkin(int skinIndex);
    bool SelectWeapon(const std::string& weaponId);
    bool SelectEggSkin(int skinIndex);

    std::string GetSelectedWeapon() const;
    int GetSelectedEggSkin() const;
    std::string GetSelectedEggTexturePath() const;
    bool IsWeaponUnlocked(const std::string& weaponId) const;
    bool IsEggSkinUnlocked(int skinIndex) const;

    const std::vector<ShopItem>& GetWeaponItems() const;
    const std::vector<ShopItem>& GetEggItems() const;
};

#endif // SHOPMANAGER_H
