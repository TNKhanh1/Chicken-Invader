#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "raylib.h"
#include "GameState.h"
#include <vector>
#include <memory>

// (struct MeatItem đã được chuyển thành class Meat trong Item.h)

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
    
    // Player resources
    Texture2D texSpaceship;
    Texture2D texSpaceshipHypergun;
    Texture2D texEnemy;
    Texture2D texAsteroid1;
    Texture2D texAsteroid2;
    Texture2D texBulletStrong;
    Texture2D texBulletWeak;
    Texture2D texBulletPlayer;
    Texture2D texEnemyBullet;
    Texture2D texMeat;

    // Audio
    Sound sfxShoot;
    Sound sfxExplosion;
    Sound sfxPickup;
    Music bgMusic;

    // Entity lists
    std::vector<std::shared_ptr<class Bullet>> activeBullets;
    std::vector<std::shared_ptr<class Enemy>> activeEnemies;
    std::vector<std::shared_ptr<class Item>> activeItems;
    std::shared_ptr<class Spaceship> player;
    
    // Wave variables
    int currentWave;
    int currentBatch;
    float waveTimer;
    bool isWaveTransitioning;

    // Test selection
    int testSelectedWave = 1;
    int testSelectedBatch = 1;

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
    void StartWave(int waveIndex);

    // Các hàm cho vòng lặp
    void Update(float deltaTime);
    void Draw();
    
    // UI Helpers
    bool DrawButton(Rectangle bounds, const char* text);

    // Getter/Setter trạng thái
    void ChangeState(GameState newState) {
        previousState = currentState;
        currentState = newState;
    }
    GameState GetCurrentState() const { return currentState; }

    // Screen info getters
    int GetScreenWidth() const { return screenWidth; }
    int GetScreenHeight() const { return screenHeight; }
    
    // Texture Getters
    Texture2D GetTexSpaceship() const { return texSpaceship; }
    Texture2D GetTexSpaceshipHypergun() const { return texSpaceshipHypergun; }
    Texture2D GetTexEnemy() const { return texEnemy; }
    Texture2D GetTexAsteroid1() const { return texAsteroid1; }
    Texture2D GetTexAsteroid2() const { return texAsteroid2; }
    Texture2D GetTexBulletStrong() const { return texBulletStrong; }
    Texture2D GetTexBulletWeak() const { return texBulletWeak; }
    Texture2D GetTexBulletPlayer() const { return texBulletPlayer; }
    Texture2D GetTexEnemyBullet() const { return texEnemyBullet; }
    Texture2D GetTexMeat() const { return texMeat; }
    
    // Score management
    int GetScore() const { return score; }
    void AddScore(int value) { score += value; }
    
    // Audio wrappers
    void PlayShootSound() { /*PlaySound(sfxShoot);*/ }
    void PlayExplosionSound() { /*PlaySound(sfxExplosion);*/ }
    void PlayPickupSound() { /*PlaySound(sfxPickup);*/ }
    
    // Entity management
    void AddBullet(std::shared_ptr<class Bullet> bullet) { activeBullets.push_back(bullet); }
    void AddEnemy(std::shared_ptr<class Enemy> enemy) { activeEnemies.push_back(enemy); }
    std::shared_ptr<class Spaceship> GetPlayer() const { return player; }
    
    // Yêu cầu thoát game
    void QuitGame() { isRunning = false; }
};

#endif // GAMEMANAGER_H
