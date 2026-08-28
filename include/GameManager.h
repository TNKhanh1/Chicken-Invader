#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "raylib.h"
#include "GameState.h"
#include "Observer.h"
#include "StatsPanel.h"
#include <vector>
#include <memory>

// (struct MeatItem đã được chuyển thành class Meat trong Item.h)

// Singleton Pattern
class GameManager : public ISubject {
private:
    // Instance duy nhất của game
    static GameManager* instance;
    StatsPanel statsPanel;

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
    Texture2D texCoin;

    Texture2D texSwordItem;
    Texture2D texShieldItem;
    Texture2D texHeartItem;
    
    // Player resources
    Texture2D texSpaceship;
    Texture2D texSpaceshipHypergun;
    Texture2D texEnemyAnims[20];
    Texture2D texEggsecutionerBoss;
    Texture2D texBomberBoss;
    Texture2D texSpark;
    Texture2D texAsteroid1;
    Texture2D texAsteroid2;
    Texture2D texBulletStrong;
    Texture2D texBulletWeak;
    Texture2D texBulletPlayer;
    Texture2D texEnemyBullet;
    Texture2D texMeat;
    
    // Weapon project assets
    Texture2D texPlasmaRifle;
    Texture2D texAbsolverBeam[5];
    Texture2D texNeutronGun[3];
    Texture2D texRiddler;
    Texture2D texLightningFryer;
    Texture2D texIonBlaster[2];
    Texture2D texUtensilPoker[2];
    Texture2D texLaserCannon[4];

    // Selection screen textures
    Texture2D texLoi;    // Argument card background (loi.png)
    Texture2D texChiSo;  // Stat card background (chiso.png)

    // Selection screen state
    float  selectionAnimTimer     = 0.0f;
    int    nextWaveAfterSelection = 1;
    bool   pendingArgumentAfterStat = false;
    bool   isStatSelection          = true; // true = stat, false = argument
    int    extraStatSelectionsPending = 0;
    int    shownCardIndices[4]    = {0, 1, 2, 3};
    int    currentNumChoices      = 3;
    
    void GenerateSelectionPool(bool forStat);

    // Audio
    Sound sfxShoot;
    Sound sfxExplosion;
    Sound sfxPickup;
    Music bgMusic;

    // Entity lists
    std::vector<IObserver*> observers;
    std::vector<std::shared_ptr<class Bullet>> activeBullets;
    std::vector<std::shared_ptr<class Bullet>> pendingBullets;
    std::vector<std::shared_ptr<class Enemy>> activeEnemies;
    std::vector<std::shared_ptr<class Enemy>> pendingEnemies;
    std::vector<std::shared_ptr<class Item>> activeItems;
    std::vector<std::shared_ptr<class Item>> pendingItems;
    std::shared_ptr<class Spaceship> player;
    
    // Wave variables
    int currentStage = 1;
    int currentWave;
    int currentBatch;
    float waveTimer;
    bool isWaveTransitioning;

    // --- Wave Intro Animation (State: WAVE_INTRO) ---
    static constexpr float WARP_DURATION  = 3.4f;  // Thời gian warp speed background
    static constexpr float TEXT_SHOW_DURATION = 2.0f;  // Thời gian hiện chữ WAVE X
    static constexpr float TEXT_FADE_DURATION = 0.5f;  // Thời gian mờ dần chữ WAVE X
    int   pendingNextWave  = 1;      // Wave sẽ bắt đầu sau khi animation kết thúc
    float waveIntroTimer   = 0.0f;   // Đếm ngược tổng thời gian intro
    float bgScrollSpeed    = 30.0f;  // Tốc độ cuộn background (px/s) — thay magic number

    // --- Stage 7 Boss Cutscene ---
    bool isBossCutscene = false;
    float cutsceneTimer = 0.0f;
    float waveTextAlpha    = 0.0f;   // Alpha chữ WAVE X (0–255)

    // Test selection configuration (Encapsulated)
    struct TestConfig {
        int stage = 1;
        int wave = 1;
        int batch = 1;
        int maxStage = 7;
        int maxWave = 10;
        int maxBatch = 5;
    } testConfig;

struct DamageText {
    Vector2 position;
    int amount;
    bool isCrit;
    float timer;
    float maxLifetime;
};

    // Debug & Weapon Sandbox Observation
    bool showDebugHitboxes = false;
    bool debugSandboxMode = false;
    int currentWeaponType = 1; // 1: Hypergun, etc.
    Vector2 autoLockTargetPos = {0, 0};
    bool isAutoLocked = false;
    float beamAnimTimer = 0.0f;
    float beamTextTimer = 0.0f;

public:
    std::vector<DamageText> activeDamageTexts;
    Texture2D texGrenade;
    Texture2D texKnife;
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
    bool SpawnWaveBatch(int wave, int batch);
    void EnterStatSelection(int nextWave); // Chuyển vào màn hình chọn chỉ số

    // Các hàm cho vòng lặp
    void Update(float deltaTime);
    void Draw();

    // ISubject implementation
    void AddObserver(IObserver* observer) override;
    void RemoveObserver(IObserver* observer) override;
    void Notify(EventType event, const std::string& data) override;
    
    int GetCurrentWave() const { return currentWave; }
    
    // UI Helpers
    bool DrawButton(Rectangle bounds, const char* text);

    // Getter/Setter trạng thái
    void ChangeState(GameState newState) {
        previousState = currentState;
        currentState = newState;
    }
    GameState GetCurrentState() const { return currentState; }

    bool IsBossCutscene() const { return isBossCutscene; }
    float GetCutsceneTimer() const { return cutsceneTimer; }

    // Screen info getters
    int GetScreenWidth() const { return screenWidth; }
    int GetScreenHeight() const { return screenHeight; }
    
    // Texture Getters
    Texture2D GetTexSpaceship() const { return texSpaceship; }
    Texture2D& GetTexSpaceshipHypergun() { return texSpaceshipHypergun; }
    Texture2D& GetTexEnemyAnim(int variantIndex) { 
        if (variantIndex >= 0 && variantIndex < 20) return texEnemyAnims[variantIndex];
        return texEnemyAnims[0]; // fallback
    }
    Texture2D& GetTexAsteroid1() { return texAsteroid1; }
    Texture2D GetTexAsteroid2() const { return texAsteroid2; }
    Texture2D GetTexBulletStrong() const { return texBulletStrong; }
    Texture2D GetTexBulletWeak() const { return texBulletWeak; }
    Texture2D& GetTexSpark() { return texSpark; }
    Texture2D GetTexBulletPlayer() const { return texBulletPlayer; }
    Texture2D GetTexEnemyBullet() const { return texEnemyBullet; }
    Texture2D GetTexMeat() const { return texMeat; }
    
    Texture2D GetTexSwordItem() const { return texSwordItem; }
    Texture2D GetTexShieldItem() const { return texShieldItem; }
    Texture2D GetTexHeartItem() const { return texHeartItem; }
    
    // Weapon Asset Getters
    Texture2D GetTexPlasmaRifle() const { return texPlasmaRifle; }
    Texture2D GetTexAbsolverBeam(int idx) const { return texAbsolverBeam[idx >= 0 && idx < 5 ? idx : 0]; }
    Texture2D GetTexNeutronGun(int idx) const { return texNeutronGun[idx >= 0 && idx < 3 ? idx : 0]; }
    Texture2D GetTexRiddler() const { return texRiddler; }
    Texture2D GetTexLightningFryer() const { return texLightningFryer; }
    Texture2D GetTexIonBlaster(int idx) const { return texIonBlaster[idx >= 0 && idx < 2 ? idx : 0]; }
    Texture2D GetTexUtensilPoker(int idx) const { return texUtensilPoker[idx >= 0 && idx < 2 ? idx : 0]; }
    Texture2D GetTexLaserCannon(int idx) const { return texLaserCannon[idx >= 0 && idx < 4 ? idx : 0]; }
    
    // Score management
    int GetScore() const { return score; }
    void AddScore(int value) { score += value; }
    
    // Audio wrappers
    void PlayShootSound() { /*PlaySound(sfxShoot);*/ }
    void PlayExplosionSound() { /*PlaySound(sfxExplosion);*/ }
    void PlayPickupSound() { /*PlaySound(sfxPickup);*/ }
    
    // Entity management
    void AddBullet(std::shared_ptr<class Bullet> bullet) { pendingBullets.push_back(bullet); }
    void AddEnemy(std::shared_ptr<class Enemy> enemy) { pendingEnemies.push_back(enemy); }
    std::vector<std::shared_ptr<class Enemy>>& GetActiveEnemies() { return activeEnemies; }
    void AddItem(std::shared_ptr<class Item> item) { pendingItems.push_back(item); }
    std::shared_ptr<class Spaceship> GetPlayer() const { return player; }
    
    // Custom Font handling
    Font customFont;
    Font GetCustomFont() const { return customFont; }
    void DrawTextCustom(const char* text, int posX, int posY, int fontSize, Color color);
    int MeasureTextCustom(const char* text, int fontSize);

    // Yêu cầu thoát game
    void QuitGame() { isRunning = false; }
};

// Override Raylib default text functions globally for any file including GameManager.h
#define DrawText(text, x, y, size, ...) GameManager::GetInstance()->DrawTextCustom(text, x, y, size, __VA_ARGS__)
#define MeasureText(text, size) GameManager::GetInstance()->MeasureTextCustom(text, size)

#endif // GAMEMANAGER_H
