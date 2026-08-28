#pragma once
#include "raylib.h"
#include <string>

class Spaceship; // forward declaration

class StatsPanel {
private:
    float slideX;
    
    // Layout constants
    static constexpr float PANEL_W      = 340.0f;
    static constexpr float PANEL_X_SHOW =  10.0f;
    static constexpr float PANEL_X_HIDE = -PANEL_W - 5.0f;
    static constexpr float LERP_IN_SPD  =  12.0f;
    static constexpr float LERP_OUT_SPD =  14.0f;

    void DrawRow(float x, float& y, const char* label, const char* value, Color valueColor) const;

public:
    StatsPanel();
    void Update(float deltaTime, bool tabHeld);
    void Draw(const Spaceship* player, int screenHeight) const;
};
