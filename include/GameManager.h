#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "raylib.h"
#include "GameState.h"

// Singleton Pattern
class GameManager {
private:
    // Instance duy nhất của game
    static GameManager* instance;

    // Các biến trạng thái của trò chơi
    GameState currentState;
    int screenWidth;
    int screenHeight;
    bool isRunning;

    // Private constructor để chặn việc tạo instance bên ngoài
    GameManager();

public:
    // Ngăn chặn copy và assignment
    GameManager(const GameManager&) = delete;
    void operator=(const GameManager&) = delete;

    // Hàm lấy instance duy nhất
    static GameManager* GetInstance();

    // Hủy instance để tránh rò rỉ bộ nhớ khi kết thúc
    static void DestroyInstance();

    // Các hàm quản lý vòng lặp chính
    void Init(int width, int height, const char* title);
    void Run();
    void CleanUp();

    // Các hàm cho vòng lặp
    void Update(float deltaTime);
    void Draw();

    // Getter/Setter trạng thái
    void ChangeState(GameState newState) { currentState = newState; }
    GameState GetCurrentState() const { return currentState; }
    
    // Yêu cầu thoát game
    void QuitGame() { isRunning = false; }
};

#endif // GAMEMANAGER_H
