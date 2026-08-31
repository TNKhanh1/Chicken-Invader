#include "TutorialUI.h"
#include "FontManager.h"
#include "SoundManager.h"

TutorialUI::TutorialUI(int sw, int sh) : screenWidth(sw), screenHeight(sh), currentPage(1), backRequested(false) {
    // Initialize button bounds
    float panelW = 800;
    float panelH = 550;
    float startX = (screenWidth - panelW) / 2.0f;
    float startY = (screenHeight - panelH) / 2.0f;

    prevBtn = { startX + 50, startY + panelH - 70, 150, 40 };
    nextBtn = { startX + panelW - 200, startY + panelH - 70, 150, 40 };
    backBtn = { startX + (panelW / 2) - 100, startY + panelH - 70, 200, 40 };
}

TutorialUI::~TutorialUI() {}

void TutorialUI::Init() {
    Reset();
}

void TutorialUI::Cleanup() {}

void TutorialUI::Reset() {
    currentPage = 1;
    backRequested = false;
}

bool TutorialUI::IsBackRequested() const {
    return backRequested;
}

void TutorialUI::Update() {
    HandleInput();
}

void TutorialUI::HandleInput() {
    Vector2 mousePos = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    // Prev Button
    if (currentPage > 1) {
        if (CheckCollisionPointRec(mousePos, prevBtn) && mousePressed) {
            SoundManager::GetInstance()->PlayBeep();
            currentPage--;
        }
    }

    // Next Button
    if (currentPage < totalPages) {
        if (CheckCollisionPointRec(mousePos, nextBtn) && mousePressed) {
            SoundManager::GetInstance()->PlayBeep();
            currentPage++;
        }
    }

    // Back Button (Close)
    if (CheckCollisionPointRec(mousePos, backBtn) && mousePressed) {
        SoundManager::GetInstance()->PlayBeep();
        backRequested = true;
    }
}

void TutorialUI::Draw() {
    // Dim background
    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.8f));

    float panelW = 800;
    float panelH = 550;
    float startX = (screenWidth - panelW) / 2.0f;
    float startY = (screenHeight - panelH) / 2.0f;

    // Draw Panel
    DrawRectangleRounded({startX, startY, panelW, panelH}, 0.1f, 10, ColorAlpha(DARKGRAY, 0.95f));
    DrawRectangleRoundedLinesEx({startX, startY, panelW, panelH}, 0.1f, 10, 4, GRAY);

    // Draw Title
    FontManager::GetInstance()->DrawGameTextCentered("TUTORIAL", screenWidth / 2, startY + 30, 40, ORANGE, "Retro");
    
    // Draw Page Content
    switch (currentPage) {
        case 1: DrawPage1(); break;
        case 2: DrawPage2(); break;
        case 3: DrawPage3(); break;
        case 4: DrawPage4(); break;
        case 5: DrawPage5(); break;
    }

    // Draw Buttons
    Vector2 mousePos = GetMousePosition();

    // Prev Button
    if (currentPage > 1) {
        bool hover = CheckCollisionPointRec(mousePos, prevBtn);
        DrawRectangleRounded(prevBtn, 0.2f, 8, hover ? LIGHTGRAY : GRAY);
        FontManager::GetInstance()->DrawGameTextCentered("< PREV", prevBtn.x + prevBtn.width/2, prevBtn.y + 10, 20, BLACK, "Modern");
    }

    // Next Button
    if (currentPage < totalPages) {
        bool hover = CheckCollisionPointRec(mousePos, nextBtn);
        DrawRectangleRounded(nextBtn, 0.2f, 8, hover ? LIGHTGRAY : GRAY);
        FontManager::GetInstance()->DrawGameTextCentered("NEXT >", nextBtn.x + nextBtn.width/2, nextBtn.y + 10, 20, BLACK, "Modern");
    }

    // Back Button
    bool hover = CheckCollisionPointRec(mousePos, backBtn);
    DrawRectangleRounded(backBtn, 0.2f, 8, hover ? MAROON : RED);
    FontManager::GetInstance()->DrawGameTextCentered("BACK TO MENU", backBtn.x + backBtn.width/2, backBtn.y + 10, 20, WHITE, "Modern");

    // Page indicator
    FontManager::GetInstance()->DrawGameTextCentered(TextFormat("Page %d / %d", currentPage, totalPages), screenWidth / 2, startY + panelH - 120, 20, LIGHTGRAY, "Modern");
}

void TutorialUI::DrawPage1() {
    float startY = (screenHeight - 550) / 2.0f + 100;
    FontManager::GetInstance()->DrawGameTextCentered("- CONTROLS -", screenWidth / 2, startY, 30, WHITE, "Modern");
    
    float col1 = screenWidth / 2 - 200;
    float col2 = screenWidth / 2 + 200;
    
    FontManager::GetInstance()->DrawGameTextCentered("[ PLAYER 1 (KEYBOARD) ]", col1, startY + 60, 24, SKYBLUE, "Modern");
    FontManager::GetInstance()->DrawGameTextCentered("Move: W, A, S, D or Arrows", col1, startY + 110, 20, LIGHTGRAY, "Modern");
    FontManager::GetInstance()->DrawGameTextCentered("Shoot: SPACE", col1, startY + 150, 20, LIGHTGRAY, "Modern");
    FontManager::GetInstance()->DrawGameTextCentered("Skill (Ultimate): M", col1, startY + 190, 20, LIGHTGRAY, "Modern");

    FontManager::GetInstance()->DrawGameTextCentered("[ PLAYER 2 (MOUSE) ]", col2, startY + 60, 24, PINK, "Modern");
    FontManager::GetInstance()->DrawGameTextCentered("Move: Mouse Cursor", col2, startY + 110, 20, LIGHTGRAY, "Modern");
    FontManager::GetInstance()->DrawGameTextCentered("Shoot: Left Click", col2, startY + 150, 20, LIGHTGRAY, "Modern");
    FontManager::GetInstance()->DrawGameTextCentered("Skill (Ultimate): Right Click", col2, startY + 190, 20, LIGHTGRAY, "Modern");
}

void TutorialUI::DrawPage2() {
    float startY = (screenHeight - 550) / 2.0f + 100;
    FontManager::GetInstance()->DrawGameTextCentered("- SPACESHIP STATS -", screenWidth / 2, startY, 30, WHITE, "Modern");
    
    FontManager::GetInstance()->DrawGameTextCentered("Press [TAB] during gameplay to show/hide the stats panel.", screenWidth / 2, startY + 60, 20, LIGHTGRAY, "Modern");
    FontManager::GetInstance()->DrawGameTextCentered("The stats panel displays real-time information:", screenWidth / 2, startY + 110, 20, LIGHTGRAY, "Modern");
    
    FontManager::GetInstance()->DrawGameText("- HP, Damage, Armor, Fire Rate", screenWidth / 2 - 150, startY + 150, 20, YELLOW, "Modern");
    FontManager::GetInstance()->DrawGameText("- Crit Chance, Crit Damage, Mana", screenWidth / 2 - 150, startY + 190, 20, YELLOW, "Modern");
    FontManager::GetInstance()->DrawGameText("- Level, EXP", screenWidth / 2 - 150, startY + 230, 20, YELLOW, "Modern");
    
    FontManager::GetInstance()->DrawGameTextCentered("Pick up meats dropped by enemies to level up your ship!", screenWidth / 2, startY + 280, 20, LIME, "Modern");
}

void TutorialUI::DrawPage3() {
    float startY = (screenHeight - 550) / 2.0f + 100;
    FontManager::GetInstance()->DrawGameTextCentered("- EARNING COINS -", screenWidth / 2, startY, 30, WHITE, "Modern");
    
    FontManager::GetInstance()->DrawGameTextCentered("Coins are the main currency of the game.", screenWidth / 2, startY + 60, 20, LIGHTGRAY, "Modern");
    
    FontManager::GetInstance()->DrawGameText("1. Kill enemies to earn coins instantly.", screenWidth / 2 - 200, startY + 110, 20, YELLOW, "Modern");
    FontManager::GetInstance()->DrawGameText("2. Clear stages to receive a huge coin bonus.", screenWidth / 2 - 200, startY + 150, 20, YELLOW, "Modern");
    
    FontManager::GetInstance()->DrawGameTextCentered("If your ship is destroyed, you keep the coins you earned.", screenWidth / 2, startY + 220, 20, LIGHTGRAY, "Modern");
    FontManager::GetInstance()->DrawGameTextCentered("Save up to buy powerful items in the Shop!", screenWidth / 2, startY + 260, 20, LIME, "Modern");
}

void TutorialUI::DrawPage4() {
    float startY = (screenHeight - 550) / 2.0f + 100;
    FontManager::GetInstance()->DrawGameTextCentered("- THE SHOP -", screenWidth / 2, startY, 30, WHITE, "Modern");
    
    FontManager::GetInstance()->DrawGameTextCentered("Access the Shop from the Main Menu.", screenWidth / 2, startY + 60, 20, LIGHTGRAY, "Modern");
    
    FontManager::GetInstance()->DrawGameText("- WEAPONS: Buy new guns with different firing patterns.", screenWidth / 2 - 250, startY + 120, 20, YELLOW, "Modern");
    FontManager::GetInstance()->DrawGameText("- EGG SKINS: Customize the appearance of enemy bullets.", screenWidth / 2 - 250, startY + 170, 20, YELLOW, "Modern");
    
    FontManager::GetInstance()->DrawGameTextCentered("Once purchased, items can be equipped or unequipped anytime.", screenWidth / 2, startY + 240, 20, LIME, "Modern");
}

void TutorialUI::DrawPage5() {
    float startY = (screenHeight - 550) / 2.0f + 100;
    FontManager::GetInstance()->DrawGameTextCentered("- ANCIENT RUNES -", screenWidth / 2, startY, 30, WHITE, "Modern");
    
    FontManager::GetInstance()->DrawGameTextCentered("Runes grant powerful passive abilities.", screenWidth / 2, startY + 60, 20, LIGHTGRAY, "Modern");
    
    FontManager::GetInstance()->DrawGameText("Before the game starts, you can select up to 3 Runes:", screenWidth / 2 - 250, startY + 120, 20, YELLOW, "Modern");
    
    FontManager::GetInstance()->DrawGameText("- Rune of Ares: Enhances attack and critical damage.", screenWidth / 2 - 230, startY + 160, 20, RED, "Modern");
    FontManager::GetInstance()->DrawGameText("- Rune of Gaia: Grants regeneration and max HP.", screenWidth / 2 - 230, startY + 200, 20, GREEN, "Modern");
    FontManager::GetInstance()->DrawGameText("- Rune of Helios: Increases attack speed and mobility.", screenWidth / 2 - 230, startY + 240, 20, GOLD, "Modern");
    FontManager::GetInstance()->DrawGameText("- Rune of Hades: Absorbs life and manipulates darkness.", screenWidth / 2 - 230, startY + 280, 20, PURPLE, "Modern");
}
