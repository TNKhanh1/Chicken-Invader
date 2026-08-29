#ifndef SHOPUI_H
#define SHOPUI_H

#include "raylib.h"
#include <vector>
#include <map>
#include <string>

class ShopUI {
private:
    int screenWidth;
    int screenHeight;

    // Tabs
    enum class ShopTab { WEAPONS, EGGS };
    ShopTab currentTab;

    // Layout
    Rectangle weaponsTabBtn;
    Rectangle eggsTabBtn;
    Rectangle backBtn;
    
    // Texture cache for items
    std::map<std::string, Texture2D> textureCache;

    // Scroll handling for items (if many)
    float scrollOffset;
    
    void DrawWeaponTab();
    void DrawEggTab();
    void HandleWeaponInput();
    void HandleEggInput();
    
    Texture2D GetOrLoadTexture(const std::string& path);

public:
    ShopUI(int sw, int sh);
    ~ShopUI();

    void Init();
    void Update();
    void Draw();
    void Cleanup();
};

#endif // SHOPUI_H
