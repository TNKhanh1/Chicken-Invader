#include "ShopUI.h"
#include "ShopManager.h"
#include "CoinManager.h"
#include "FontManager.h"
#include "GameManager.h"

ShopUI::ShopUI(int sw, int sh) : screenWidth(sw), screenHeight(sh), currentTab(ShopTab::WEAPONS), scrollOffset(0.0f) {
    weaponsTabBtn = { 50.0f, 100.0f, 200.0f, 50.0f };
    eggsTabBtn = { 260.0f, 100.0f, 200.0f, 50.0f };
    backBtn = { 50.0f, (float)sh - 80.0f, 150.0f, 50.0f };
}

ShopUI::~ShopUI() {
    Cleanup();
}

void ShopUI::Init() {
    // Load some default textures if needed, or delay load via GetOrLoadTexture
}

void ShopUI::Cleanup() {
    for (auto& pair : textureCache) {
        UnloadTexture(pair.second);
    }
    textureCache.clear();
}

Texture2D ShopUI::GetOrLoadTexture(const std::string& path) {
    if (path.empty()) return Texture2D{0};
    
    if (textureCache.find(path) == textureCache.end()) {
        Texture2D tex = LoadTexture(path.c_str());
        SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
        textureCache[path] = tex;
    }
    return textureCache[path];
}

void ShopUI::Update() {
    Vector2 mousePoint = GetMousePosition();

    // Check Back button
    if (CheckCollisionPointRec(mousePoint, backBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        GameManager::GetInstance()->ChangeState(GameState::MAIN_MENU);
        return;
    }

    // Check Tabs
    if (CheckCollisionPointRec(mousePoint, weaponsTabBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        currentTab = ShopTab::WEAPONS;
        scrollOffset = 0.0f;
    }
    if (CheckCollisionPointRec(mousePoint, eggsTabBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        currentTab = ShopTab::EGGS;
        scrollOffset = 0.0f;
    }

    // Scroll
    float wheelMove = GetMouseWheelMove();
    if (wheelMove != 0) {
        scrollOffset += wheelMove * 30.0f;
        if (scrollOffset > 0) scrollOffset = 0.0f;
    }

    // Handle items
    if (currentTab == ShopTab::WEAPONS) {
        HandleWeaponInput();
    } else {
        HandleEggInput();
    }
}

void ShopUI::HandleWeaponInput() {
    Vector2 mousePoint = GetMousePosition();
    const auto& weapons = ShopManager::GetInstance()->GetWeaponItems();
    
    int itemsPerRow = 3;
    float itemW = 250.0f;
    float itemH = 150.0f;
    float spacing = 20.0f;
    float totalW = itemsPerRow * itemW + (itemsPerRow - 1) * spacing;
    float startX = (screenWidth - totalW) / 2.0f;
    float startY = 180.0f + scrollOffset;

    for (size_t i = 0; i < weapons.size(); ++i) {
        int row = i / itemsPerRow;
        int col = i % itemsPerRow;
        
        Rectangle itemRect = { startX + col * (itemW + spacing), startY + row * (itemH + spacing), itemW, itemH };
        
        if (CheckCollisionPointRec(mousePoint, itemRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (ShopManager::GetInstance()->IsWeaponUnlocked(weapons[i].id)) {
                ShopManager::GetInstance()->SelectWeapon(weapons[i].id);
            } else {
                ShopManager::GetInstance()->BuyWeapon(weapons[i].id);
            }
        }
    }
}

void ShopUI::HandleEggInput() {
    Vector2 mousePoint = GetMousePosition();
    const auto& eggs = ShopManager::GetInstance()->GetEggItems();
    
    int itemsPerRow = 5;
    float itemW = 120.0f;
    float itemH = 150.0f;
    float spacing = 20.0f;
    float totalW = itemsPerRow * itemW + (itemsPerRow - 1) * spacing;
    float startX = (screenWidth - totalW) / 2.0f;
    float startY = 180.0f + scrollOffset;

    for (size_t i = 0; i < eggs.size(); ++i) {
        int row = i / itemsPerRow;
        int col = i % itemsPerRow;
        
        Rectangle itemRect = { startX + col * (itemW + spacing), startY + row * (itemH + spacing), itemW, itemH };
        
        if (CheckCollisionPointRec(mousePoint, itemRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // "egg1" -> skinIndex 1
            int skinIndex = 0;
            if (eggs[i].id.find("egg") == 0) {
                try {
                    skinIndex = std::stoi(eggs[i].id.substr(3));
                } catch(...) {}
            }
            
            if (ShopManager::GetInstance()->IsEggSkinUnlocked(skinIndex)) {
                ShopManager::GetInstance()->SelectEggSkin(skinIndex);
            } else {
                ShopManager::GetInstance()->BuyEggSkin(skinIndex);
            }
        }
    }
}

void ShopUI::Draw() {
    // Background
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, { 20, 20, 40, 255 }, { 10, 10, 20, 255 });
    
    // Title & Coin
    FontManager::GetInstance()->DrawGameText("SHOP", 50, 30, 40, ORANGE, "Retro");
    
    char coinStr[64];
    snprintf(coinStr, sizeof(coinStr), "COINS: %d", CoinManager::GetInstance()->GetTotalCoins());
    FontManager::GetInstance()->DrawGameText(coinStr, screenWidth - 250, 40, 30, YELLOW, "Retro");

    // Tabs
    Color wpColor = (currentTab == ShopTab::WEAPONS) ? DARKGRAY : LIGHTGRAY;
    Color egColor = (currentTab == ShopTab::EGGS) ? DARKGRAY : LIGHTGRAY;
    
    DrawRectangleRec(weaponsTabBtn, wpColor);
    DrawRectangleLinesEx(weaponsTabBtn, 2, WHITE);
    FontManager::GetInstance()->DrawGameTextCentered("WEAPONS", weaponsTabBtn.x + weaponsTabBtn.width/2, weaponsTabBtn.y + 15, 20, WHITE, "Retro");

    DrawRectangleRec(eggsTabBtn, egColor);
    DrawRectangleLinesEx(eggsTabBtn, 2, WHITE);
    FontManager::GetInstance()->DrawGameTextCentered("EGG SKINS", eggsTabBtn.x + eggsTabBtn.width/2, eggsTabBtn.y + 15, 20, WHITE, "Retro");

    // Items
    BeginScissorMode(0, 160, screenWidth, screenHeight - 160 - 100);
    if (currentTab == ShopTab::WEAPONS) DrawWeaponTab();
    else DrawEggTab();
    EndScissorMode();

    // Back btn
    Vector2 mp = GetMousePosition();
    Color bColor = CheckCollisionPointRec(mp, backBtn) ? GRAY : DARKGRAY;
    DrawRectangleRec(backBtn, bColor);
    DrawRectangleLinesEx(backBtn, 2, WHITE);
    FontManager::GetInstance()->DrawGameTextCentered("BACK", backBtn.x + backBtn.width/2, backBtn.y + 15, 20, WHITE, "Retro");
}

void ShopUI::DrawWeaponTab() {
    const auto& weapons = ShopManager::GetInstance()->GetWeaponItems();
    std::string selected = ShopManager::GetInstance()->GetSelectedWeapon();
    Vector2 mp = GetMousePosition();

    int itemsPerRow = 3;
    float itemW = 250.0f;
    float itemH = 150.0f;
    float spacing = 20.0f;
    float totalW = itemsPerRow * itemW + (itemsPerRow - 1) * spacing;
    float startX = (screenWidth - totalW) / 2.0f;
    float startY = 180.0f + scrollOffset;

    for (size_t i = 0; i < weapons.size(); ++i) {
        int row = i / itemsPerRow;
        int col = i % itemsPerRow;
        Rectangle rect = { startX + col * (itemW + spacing), startY + row * (itemH + spacing), itemW, itemH };
        
        bool isUnlocked = ShopManager::GetInstance()->IsWeaponUnlocked(weapons[i].id);
        bool isSelected = (selected == weapons[i].id);
        bool isHovered = CheckCollisionPointRec(mp, rect);

        Color bgColor = { 40, 40, 60, 200 };
        Color borderColor = DARKGRAY;
        
        if (isSelected) {
            bgColor = { 40, 80, 40, 200 };
            borderColor = GREEN;
        } else if (isUnlocked) {
            bgColor = { 60, 60, 80, 200 };
            borderColor = SKYBLUE;
        } else {
            bgColor = { 80, 40, 40, 200 }; // Locked
        }
        
        if (isHovered && (!isSelected)) {
            bgColor.a = 255;
            rect.x -= 2; rect.y -= 2;
            rect.width += 4; rect.height += 4;
        }

        DrawRectangleRounded(rect, 0.1f, 10, bgColor);
        DrawRectangleRoundedLinesEx(rect, 0.1f, 10, 2.0f, borderColor);

        // Name
        FontManager::GetInstance()->DrawGameTextCentered(weapons[i].name, rect.x + itemW/2, rect.y + 20, 20, WHITE, "Modern");
        
        // Status
        if (isSelected) {
            FontManager::GetInstance()->DrawGameTextCentered("EQUIPPED", rect.x + itemW/2, rect.y + 80, 18, GREEN, "Modern");
        } else if (isUnlocked) {
            FontManager::GetInstance()->DrawGameTextCentered("CLICK TO SELECT", rect.x + itemW/2, rect.y + 80, 16, SKYBLUE, "Modern");
        } else {
            char priceStr[32];
            snprintf(priceStr, sizeof(priceStr), "PRICE: %d", weapons[i].price);
            FontManager::GetInstance()->DrawGameTextCentered(priceStr, rect.x + itemW/2, rect.y + 80, 18, YELLOW, "Modern");
        }
    }
}

void ShopUI::DrawEggTab() {
    const auto& eggs = ShopManager::GetInstance()->GetEggItems();
    int selected = ShopManager::GetInstance()->GetSelectedEggSkin();
    Vector2 mp = GetMousePosition();

    int itemsPerRow = 5;
    float itemW = 120.0f;
    float itemH = 150.0f;
    float spacing = 20.0f;
    float totalW = itemsPerRow * itemW + (itemsPerRow - 1) * spacing;
    float startX = (screenWidth - totalW) / 2.0f;
    float startY = 180.0f + scrollOffset;

    for (size_t i = 0; i < eggs.size(); ++i) {
        int skinIndex = 0;
        if (eggs[i].id.find("egg") == 0) {
            try { skinIndex = std::stoi(eggs[i].id.substr(3)); } catch(...) {}
        }
        
        int row = i / itemsPerRow;
        int col = i % itemsPerRow;
        Rectangle rect = { startX + col * (itemW + spacing), startY + row * (itemH + spacing), itemW, itemH };
        
        bool isUnlocked = ShopManager::GetInstance()->IsEggSkinUnlocked(skinIndex);
        bool isSelected = (selected == skinIndex);
        bool isHovered = CheckCollisionPointRec(mp, rect);

        Color bgColor = { 40, 40, 60, 200 };
        Color borderColor = DARKGRAY;
        
        if (isSelected) {
            bgColor = { 40, 80, 40, 200 };
            borderColor = GREEN;
        } else if (isUnlocked) {
            bgColor = { 60, 60, 80, 200 };
            borderColor = SKYBLUE;
        }
        
        if (isHovered && (!isSelected)) {
            bgColor.a = 255;
            rect.x -= 2; rect.y -= 2;
            rect.width += 4; rect.height += 4;
        }

        DrawRectangleRounded(rect, 0.15f, 10, bgColor);
        DrawRectangleRoundedLinesEx(rect, 0.15f, 10, 2.0f, borderColor);

        // Preview Image
        Texture2D tex = GetOrLoadTexture(eggs[i].previewPath);
        if (tex.id != 0) {
            float scale = 60.0f / tex.width;
            if (tex.height * scale > 60.0f) {
                scale = 60.0f / tex.height;
            }
            float drawW = tex.width * scale;
            float drawH = tex.height * scale;
            DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height},
                           {rect.x + itemW/2 - drawW/2, rect.y + 20, drawW, drawH},
                           {0,0}, 0, WHITE);
        }

        // Status
        if (isSelected) {
            FontManager::GetInstance()->DrawGameTextCentered("EQUIPPED", rect.x + itemW/2, rect.y + 110, 14, GREEN, "Modern");
        } else if (isUnlocked) {
            FontManager::GetInstance()->DrawGameTextCentered("OWNED", rect.x + itemW/2, rect.y + 110, 14, SKYBLUE, "Modern");
        } else {
            char priceStr[32];
            snprintf(priceStr, sizeof(priceStr), "%d COIN", eggs[i].price);
            FontManager::GetInstance()->DrawGameTextCentered(priceStr, rect.x + itemW/2, rect.y + 110, 14, YELLOW, "Modern");
        }
    }
}
