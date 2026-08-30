#include "SummaryScreen.h"
#include "FontManager.h"
#include "GameManager.h"

SummaryScreen::SummaryScreen(int sw, int sh) : sw(sw), sh(sh) {
    Reset();
}

void SummaryScreen::Show(SummaryResult result, int score, int sessionCoins, int stageBonus, int totalCoins) {
    this->result = result;
    this->score = score;
    this->sessionCoins = sessionCoins;
    this->stageBonus = stageBonus;
    this->totalCoins = totalCoins;
    Reset();
}

void SummaryScreen::Update() {
    alphaTimer += GetFrameTime() * 2.0f; // Fade in effect
    if (alphaTimer > 1.0f) alphaTimer = 1.0f;

    // Check button click
    Vector2 mousePos = GetMousePosition();
    Rectangle backBtn = { (float)sw/2 - 120, (float)sh/2 + 150, 240, 50 };
    if (CheckCollisionPointRec(mousePos, backBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        backRequested = true;
    }
}

bool SummaryScreen::IsBackRequested() const {
    return backRequested;
}

void SummaryScreen::Reset() {
    backRequested = false;
    alphaTimer = 0.0f;
}

void SummaryScreen::Draw() {
    // Dim background
    DrawRectangle(0, 0, sw, sh, ColorAlpha(BLACK, 0.7f * alphaTimer));

    float panelW = 600;
    float panelH = 450;
    float startX = (sw - panelW) / 2.0f;
    float startY = (sh - panelH) / 2.0f;

    // Draw Panel
    DrawRectangleRounded({startX, startY, panelW, panelH}, 0.1f, 10, ColorAlpha(DARKGRAY, 0.9f * alphaTimer));
    DrawRectangleRoundedLinesEx({startX, startY, panelW, panelH}, 0.1f, 10, 4, ColorAlpha(GRAY, alphaTimer));

    // Draw Title
    const char* title = "";
    Color titleColor = WHITE;
    if (result == SummaryResult::WIN) {
        title = "STAGE COMPLETE!";
        titleColor = GREEN;
    } else if (result == SummaryResult::LOSE) {
        title = "GAME OVER!";
        titleColor = RED;
    } else {
        title = "RUN ENDED";
        titleColor = GOLD;
    }
    
    FontManager::GetInstance()->DrawGameTextCentered(title, sw/2, startY + 30, 40, ColorAlpha(titleColor, alphaTimer), "Modern");

    // Draw Score
    FontManager::GetInstance()->DrawGameTextCentered(TextFormat("FINAL SCORE: %d", score), sw/2, startY + 110, 30, ColorAlpha(WHITE, alphaTimer), "Modern");

    // Draw Coins
    const char* coinsStr = TextFormat("Coins earned: %d + %d (bonus) = %d", sessionCoins, stageBonus, sessionCoins + stageBonus);
    FontManager::GetInstance()->DrawGameTextCentered(coinsStr, sw/2, startY + 180, 20, ColorAlpha(YELLOW, alphaTimer), "Modern");

    const char* totalStr = TextFormat("Total Coins: %d", totalCoins);
    FontManager::GetInstance()->DrawGameTextCentered(totalStr, sw/2 + 10, startY + 230, 24, ColorAlpha(GOLD, alphaTimer), "Modern");
    
    // Draw coin icon near Total Coins
    Texture2D coinTex = GameManager::GetInstance()->GetTexCoin(); // Wait, GameManager doesn't have GetTexCoin public. 
    // I can just use a circle for now or expose it. Let's expose it in GameManager.
    // Or I'll just draw a circle.
    int totalStrW = FontManager::GetInstance()->MeasureGameText(totalStr, 24, "Modern").x;
    DrawCircle(sw/2 - totalStrW/2 - 20, startY + 242, 12, ColorAlpha(GOLD, alphaTimer));

    // Draw Back Button
    Rectangle backBtn = { (float)sw/2 - 120, startY + panelH - 80, 240, 50 };
    bool isHovered = CheckCollisionPointRec(GetMousePosition(), backBtn);
    DrawRectangleRounded(backBtn, 0.2f, 10, ColorAlpha(isHovered ? LIGHTGRAY : GRAY, alphaTimer));
    DrawRectangleRoundedLinesEx(backBtn, 0.2f, 10, 2, ColorAlpha(WHITE, alphaTimer));
    FontManager::GetInstance()->DrawGameTextCentered("CHOOSE MODE", sw/2, startY + panelH - 65, 24, ColorAlpha(isHovered ? BLACK : DARKGRAY, alphaTimer), "Modern");
}
