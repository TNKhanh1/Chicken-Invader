#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include "raylib.h"
#include <vector>
#include <string>

// Struct đại diện cho một nút bấm Stage
struct StageButton {
    int id;
    Rectangle rect;
    std::string text;
    bool isLocked;
    bool isCleared;
    bool isBossStage;
    
    // Animation properties
    float hoverTime;
};

class MenuManager {
private:
    std::vector<StageButton> stageButtons;
    Rectangle shopButton;
    Rectangle settingsButton;
    
    int screenWidth;
    int screenHeight;
    
    // Helper methods
    void CalculateLayout();
    void DrawGlassmorphicPanel(Rectangle rect, bool isHovered, bool isLocked, bool isBoss, float hoverTime);

public:
    MenuManager(int sw, int sh);
    ~MenuManager() = default;

    // Trả về -1 nếu không click, trả về ID của stage nếu được click (1-7),
    // trả về 100 cho SHOP, 101 cho SETTINGS
    int UpdateAndDraw(float deltaTime);
    
    void UpdateStageStatus(); // Cập nhật lại trạng thái lock/clear từ ProgressManager
};

#endif // MENUMANAGER_H
