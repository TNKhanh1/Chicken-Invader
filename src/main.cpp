#include "../include/GameManager.h"

int main() {
    GameManager::GetInstance()->Init(1600, 900, "Chicken Invaders - OOP");
    
    // Chạy vòng lặp game chính
    GameManager::GetInstance()->Run();

    // Dọn dẹp bộ nhớ và thoát
    GameManager::DestroyInstance();
    return 0;
}