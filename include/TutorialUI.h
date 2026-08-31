#ifndef TUTORIALUI_H
#define TUTORIALUI_H

#include "raylib.h"

class TutorialUI {
private:
    int screenWidth;
    int screenHeight;

    int currentPage; // 1 to 5
    const int totalPages = 5;

    // Button bounds
    Rectangle nextBtn;
    Rectangle prevBtn;
    Rectangle backBtn;

    // Private helpers
    void DrawPage1();
    void DrawPage2();
    void DrawPage3();
    void DrawPage4();
    void DrawPage5();

    void HandleInput();
    bool backRequested;

public:
    TutorialUI(int sw, int sh);
    ~TutorialUI();

    void Init();
    void Update();
    void Draw();
    void Cleanup();

    bool IsBackRequested() const;
    void Reset();
};

#endif // TUTORIALUI_H
