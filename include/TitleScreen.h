#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include "raylib.h"

enum class TitleAction {
    NONE,
    CONTINUE,
    NEW_GAME
};

class TitleScreen {
public:
    TitleScreen(int sw, int sh);
    TitleAction UpdateAndDraw(float deltaTime);
    void Reset();

private:
    int sw, sh;
    float titleScale;     // Hiệu ứng scale nẩy cho title
    float alphaTimer;     // Fade-in khi lần đầu hiện

    // Tọa độ nút, tính từ sw/sh (không hardcode)
    void DrawTitle();
    void DrawButtons(TitleAction& outAction);
};

#endif // TITLESCREEN_H
