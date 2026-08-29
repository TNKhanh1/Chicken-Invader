#include "../include/MenuManager.h"
#include "../include/ProgressManager.h"
#include <iostream>
#include <cmath>

MenuManager::MenuManager(int sw, int sh) : screenWidth(sw), screenHeight(sh) {
    CalculateLayout();
    UpdateStageStatus();
}

void MenuManager::CalculateLayout() {
    stageButtons.clear();
    
    // Khởi tạo kích thước cho Stage 1-6 (Grid 3x2)
    float btnWidth = 180.0f;
    float btnHeight = 180.0f;
    float paddingX = 40.0f;
    float paddingY = 40.0f;
    
    // Tọa độ bắt đầu của khối Grid bên trái
    float startX = 150.0f;
    float startY = 300.0f;
    
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
    
    // Stage 7 (Boss) ở bên phải, to hơn một chút
    float bossStartX = startX + 3 * (btnWidth + paddingX) + 60.0f; // Dịch sang phải, cách Grid 1 khoảng
    float bossStartY = startY + (btnHeight + paddingY) / 2.0f - 125.0f; // Căn giữa theo chiều dọc
    
    StageButton bossBtn;
    bossBtn.id = 7;
    bossBtn.text = "BOSS STAGE";
    bossBtn.rect = { bossStartX, bossStartY, 220.0f, 250.0f };
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
    int titleW = MeasureText(title, 50); // Cần có font custom nếu muốn đẹp, tạm dùng MeasureText của Raylib
    DrawText(title, screenWidth/2 - titleW/2, 100, 50, GOLD);
    
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
        
        int textW = MeasureText(btn.text.c_str(), 24);
        DrawText(btn.text.c_str(), btn.rect.x + btn.rect.width/2 - textW/2, btn.rect.y + btn.rect.height/2 - 12, 24, textColor);
        
        if (btn.isLocked) {
            int lockW = MeasureText("LOCKED", 16);
            DrawText("LOCKED", btn.rect.x + btn.rect.width/2 - lockW/2, btn.rect.y + btn.rect.height/2 + 20, 16, GRAY);
        } else if (btn.isCleared) {
            int clrW = MeasureText("CLEARED", 16);
            DrawText("CLEARED", btn.rect.x + btn.rect.width/2 - clrW/2, btn.rect.y + btn.rect.height/2 + 20, 16, LIME);
        }
    }
    
    // Xử lý nút Shop
    bool shopHovered = CheckCollisionPointRec(mousePos, shopButton);
    DrawRectangleRounded(shopButton, 0.2f, 8, shopHovered ? (Color){60, 60, 60, 200} : (Color){40, 40, 40, 180});
    DrawRectangleRoundedLinesEx(shopButton, 0.2f, 8, 2.0f, shopHovered ? WHITE : GRAY);
    int shopW = MeasureText("SHOP", 20);
    DrawText("SHOP", shopButton.x + shopButton.width/2 - shopW/2, shopButton.y + 15, 20, WHITE);
    if (shopHovered && mousePressed) clickedId = 100;
    
    // Xử lý nút Settings
    bool setHovered = CheckCollisionPointRec(mousePos, settingsButton);
    DrawRectangleRounded(settingsButton, 0.2f, 8, setHovered ? (Color){60, 60, 60, 200} : (Color){40, 40, 40, 180});
    DrawRectangleRoundedLinesEx(settingsButton, 0.2f, 8, 2.0f, setHovered ? WHITE : GRAY);
    int setW = MeasureText("SETTINGS", 20);
    DrawText("SETTINGS", settingsButton.x + settingsButton.width/2 - setW/2, settingsButton.y + 15, 20, WHITE);
    if (setHovered && mousePressed) clickedId = 101;
    
    return clickedId;
}
