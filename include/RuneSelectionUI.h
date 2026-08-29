#ifndef RUNE_SELECTION_UI_H
#define RUNE_SELECTION_UI_H

#include "raylib.h"

class RuneSelectionUI {
private:
    int   screenW, screenH;
    bool  startRequested = false;
    float animTimer      = 0.0f; // Cho animation hiện ra

    // Helpers vẽ UI
    void DrawPanel(float x, float y, float w, float h, Color bg, Color border);
    void DrawDiamond(Vector2 center, float halfSize, Color fill, Color outline);
    void DrawLevelBadge(int level, bool isMax, float cx, float cy, float size);
    void DrawRuneRow(int idx, float rowY, float rowH, bool isHovered);

public:
    RuneSelectionUI(int w, int h);

    void Update(float deltaTime);
    void Draw();

    bool IsStartRequested() const { return startRequested; }
    void ResetStartFlag() { startRequested = false; }
};

#endif // RUNE_SELECTION_UI_H
