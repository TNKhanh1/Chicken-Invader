#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "Observer.h"
#include "raylib.h"
#include <iostream>

// Hệ thống quản lý UI (Observer lắng nghe các sự kiện từ Model)
class UIManager : public IObserver {
private:
    float playerHp;
    float playerMaxHp;
    float playerMana;
    float playerMaxMana;
    int playerLevel;
    float playerExp;

public:
    UIManager();

    // Kế thừa hàm OnNotify từ IObserver
    void OnNotify(EventType event, const std::string& data) override;

    // Hàm gọi trong GameManager để vẽ UI lên màn hình
    void DrawUI();
};

#endif // UIMANAGER_H
