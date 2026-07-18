#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "raylib.h"
#include "GameState.h"
#include <vector>
#include <memory>

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
    int score;

    // Private constructor/destructor để chặn việc tạo instance bên ngoài
    GameManager();
    ~GameManager();

    // Entity lists
    std::vector<std::shared_ptr<class Bullet>> activeBullets;
    std::vector<std::shared_ptr<class Enemy>> activeEnemies;
    std::shared_ptr<class Spaceship> player;
    
    // Spawner variables
    float spawnTimer;

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
    
    // Score management
    int GetScore() const { return score; }
    void AddScore(int value) { score += value; }
    
    // Entity management
    void AddBullet(std::shared_ptr<class Bullet> bullet) { activeBullets.push_back(bullet); }
    void AddEnemy(std::shared_ptr<class Enemy> enemy) { activeEnemies.push_back(enemy); }
    std::shared_ptr<class Spaceship> GetPlayer() const { return player; }
    
    // Yêu cầu thoát game
    void QuitGame() { isRunning = false; }
};

#endif // GAMEMANAGER_H
