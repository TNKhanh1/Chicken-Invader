#include "UIManager.h"
#include "GameManager.h"
UIManager::UIManager() : playerHp(0), playerMaxHp(100), playerMana(0), playerMaxMana(100), playerLevel(1), playerExp(0) {}

void UIManager::OnNotify(EventType event, const std::string& data) {
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

void UIManager::DrawUI() {
    DrawText(TextFormat("Level: %d", playerLevel), 10, 10, 20, BLACK);
    DrawText(TextFormat("Exp: %.1f", playerExp), 10, 40, 20, DARKBLUE);
    DrawText(TextFormat("Mana: %.1f", playerMana), 10, 70, 20, BLUE);
}
