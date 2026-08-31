#include "../include/MenuManager.h"
#include "../include/ProgressManager.h"
#include "../include/FontManager.h"
#include "../include/SoundManager.h"
#include <iostream>
#include <cmath>

MenuManager::MenuManager(int sw, int sh) : screenWidth(sw), screenHeight(sh) {
    CalculateLayout();
    UpdateStageStatus();
}

void MenuManager::CalculateLayout() {
    stageButtons.clear();
    
    // Kích thước nút Stage 1-6
    float btnWidth = 180.0f;
    float btnHeight = 180.0f;
    float paddingX = 40.0f;
    float paddingY = 40.0f;
    
    // Kích thước nút Boss
    float bossWidth = 220.0f;
    float bossHeight = 250.0f;
    float bossSpacing = 80.0f; // Khoảng cách giữa Grid và Boss

    // Tính tổng chiều rộng của toàn bộ khối (Grid + Boss)
    float gridWidth = 3 * btnWidth + 2 * paddingX;
    float totalWidth = gridWidth + bossSpacing + bossWidth;

    // Tính tổng chiều cao của khối Grid
    float gridHeight = 2 * btnHeight + paddingY;

    // Tính toán StartX và StartY để căn giữa toàn bộ khối trên màn hình
    float startX = (screenWidth - totalWidth) / 2.0f;
    float startY = (screenHeight - gridHeight) / 2.0f + 50.0f; // Dịch xuống một chút để nhường chỗ cho Title
    
    int stageId = 1;
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 3; ++col) {
            StageButton btn;
            btn.id = stageId;
            btn.text = "STAGE " + std::to_string(stageId);
            btn.rect = { startX + col * (btnWidth + paddingX), 
                         startY + row * (btnHeight + paddingY), 
                         btnWidth, btnHeight };
            btn.isBossStage = false;
            btn.hoverTime = 0.0f;
            stageButtons.push_back(btn);
            stageId++;
        }
    }
    
    // Stage 7 (Boss) ở bên phải, căn giữa theo chiều dọc của khối Grid
    float bossStartX = startX + gridWidth + bossSpacing;
    float bossStartY = startY + (gridHeight - bossHeight) / 2.0f; 
    
    StageButton bossBtn;
    bossBtn.id = 7;
    bossBtn.text = "FINAL STAGE";
    bossBtn.rect = { bossStartX, bossStartY, bossWidth, bossHeight };
    bossBtn.isBossStage = true;
    bossBtn.hoverTime = 0.0f;
    stageButtons.push_back(bossBtn);
    
    // Nút Shop và Settings ở góc trên phải
    float topBtnWidth = 120.0f;
    float topBtnHeight = 50.0f;
    shopButton = { (float)screenWidth - 300.0f, 40.0f, topBtnWidth, topBtnHeight };
    settingsButton = { (float)screenWidth - 160.0f, 40.0f, topBtnWidth, topBtnHeight };
}

void MenuManager::UpdateStageStatus() {
    int highestUnlocked = ProgressManager::GetInstance()->GetHighestUnlockedStage();
    for (auto& btn : stageButtons) {
        btn.isLocked = (btn.id > highestUnlocked);
        btn.isCleared = (btn.id < highestUnlocked); // Nếu đã qua thì sáng lên
    }
}

void MenuManager::DrawGlassmorphicPanel(Rectangle rect, bool isHovered, bool isLocked, bool isBoss, float hoverTime) {
    // Độ bo góc
    float roundness = 0.15f;
    int segments = 16;
    
    if (isLocked) {
        // Trạng thái khóa: Màu xám/đen mờ
        DrawRectangleRounded(rect, roundness, segments, { 30, 30, 30, 180 });
        DrawRectangleRoundedLinesEx(rect, roundness, segments, 2.0f, DARKGRAY);
    } else {
        // Trạng thái mở khóa: Màu xanh/tím Glassmorphism
        Color bgColor = isBoss ? (Color){ 80, 20, 20, 180 } : (Color){ 20, 40, 80, 180 };
        Color borderColor = isBoss ? RED : SKYBLUE;
        
        if (isHovered) {
            bgColor = isBoss ? (Color){ 120, 30, 30, 200 } : (Color){ 40, 70, 120, 200 };
            borderColor = isBoss ? ORANGE : GREEN;
            // Hiệu ứng phình to nhẹ khi hover
            rect.x -= 2.0f; rect.y -= 2.0f;
            rect.width += 4.0f; rect.height += 4.0f;
        }
        
        // Thêm nhấp nháy cho Boss Stage
        if (isBoss) {
            float pulse = (sin(GetTime() * 4.0f) + 1.0f) / 2.0f; // 0.0 to 1.0
            borderColor = { (unsigned char)(RED.r + pulse * (YELLOW.r - RED.r)),
                            (unsigned char)(RED.g + pulse * (YELLOW.g - RED.g)),
                            (unsigned char)(RED.b + pulse * (YELLOW.b - RED.b)), 255 };
        }
        
        // Đổ bóng (Shadow)
        Rectangle shadowRect = { rect.x + 5, rect.y + 5, rect.width, rect.height };
        DrawRectangleRounded(shadowRect, roundness, segments, { 0, 0, 0, 100 });
        
        // Vẽ nền Glass
        DrawRectangleRounded(rect, roundness, segments, bgColor);
        // Vẽ viền
        DrawRectangleRoundedLinesEx(rect, roundness, segments, isHovered ? 4.0f : 2.0f, borderColor);
    }
}

int MenuManager::UpdateAndDraw(float deltaTime) {
    int clickedId = -1;
    Vector2 mousePos = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    
    // Tiêu đề
    const char* title = "CHICKEN INVADERS : GALAXY DEFENDER";
    FontManager::GetInstance()->DrawGameTextCentered(title, screenWidth/2, 100, 50, GOLD, "Retro");
    
    // Cập nhật và Vẽ các Stage Button
    for (auto& btn : stageButtons) {
        bool isHovered = CheckCollisionPointRec(mousePos, btn.rect);
        
        if (isHovered && !btn.isLocked) {
            btn.hoverTime += deltaTime;
            if (mousePressed) {
                clickedId = btn.id; // Chọn stage
            }
        } else {
            btn.hoverTime -= deltaTime;
            if (btn.hoverTime < 0.0f) btn.hoverTime = 0.0f;
        }
        
        DrawGlassmorphicPanel(btn.rect, isHovered && !btn.isLocked, btn.isLocked, btn.isBossStage, btn.hoverTime);
        
        // Vẽ Text bên trong
        Color textColor = btn.isLocked ? DARKGRAY : (btn.isCleared ? GREEN : WHITE);
        if (btn.isBossStage && !btn.isLocked) textColor = ORANGE;
        
        FontManager::GetInstance()->DrawGameTextCentered(btn.text.c_str(), btn.rect.x + btn.rect.width/2, btn.rect.y + btn.rect.height/2 - 12, 24, textColor, "Modern");
        
        if (btn.isLocked) {
            FontManager::GetInstance()->DrawGameTextCentered("LOCKED", btn.rect.x + btn.rect.width/2, btn.rect.y + btn.rect.height/2 + 20, 16, GRAY, "Modern");
        } else if (btn.isCleared) {
            FontManager::GetInstance()->DrawGameTextCentered("CLEARED", btn.rect.x + btn.rect.width/2, btn.rect.y + btn.rect.height/2 + 20, 16, LIME, "Modern");
        }
    }
    
    // Xử lý nút Shop
    bool shopHovered = CheckCollisionPointRec(mousePos, shopButton);
    DrawRectangleRounded(shopButton, 0.2f, 8, shopHovered ? (Color){60, 60, 60, 200} : (Color){40, 40, 40, 180});
    DrawRectangleRoundedLinesEx(shopButton, 0.2f, 8, 2.0f, shopHovered ? WHITE : GRAY);
    FontManager::GetInstance()->DrawGameTextCentered("SHOP", shopButton.x + shopButton.width/2, shopButton.y + 15, 20, WHITE, "Modern");
    if (shopHovered && mousePressed) clickedId = 100;
    
    // Xử lý nút Settings
    bool setHovered = CheckCollisionPointRec(mousePos, settingsButton);
    DrawRectangleRounded(settingsButton, 0.2f, 8, setHovered ? (Color){60, 60, 60, 200} : (Color){40, 40, 40, 180});
    DrawRectangleRoundedLinesEx(settingsButton, 0.2f, 8, 2.0f, setHovered ? WHITE : GRAY);
    FontManager::GetInstance()->DrawGameTextCentered("SETTINGS", settingsButton.x + settingsButton.width/2, settingsButton.y + 15, 20, WHITE, "Modern");
    if (setHovered && mousePressed) clickedId = 101;
    
    if (clickedId != -1) {
        SoundManager::GetInstance()->PlayBeep();
    }

    return clickedId;
}
