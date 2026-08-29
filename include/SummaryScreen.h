#ifndef SUMMARYSCREEN_H
#define SUMMARYSCREEN_H

#include "raylib.h"

enum class SummaryResult { WIN, LOSE, QUIT };

class SummaryScreen {
public:
    SummaryScreen(int sw, int sh);
    void Show(SummaryResult result, int score, int sessionCoins, int stageBonus, int totalCoins);
    void Draw();
    void Update();
    bool IsBackRequested() const;
    void Reset();
private:
    int sw, sh;
    SummaryResult result;
    int score, sessionCoins, stageBonus, totalCoins;
    bool backRequested = false;
    void DrawInfoRow(float x, float y, const char* label, const char* value, Color color);
    
    // For visual effects
    float alphaTimer;
};

#endif // SUMMARYSCREEN_H
