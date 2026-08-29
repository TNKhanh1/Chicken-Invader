#include "TitleScreen.h"
#include "FontManager.h"
#include <cmath>

TitleScreen::TitleScreen(int sw, int sh)
    : sw(sw), sh(sh), titleScale(1.0f), alphaTimer(0.0f) {}

TitleAction TitleScreen::UpdateAndDraw(float deltaTime) {
    // Fade-in effect
    alphaTimer += deltaTime * 1.5f;
    if (alphaTimer > 1.0f) alphaTimer = 1.0f;
    // unsigned char alpha = (unsigned char)(255 * alphaTimer); // Unused variable removed

    // Hiệu ứng title scale nẩy nhẹ
    titleScale = 1.0f + 0.015f * sinf(GetTime() * 2.0f);

    TitleAction action = TitleAction::NONE;

    DrawTitle();
    DrawButtons(action);

    return action;
}

void TitleScreen::DrawTitle() {
    // Vẽ "CHICKEN INVADER" căn giữa màn hình phía trên
    const char* title = "CHICKEN INVADER";
    int fontSize = (int)(80 * titleScale);
    FontManager::GetInstance()->DrawGameTextCentered(
        title,
        sw / 2, sh / 2 - 120,
        fontSize,
        YELLOW, 
        "Retro"
    );
}

void TitleScreen::DrawButtons(TitleAction& outAction) {
    float btnW = 280, btnH = 60;
    float btnX = sw / 2.0f - btnW / 2.0f;
    float continueY = (float)sh / 2 + 20;
    float newGameY = (float)sh / 2 + 100;

    // Nút CONTINUE
    Rectangle contBtn = { btnX, continueY, btnW, btnH };
    bool contHover = CheckCollisionPointRec(GetMousePosition(), contBtn);
    DrawRectangleRounded(contBtn, 0.3f, 8, contHover ? GOLD : DARKGRAY);
    DrawRectangleRoundedLinesEx(contBtn, 0.3f, 8, 2, contHover ? WHITE : GRAY);
    FontManager::GetInstance()->DrawGameTextCentered(
        "CONTINUE", (int)(btnX + btnW/2), (int)(continueY + 15), 28,
        contHover ? BLACK : WHITE, "Modern"
    );
    if (contHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        outAction = TitleAction::CONTINUE;

    // Nút NEW GAME
    Rectangle ngBtn = { btnX, newGameY, btnW, btnH };
    bool ngHover = CheckCollisionPointRec(GetMousePosition(), ngBtn);
    DrawRectangleRounded(ngBtn, 0.3f, 8, ngHover ? RED : DARKGRAY);
    DrawRectangleRoundedLinesEx(ngBtn, 0.3f, 8, 2, ngHover ? WHITE : GRAY);
    FontManager::GetInstance()->DrawGameTextCentered(
        "NEW GAME", (int)(btnX + btnW/2), (int)(newGameY + 15), 28,
        ngHover ? BLACK : WHITE, "Modern"
    );
    if (ngHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        outAction = TitleAction::NEW_GAME;
}

void TitleScreen::Reset() {
    alphaTimer = 0.0f;
    titleScale = 1.0f;
}
