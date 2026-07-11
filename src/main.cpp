#include "../include/GameManager.h"

int main() {
    // Khởi tạo Game (Kích thước cửa sổ mặc định 1280x720)
    GameManager::GetInstance()->Init(1280, 720, "Chicken Invaders - OOP");

    // Chạy vòng lặp game chính
    GameManager::GetInstance()->Run();

    // Dọn dẹp bộ nhớ và thoát
    GameManager::GetInstance()->CleanUp();
    GameManager::DestroyInstance();

    return 0;
}