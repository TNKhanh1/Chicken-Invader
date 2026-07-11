#include "../include/GameManager.h"
#include <iostream>

// Khởi tạo instance của Singleton bằng nullptr
GameManager* GameManager::instance = nullptr;

GameManager::GameManager() 
    : currentState(GameState::MAIN_MENU), screenWidth(1280), screenHeight(720), isRunning(false) {
}

GameManager* GameManager::GetInstance() {
    if (instance == nullptr) {
        instance = new GameManager();
    }
    return instance;
}

void GameManager::DestroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

void GameManager::Init(int width, int height, const char* title) {
    screenWidth = width;
    screenHeight = height;
    
    // Khởi tạo cửa sổ Raylib
    InitWindow(screenWidth, screenHeight, title);
    SetTargetFPS(60); // Đặt tốc độ khung hình 60 FPS
    
    isRunning = true;
    currentState = GameState::PLAYING; // Tạm set thành PLAYING để test nhanh
}

void GameManager::Run() {
    // Vòng lặp game chính
    while (isRunning && !WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        
        Update(deltaTime);
        Draw();
    }
}

void GameManager::Update(float deltaTime) {
    // Xử lý logic game dựa trên trạng thái hiện tại
    switch (currentState) {
        case GameState::MAIN_MENU:
            // Xử lý menu
            break;
        case GameState::PLAYING:
            // Cập nhật người chơi, quái vật, đạn...
            break;
        case GameState::PAUSED:
            // Tạm dừng
            break;
        case GameState::GAME_OVER:
            // Kết thúc game
            break;
        default:
            break;
    }
}

void GameManager::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE); // Background màu trắng tạm thời

    switch (currentState) {
        case GameState::MAIN_MENU:
            DrawText("MAIN MENU", 10, 10, 20, DARKGRAY);
            break;
        case GameState::PLAYING:
            DrawText("PLAYING STATE - Vong lap dang chay...", 10, 10, 20, DARKBLUE);
            DrawText("Thiet ke boi Design Pattern", 10, 40, 20, DARKGRAY);
            break;
        case GameState::PAUSED:
            DrawText("PAUSED", 10, 10, 20, DARKGRAY);
            break;
        default:
            break;
    }

    EndDrawing();
}

void GameManager::CleanUp() {
    CloseWindow(); // Đóng cửa sổ Raylib
}
