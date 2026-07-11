#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "Observer.h"
#include "raylib.h"
#include <iostream>

// Hệ thống quản lý UI (Observer lắng nghe các sự kiện từ Model)
class UIManager : public IObserver {
private:
    float playerHp;
    float playerMaxHp;
    float playerMana;
    float playerMaxMana;
    int playerLevel;
    float playerExp;

public:
    UIManager() : playerHp(0), playerMaxHp(100), playerMana(0), playerMaxMana(100), playerLevel(1), playerExp(0) {}

    // Kế thừa hàm OnNotify từ IObserver
    void OnNotify(EventType event, const std::string& data) override {
        switch (event) {
            case EventType::PLAYER_TOOK_DAMAGE:
                // parse data để update máu
                break;
            case EventType::PLAYER_MANA_CHANGED:
                playerMana = std::stof(data);
                break;
            case EventType::PLAYER_EXP_GAINED:
                playerExp = std::stof(data);
                break;
            case EventType::PLAYER_LEVEL_UP:
                playerLevel = std::stoi(data);
                break;
            default:
                break;
        }
    }

    // Hàm gọi trong GameManager để vẽ UI lên màn hình
    void DrawUI() {
        // Vẽ thanh máu
        DrawText(TextFormat("Level: %d", playerLevel), 10, 10, 20, BLACK);
        DrawText(TextFormat("Exp: %.1f", playerExp), 10, 40, 20, DARKBLUE);
        DrawText(TextFormat("Mana: %.1f", playerMana), 10, 70, 20, BLUE);
        // Có thể vẽ thêm Rectangle làm thanh bar sau này
    }
};

#endif // UIMANAGER_H
