#include "../include/GameManager.h"

int main() {
    // Khởi tạo Game (Kích thước cửa sổ 1600x900)
    GameManager::GetInstance()->Init(1600, 900, "Chicken Invaders - OOP");

    // Chạy vòng lặp game chính
    GameManager::GetInstance()->Run();

    // Dọn dẹp bộ nhớ và thoát
    GameManager::GetInstance()->CleanUp();
    GameManager::DestroyInstance();

    return 0;
}