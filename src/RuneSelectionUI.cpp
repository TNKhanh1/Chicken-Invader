#include "RuneSelectionUI.h"
#include "RuneManager.h"
#include "CoinManager.h"
#include "FontManager.h"
#include <string>

RuneSelectionUI::RuneSelectionUI(int w, int h) : screenW(w), screenH(h) {}

void RuneSelectionUI::Update(float deltaTime) {
    animTimer += deltaTime;

    float panelW = 900;
    float panelH = 600;
    float startX = (screenW - panelW) / 2.0f;
    float startY = (screenH - panelH) / 2.0f;

    float rowH = 100;
    float rowStartY = startY + 80;

    Vector2 mousePos = GetMousePosition();
    
    // Check click on Rune Rows
    for (int i = 0; i < RuneManager::GetInstance()->GetRuneCount(); ++i) {
        float rowY = rowStartY + i * (rowH + 10);
        Rectangle rowRect = { startX + 20, rowY, panelW - 40, rowH };
        if (CheckCollisionPointRec(mousePos, rowRect)) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Try to upgrade
                RuneManager::GetInstance()->UpgradeRune(i);
            }
        }
    }

    // Check click on RESET button
    Rectangle resetBtn = { startX + 20, startY + panelH - 60, 160, 40 };
    if (CheckCollisionPointRec(mousePos, resetBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        RuneManager::GetInstance()->ResetAllAndRefund();
    }

    // Check click on START button
    Rectangle startBtn = { startX + panelW - 220, startY + panelH - 70, 200, 50 };
    if (CheckCollisionPointRec(mousePos, startBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        startRequested = true;
    }
}

void RuneSelectionUI::DrawPanel(float x, float y, float w, float h, Color bg, Color border) {
    DrawRectangleRounded({x, y, w, h}, 0.1f, 10, bg);
    DrawRectangleRoundedLinesEx({x, y, w, h}, 0.1f, 10, 4, border);
}

void RuneSelectionUI::DrawDiamond(Vector2 center, float halfSize, Color fill, Color outline) {
    Vector2 top = { center.x, center.y - halfSize };
    Vector2 bottom = { center.x, center.y + halfSize };
    Vector2 left = { center.x - halfSize, center.y };
    Vector2 right = { center.x + halfSize, center.y };

    DrawTriangle(top, left, bottom, fill);
    DrawTriangle(top, bottom, right, fill);
    
    DrawLineEx(top, left, 3, outline);
    DrawLineEx(left, bottom, 3, outline);
    DrawLineEx(bottom, right, 3, outline);
    DrawLineEx(right, top, 3, outline);
}

void RuneSelectionUI::DrawLevelBadge(int level, bool isMax, float cx, float cy, float size) {
    Color bg = GRAY;
    if (isMax) bg = GOLD;
    else if (level == 1) bg = GREEN;
    else if (level == 2) bg = PURPLE;
    else if (level == 3) bg = ORANGE; // Fallback, usually 3 is max and gold

    DrawRectangleRounded({cx - size/2, cy - size/2, size, size}, 0.2f, 10, bg);
    DrawRectangleRoundedLinesEx({cx - size/2, cy - size/2, size, size}, 0.2f, 10, 2, DARKGRAY);

    std::string text = isMax ? "MAX" : std::to_string(level);
    int fontSize = isMax ? 20 : 30;
    FontManager::GetInstance()->DrawGameTextCentered(text.c_str(), cx, cy - fontSize/2, fontSize, WHITE, "Modern");
}

void RuneSelectionUI::DrawRuneRow(int idx, float rowY, float rowH, bool isHovered) {
    IRune* rune = RuneManager::GetInstance()->GetRune(idx);
    if (!rune) return;

    float startX = (screenW - 900) / 2.0f + 20;
    float rowW = 900 - 40;

    // Row Background
    Color rowBg = isHovered ? ColorAlpha(BROWN, 0.4f) : ColorAlpha(BROWN, 0.2f);
    DrawRectangleRounded({startX, rowY, rowW, rowH}, 0.1f, 10, rowBg);
    DrawRectangleRoundedLinesEx({startX, rowY, rowW, rowH}, 0.1f, 10, 2, DARKGRAY);

    // Diamond
    DrawDiamond({startX + 50, rowY + rowH/2}, 30, rune->GetColor(), DARKGRAY);

    // Text info
    FontManager::GetInstance()->DrawGameText(rune->GetName().c_str(), startX + 110, rowY + 15, 24, WHITE, "Modern");
    
    std::string desc = rune->GetDescription();
    size_t pos = desc.find('\n');
    if (pos != std::string::npos) {
        FontManager::GetInstance()->DrawGameText(desc.substr(0, pos).c_str(), startX + 110, rowY + 45, 18, LIGHTGRAY, "Modern");
        FontManager::GetInstance()->DrawGameText(desc.substr(pos + 1).c_str(), startX + 110, rowY + 70, 18, ORANGE, "Modern");
    }

    // Badge Level
    DrawLevelBadge(rune->GetLevel(), rune->IsMaxLevel(), startX + rowW - 200, rowY + rowH/2, 60);

    // Cost Button
    float costX = startX + rowW - 120;
    float costY = rowY + rowH/2 - 25;
    DrawRectangleRounded({costX, costY, 100, 50}, 0.2f, 10, DARKBROWN);
    DrawRectangleRoundedLinesEx({costX, costY, 100, 50}, 0.2f, 10, 2, DARKGRAY);

    if (rune->IsMaxLevel()) {
        FontManager::GetInstance()->DrawGameTextCentered("MAX", costX + 50, costY + 15, 20, GOLD, "Modern");
    } else {
        DrawCircle(costX + 25, costY + 25, 10, GOLD); // Coin Icon
        FontManager::GetInstance()->DrawGameText(std::to_string(rune->GetCostToUpgrade()).c_str(), costX + 45, costY + 15, 20, WHITE, "Modern");
    }
}

void RuneSelectionUI::Draw() {
    // Semi-transparent background
    DrawRectangle(0, 0, screenW, screenH, ColorAlpha(BLACK, 0.7f));

    float panelW = 900;
    float panelH = 600;
    float startX = (screenW - panelW) / 2.0f;
    float startY = (screenH - panelH) / 2.0f;

    // Main Panel
    DrawPanel(startX, startY, panelW, panelH, DARKBROWN, GOLD);
    DrawPanel(startX + 10, startY + 10, panelW - 20, panelH - 20, ColorAlpha(BROWN, 0.5f), BLANK);

    // Title
    FontManager::GetInstance()->DrawGameText("RUNES", startX + 40, startY + 25, 40, GOLD, "Modern");

    // Total Coins
    std::string coinStr = std::to_string(CoinManager::GetInstance()->GetTotalCoins());
    int coinW = FontManager::GetInstance()->MeasureGameText(coinStr.c_str(), 30, "Modern").x;
    DrawCircle(startX + panelW - coinW - 50, startY + 45, 15, GOLD);
    FontManager::GetInstance()->DrawGameText(coinStr.c_str(), startX + panelW - coinW - 20, startY + 30, 30, WHITE, "Modern");

    // Draw Rows
    float rowH = 100;
    float rowStartY = startY + 80;
    Vector2 mousePos = GetMousePosition();

    for (int i = 0; i < RuneManager::GetInstance()->GetRuneCount(); ++i) {
        float rowY = rowStartY + i * (rowH + 10);
        Rectangle rowRect = { startX + 20, rowY, panelW - 40, rowH };
        bool isHovered = CheckCollisionPointRec(mousePos, rowRect);
        DrawRuneRow(i, rowY, rowH, isHovered);
    }

    // RESET Button
    Rectangle resetBtn = { startX + 20, startY + panelH - 60, 160, 40 };
    bool resetHover = CheckCollisionPointRec(mousePos, resetBtn);
    DrawRectangleRounded(resetBtn, 0.2f, 10, resetHover ? RED : DARKGRAY);
    DrawRectangleRoundedLinesEx(resetBtn, 0.2f, 10, 2, WHITE);
    FontManager::GetInstance()->DrawGameTextCentered("RESET", startX + 100, startY + panelH - 50, 20, WHITE, "Modern");

    // START Button
    Rectangle startBtn = { startX + panelW - 220, startY + panelH - 70, 200, 50 };
    bool startHover = CheckCollisionPointRec(mousePos, startBtn);
    DrawRectangleRounded(startBtn, 0.2f, 10, startHover ? GOLD : DARKBROWN);
    DrawRectangleRoundedLinesEx(startBtn, 0.2f, 10, 3, WHITE);
    FontManager::GetInstance()->DrawGameTextCentered("START !", startX + panelW - 120, startY + panelH - 65, 24, startHover ? BLACK : WHITE, "Modern");
    FontManager::GetInstance()->DrawGameTextCentered("Press Space", startX + panelW - 120, startY + panelH - 40, 12, startHover ? BLACK : LIGHTGRAY, "Modern");
}
