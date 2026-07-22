#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "raylib.h"
#include "GameState.h"
#include <vector>
#include <memory>

struct MeatItem {
    Vector2 position;
    Vector2 velocity;
    float time;
    bool active;
};

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
    
    // Background Scrolling
    float bgY;
    int currentBgIndex;
    
    GameState previousState;

    // Private constructor/destructor để chặn việc tạo instance bên ngoài
    GameManager();
    ~GameManager();

    // Textures
    Texture2D texBackgrounds[4];
    Texture2D texSettingIcon;
    Texture2D texSpaceship;
    Texture2D texEnemy;
    Texture2D texBulletPlayer;
    Texture2D texEnemyBullet;
    Texture2D texMeat;

    // Entity lists
    std::vector<std::shared_ptr<class Bullet>> activeBullets;
    std::vector<std::shared_ptr<class Enemy>> activeEnemies;
    std::vector<MeatItem> activeMeats;
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
    
    // UI Helpers
    bool DrawButton(Rectangle bounds, const char* text);

    // Getter/Setter trạng thái
    void ChangeState(GameState newState) { currentState = newState; }
    GameState GetCurrentState() const { return currentState; }
    
    // Texture Getters
    Texture2D GetTexSpaceship() const { return texSpaceship; }
    Texture2D GetTexEnemy() const { return texEnemy; }
    Texture2D GetTexBulletPlayer() const { return texBulletPlayer; }
    Texture2D GetTexEnemyBullet() const { return texEnemyBullet; }
    
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
