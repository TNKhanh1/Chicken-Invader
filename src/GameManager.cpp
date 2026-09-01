#include "WaveManager.h"
#include "../include/GameManager.h"
#include "../include/SpaceshipFactory.h"
#include "../include/EnemyFactory.h"
#include "../include/SingleShot.h"
#include "../include/SpreadShot.h"
#include "../include/DamageStoneDecorator.h"
#include "../include/WeaponStrategy.h"
#include "../include/IMovementBehavior.h"
#include "../include/StraightMovement.h"
#include "../include/HorizontalSweepMovement.h"
#include "../include/VerticalZigzagMovement.h"
#include "../include/HorizontalBounceMovement.h"
#include "../include/MeteorDiveMovement.h"
#include "../include/SineZigzagMovement.h"
#include "../include/SpiralMovement.h"
#include "../include/WaypointMovement.h"
#include "../include/Bullet.h"
#include "../include/Item.h"
#include "../include/Meat.h"
#include "../include/SpaceshipDataManager.h"
#include "../include/HypergunShootingBehavior.h"
#include "../include/BeamWeapon.h"
#include "../include/AllWeaponBehaviors.h"
#include "../include/Bosses.h"
#include "MenuManager.h"
#include "FontManager.h"
#include "ShopManager.h"
#include "ShopUI.h"
#include "TutorialUI.h"
#include "TitleScreen.h"
#include "RuneManager.h"
#include "RuneSelectionUI.h"
#include "SummaryScreen.h"
#include "SoundManager.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include "../include/CoinManager.h"
#include "../include/ItemDropManager.h"
#include "../include/PowerUpItem.h"
#include "../include/ProgressManager.h"
#include "../include/AIController.h"

// ---------------------------------------------------------
// Dữ liệu tĩnh cho màn hình chọn Argument (lõi) và Chỉ Số (stat)
// ---------------------------------------------------------
struct CardDef {
    const char* name;
    const char* description;
};

// 10 loại Argument (lõi) - Effect chưa code, chỉ lưu thông tin
static const CardDef ALL_ARGUMENTS[10] = {
    { "EXP Amplifier",
      "Gain +50% EXP\nfrom all enemies\nthroughout the game." },
    { "Stat Windfall",
      "Immediately receive\n3 extra Stat\nSelections right now." },
    { "Abundant Gifts",
      "Future Stat Selections\nshow 4 choices\ninstead of 3." },
    { "Boss Hunter",
      "Deal +80% bonus\ndamage to all\nBoss enemies." },
    { "Armor Crusher",
      "Each hit deals bonus\ndmg = 3% of\ntarget's current HP." },
    { "Blood Fury",
      "Each enemy kill\ngrants +2 permanent\nflat Damage." },
    { "Bloodthirst",
      "Killing an enemy\nrestores 5% of your\nMax HP." },
    { "Energy Flow",
      "Each shot fired\nrestores 2 Mana\nto your reserves." },
    { "Round Recovery",
      "Restore 30% of your\nMax HP at the start\nof each new Wave." },
    { "Fast Track",
      "Immediately gain\n3 Levels." }
};

// 6 loại Chỉ Số (stat)
static const CardDef ALL_STATS[6] = {
    { "Max HP  +30",
      "Your spaceship's\nmax health\ngrows permanently." },
    { "Damage  +5",
      "Your attack power\nincreases\npermanently." },
    { "Armor  +3",
      "Reduces incoming\ndamage by\n3 extra points." },
    { "Fire Rate  +10%",
      "Reduce weapon\ncooldown time\nfor faster shots." },
    { "Crit Chance  +10%",
      "Increases the\nchance to deal\ncritical damage." },
    { "Crit Damage  +30%",
      "Increases the\ndamage multiplier\nof critical hits." }
};

// Khởi tạo instance của Singleton bằng nullptr
GameManager* GameManager::instance = nullptr;

static Vector2 autoLockTargetPos2 = {0,0};
static float beamTextTimer2 = 0.0f;
static bool isAutoLocked2 = false;
void GameManager::GenerateSelectionPool(bool forStat) {
    std::vector<int> available;
    int maxIdx = forStat ? 6 : 10;
    
    for (int i = 0; i < maxIdx; ++i) {
        if (!forStat && player && player->HasArgument(i)) {
            continue; // Skip already owned cores
        }
        available.push_back(i);
    }
    
    int desiredChoices = (player && player->HasArgument(2)) ? 4 : 3;
    if (available.size() < (size_t)desiredChoices) {
        desiredChoices = available.size();
    }
    
    currentNumChoices = desiredChoices;
    
    // Shuffle
    for (int i = 0; i < currentNumChoices; i++) {
        int j = i + GetRandomValue(0, available.size() - 1 - i);
        int tmp = available[i];
        available[i] = available[j];
        available[j] = tmp;
        shownCardIndices[i] = available[i];
    }
}

// --- Implement methods for entities ---

void Spaceship::Draw() {
    if (!isActive) return;
    
    float offsetY = GetRecoilOffset();
    float squish = (offsetY / 26.0f) * 0.08f; 
    float drawWidth = 90.0f * (1.0f + squish);
    float drawHeight = 90.0f * (1.0f - squish);

    // -------------------------------------------------------------------------
    // VẼ HỆ THỐNG ĐUÔI NĂNG LƯỢNG PLASMA ĐỘNG CƠ (3-Layer Plasma Thruster VFX)
    // -------------------------------------------------------------------------
    float intensity = GetThrusterIntensity();     // Từ 0.3f đến 2.5f
    float tiltX = GetThrusterTiltX();             // Dao động nghiêng từ -28 đến +28px
    float lenMult = GetThrusterLengthMult();      // Từ 0.7f đến 1.8f
    
    // Tâm miệng xả động cơ ở phía sau con tàu
    Vector2 enginePos = { position.x, position.y + offsetY + 32.0f };
    
    // Chiều dài ngọn lửa plasma: vươn dài rực rỡ khi bứt tốc hoặc nã đạn lả tả
    float flameLen = (18.0f + intensity * 24.0f) * lenMult + (float)GetRandomValue(-2, 3);
    float flameWidth = 14.0f + std::min(12.0f, intensity * 5.0f);
    
    // Tọa độ các đỉnh của ngọn lửa hình tam giác phun lùi về sau
    Vector2 leftBase = { enginePos.x - flameWidth / 2.0f, enginePos.y };
    Vector2 rightBase = { enginePos.x + flameWidth / 2.0f, enginePos.y };
    Vector2 tip = { enginePos.x + tiltX + (float)GetRandomValue(-1, 1), enginePos.y + flameLen };
    
    // Tầng 1: Hào quang bao quanh rực rỡ (Outer Electric Blue Aura)
    unsigned char alpha1 = (unsigned char)std::min(255, (int)(75 * intensity));
    Color colorOuter = { 0, 140, 255, alpha1 };
    DrawCircleV(enginePos, flameWidth * 0.7f, colorOuter); // Ánh hào quang tại miệng xả
    DrawTriangle(leftBase, tip, rightBase, colorOuter);
    DrawTriangle(leftBase, rightBase, tip, colorOuter); // Vẽ 2 chiều chống backface culling
    
    // Tầng 2: Lõi Plasma sáng mượt mà (Middle Neon Cyan Core)
    Vector2 leftMid = { enginePos.x - (flameWidth * 0.35f), enginePos.y };
    Vector2 rightMid = { enginePos.x + (flameWidth * 0.35f), enginePos.y };
    Vector2 tipMid = { enginePos.x + (tiltX * 0.8f), enginePos.y + (flameLen * 0.75f) };
    unsigned char alpha2 = (unsigned char)std::min(255, (int)(110 * intensity));
    Color colorMid = { 0, 230, 255, alpha2 };
    DrawTriangle(leftMid, tipMid, rightMid, colorMid);
    DrawTriangle(leftMid, rightMid, tipMid, colorMid);
    
    // Tầng 3: Lõi chớp trắng rực tâm buồng đốt (Inner White-Hot Incandescence)
    Vector2 leftCore = { enginePos.x - (flameWidth * 0.18f), enginePos.y };
    Vector2 rightCore = { enginePos.x + (flameWidth * 0.18f), enginePos.y };
    Vector2 tipCore = { enginePos.x + (tiltX * 0.5f), enginePos.y + (flameLen * 0.4f) };
    unsigned char alpha3 = (unsigned char)std::min(255, (int)(160 * intensity));
    Color colorCore = { 220, 255, 255, alpha3 };
    DrawCircleV(enginePos, flameWidth * 0.3f, { 255, 255, 255, (unsigned char)std::min(255, (int)(200 * intensity)) });
    DrawTriangle(leftCore, tipCore, rightCore, colorCore);
    DrawTriangle(leftCore, rightCore, tipCore, colorCore);
    
    // Hiệu ứng tàn thiêu hạt plasma tản rã sa rây phía sau tip khi di chuyển nhanh hoặc xả đạn (Intensity > 0.8)
    if (intensity > 0.8f) {
        int sparkCount = (int)(intensity * 2.5f); // 2 đến 6 hạt tàn plasma
        for (int i = 1; i <= sparkCount; ++i) {
            float sparkDist = i * 14.0f + GetRandomValue(-4, 6);
            Vector2 sparkPos = { tip.x + (float)GetRandomValue(-7, 7) * (i * 0.3f), tip.y + sparkDist };
            float sparkRad = std::max(1.5f, 5.0f - i * 0.7f);
            unsigned char sparkAlpha = (unsigned char)std::max(0, 230 - i * 35);
            DrawCircleV(sparkPos, sparkRad, { 60, 230, 255, sparkAlpha });
        }
    }
    // -------------------------------------------------------------------------

    // Mặc định luôn vẽ Hypergun vì ảnh cũ đã bị xóa
    Texture2D tex = hasCustomTexture ? customTexture : GameManager::GetInstance()->GetTexSpaceshipHypergun();

    // Hỗ trợ vẽ texture đè nếu có Argument/Buff (VD: lõi khiên)
    if (activeBuff.shieldActive) {
        DrawCircleV(position, 50.0f, { 0, 200, 255, 100 });
    }

    if (tex.id != 0) {
        Rectangle sourceRec = hasCustomTexture ? 
                              Rectangle{41.0f, 50.0f, 108.0f, 106.0f} : 
                              Rectangle{36.0f, 201.0f, 108.0f, 95.0f};
                              
        DrawTexturePro(
            tex, 
            sourceRec, 
            {position.x, position.y + offsetY, drawWidth, drawHeight}, 
            {drawWidth/2, drawHeight/2}, 
            0.0f, 
            WHITE
        );
    }
    
    // Draw Level & Shield UI
    FontManager::GetInstance()->DrawGameText(std::to_string(GetLevel()).c_str(), position.x - 5, position.y + 40, 15, YELLOW, "Modern");
}
// -------------------------------------------

GameManager::GameManager() 
    : currentState(GameState::MAIN_MENU), nextState(GameState::MAIN_MENU), previousState(GameState::MAIN_MENU), screenWidth(1600), screenHeight(900), isRunning(false), score(0), currentWave(0), currentBatch(0), waveTimer(0.0f), isWaveTransitioning(false) {
    texSpaceship = {0};
    texSpaceshipHypergun = {0};
    texAsteroid1 = {0};
    texAsteroid2 = {0};
    texBulletStrong = {0};
    texBulletWeak = {0};
    texBulletPlayer = {0};
    texMeat = {0};
    texPlasmaRifle = {0};
    for(int i=0; i<5; i++) texAbsolverBeam[i] = {0};
    for(int i=0; i<3; i++) texNeutronGun[i] = {0};
    texRiddler = {0};
    texLightningFryer = {0};
    for(int i=0; i<2; i++) { texIonBlaster[i] = {0}; texUtensilPoker[i] = {0}; }
    for(int i=0; i<4; i++) texLaserCannon[i] = {0};
    for(int i=0; i<20; i++) texEnemyAnims[i] = {0};
    texLoi   = {0};
    texChiSo = {0};
    texSettingIcon = {0};
    for(int i=0; i<4; i++) texBackgrounds[i] = {0};
}

GameManager::~GameManager() {
    CleanUp();
}

void GameManager::AddObserver(IObserver* observer) {
    if (std::find(observers.begin(), observers.end(), observer) == observers.end()) {
        observers.push_back(observer);
    }
}

void GameManager::RemoveObserver(IObserver* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void GameManager::Notify(EventType event, const std::string& data) {
    for (auto* observer : observers) {
        observer->OnNotify(event, data);
    }
}

GameManager* GameManager::GetInstance() {
    if (instance == nullptr) {
        instance = new GameManager();
    }
    return instance;
}

Spaceship* GameManager::GetNearestPlayer(Vector2 from) const {
    Spaceship* p1 = player.get();
    Spaceship* p2 = player2.get();
    
    if (!p1 && !p2) return nullptr;
    if (p1 && !p1->IsActive()) p1 = nullptr;
    if (p2 && !p2->IsActive()) p2 = nullptr;
    
    if (p1 && p2) {
        Vector2 pos1 = p1->GetPosition();
        Vector2 pos2 = p2->GetPosition();
        float d1 = (pos1.x - from.x) * (pos1.x - from.x) + (pos1.y - from.y) * (pos1.y - from.y);
        float d2 = (pos2.x - from.x) * (pos2.x - from.x) + (pos2.y - from.y) * (pos2.y - from.y);
        return (d1 < d2) ? p1 : p2;
    } else if (p1) {
        return p1;
    } else if (p2) {
        return p2;
    }
    return nullptr;
}

void GameManager::DestroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

void GameManager::StartWave(int waveIndex) {
    // Round Recovery (Arg 8)
    if (player && player->HasArgument(8)) {
        player->Heal(player->GetMaxHp() * 0.30f);
    }

    // Clear everything from the previous wave
    activeBullets.clear();
    pendingBullets.clear();
    activeItems.clear();
    pendingItems.clear();
    activeEnemies.clear();
    pendingEnemies.clear();
    activeDamageTexts.clear();

    // Kích hoạt hiệu ứng chuyển wave (WAVE_INTRO) thay vì nhảy thẳng vào gameplay
    pendingNextWave = waveIndex;
    currentWave    = waveIndex; // cập nhật sớm để Draw() hiển thị đúng số wave
    currentBatch   = 1;
    waveIntroTimer = WARP_DURATION + TEXT_SHOW_DURATION + TEXT_FADE_DURATION;
    bgScrollSpeed  = 30.0f; // reset về bình thường, sẽ được tăng dần trong Update()
    waveTextAlpha  = 0.0f;
    ChangeState(GameState::WAVE_INTRO);
}

void GameManager::EnterStatSelection(int nextWave) {
    nextWaveAfterSelection  = nextWave;
    if (currentStage == 6) {
        pendingArgumentAfterStat = true; // Stage 6 always gives a core
        extraStatSelectionsPending = 2;  // Pick 3 stats total (1 initial + 2 extra)
    } else {
        pendingArgumentAfterStat = (nextWave == 5 || nextWave == 10 || nextWave == 15);
    }
    isStatSelection         = true;
    selectionAnimTimer      = 0.0f;

    GenerateSelectionPool(true);
    ChangeState(GameState::STAT_SELECTION);
}

bool GameManager::SpawnWaveBatch(int wave, int batch) {
    ItemDropManager::GetInstance()->OnNewBatchStarted();
    return WaveManager::GetInstance()->SpawnBatch(wave, batch);
}

void GameManager::StartStage(int stageId, GameMode mode) {
    currentStage = stageId;
    currentWave = 1;
    currentBatch = 1;
    currentGameMode = mode;
    isTestMode = false;
    
    if (currentStage != 7) {
        WaveManager::GetInstance()->LoadStage("data/stage" + std::to_string(currentStage) + ".json");
    }
    
    waveTimer = 3.0f;
    activeItems.clear();
    extraStatSelectionsPending = 0;
    pendingArgumentAfterStat = false;
    score = 0;
    CoinManager::GetInstance()->ResetSession();
    
    // Initialize Player 1
    if (!player) {
        player = SpaceshipFactory::CreateSpaceship("Hypergun", 1, {(float)screenWidth/2, (float)screenHeight - 100});
        player->SetShootingBehavior(std::make_unique<HypergunShootingBehavior>());
    } else {
        player->ResetToBaseStats();
        player->SetPosition({(float)screenWidth/2, (float)screenHeight - 100});
    }
    
    std::string savedWeapon = ShopManager::GetInstance()->GetSelectedWeapon();
    if (!savedWeapon.empty()) {
        player->SetWeapon(savedWeapon);
    }
    
    // Initialize Player 2 if needed
    if (mode == GameMode::TWO_PLAYERS || mode == GameMode::PLAYER_AND_AI) {
        if (!player2) {
            player2 = SpaceshipFactory::CreateSpaceship("Hypergun", 1, {(float)screenWidth/2 + 80, (float)screenHeight - 100});
            player2->SetShootingBehavior(std::make_unique<HypergunShootingBehavior>());
            player2->SetTextureOverride(texSpaceship2);
        } else {
            player2->ResetToBaseStats();
            player2->SetPosition({(float)screenWidth/2 + 80, (float)screenHeight - 100});
        }
        
        if (!savedWeapon.empty()) {
            player2->SetWeapon(savedWeapon);
        }
        
        if (mode == GameMode::PLAYER_AND_AI) {
            aiController = std::make_unique<AIController>();
        } else {
            aiController.reset();
        }
    } else {
        if (player2) player2.reset();
        aiController.reset();
    }
    
    RuneManager::GetInstance()->ApplyAll(player.get(), player2 ? player2.get() : nullptr);
    
    // Set starting level for boss-only stages (no leveling possible)
    if (currentStage == 6) {
        player->SetLevel(7);
        player->ReloadStatsFromCSV();
        if (player2) { player2->SetLevel(7); player2->ReloadStatsFromCSV(); }
    }
    if (currentStage == 7) {
        player->SetLevel(10);
        player->ReloadStatsFromCSV();
        if (player2) { player2->SetLevel(10); player2->ReloadStatsFromCSV(); }
    }
    
    pendingEggSkinLoad = true;
    
    if (currentStage == 7) {
        player->MultiplyHp(3.0f);
        if (player2) player2->MultiplyHp(3.0f);
    }
    
    if (currentStage == 6) {
        EnterStatSelection(1);
    } else {
        StartWave(1);
    }
    
    // Boss stage
    if (currentStage == 7) {
        isBossCutscene = true;
        cutsceneTimer = 0.0f;
        auto mcb = std::make_shared<MilitaryChickenBoss>(12, EnemyStats{150000.0f, 50.0f, 20.0f, 150.0f, 0.0f, 100}, Vector2{(float)screenWidth/2 - 150, -200.0f});
        auto scb = std::make_shared<SuperChickBoss>(13, EnemyStats{150000.0f, 50.0f, 20.0f, 150.0f, 0.0f, 100}, Vector2{(float)screenWidth/2 + 150, -200.0f});
        activeEnemies.push_back(mcb);
        activeEnemies.push_back(scb);
    }
}

void GameManager::Init(int width, int height, const char* title) {
    screenWidth = width;
    screenHeight = height;
    
    // Khởi tạo cửa sổ Raylib
    InitWindow(screenWidth, screenHeight, "CHICKEN INVADERS");
    SetTargetFPS(60); // Đặt tốc độ khung hình 60 FPS
    
    SoundManager::GetInstance()->Init();
    
    // Load progress
    ProgressManager::GetInstance()->LoadProgress();

    // Khởi tạo giao diện UI
    mainMenuUI = new MenuManager(screenWidth, screenHeight);
    shopUI = new ShopUI(screenWidth, screenHeight);
    tutorialUI = new TutorialUI(screenWidth, screenHeight);
    runeUI = new RuneSelectionUI(screenWidth, screenHeight);
    summaryScreen = new SummaryScreen(screenWidth, screenHeight);
    
    // Khởi tạo FontManager và tải fonts
    FontManager::GetInstance()->LoadFonts();

    // Tải thông tin shop (đã unlock skin)
    ShopManager::GetInstance()->Load();

    // Khởi tạo các textures
    // InitAudioDevice();
    // sfxShoot = LoadSound("assets/shoot.wav");
    // sfxExplosion = LoadSound("assets/explosion.wav");
    // sfxPickup = LoadSound("assets/pickup.wav");
    // bgMusic = LoadMusicStream("assets/bgm.mp3");
    // PlayMusicStream(bgMusic);
    
    texBackgrounds[0] = LoadTexture("assets/background.jpg");
    texBackgrounds[1] = LoadTexture("assets/background.png");
    texBackgrounds[2] = LoadTexture("assets/background1.png");
    texBackgrounds[3] = LoadTexture("assets/background2.png");
    texSettingIcon = LoadTexture("assets/setting.png");
    texCoin = LoadTexture("assets/coin.png");
    CoinManager::GetInstance()->Load();
    AddObserver(CoinManager::GetInstance());
    CoinManager::GetInstance()->ResetSession();
    
    ShopManager::GetInstance()->Load();
    
    // Tải dữ liệu phi thuyền
    SpaceshipDataManager::GetInstance()->LoadCSV("assets/spaceship/spaceship.csv");
    SpaceshipDataManager::GetInstance()->LoadJSON("Hypergun", "assets/spaceship/hypergun.json");

    texSpaceshipHypergun = LoadTexture("assets/spaceship/hypergun_spaceship.png");
    texSpaceship2 = LoadTexture("assets/spaceship/spaceship2.png");
    texBulletStrong = LoadTexture("assets/spaceship/hypergun_strong.png");
    texBulletWeak = LoadTexture("assets/spaceship/hypergun_weak.png");
    
    for (int i = 0; i < 11; i++) {
        char path[100];
        snprintf(path, sizeof(path), "assets/enemy/chicken%02d_anim.png", i + 1);
        texEnemyAnims[i] = LoadTexture(path);
    }
    texEnemyAnims[11] = LoadTexture("assets/enemy/Militarychicken_anim.png");
    texEnemyAnims[12] = LoadTexture("assets/enemy/CI4_SuperChick_Summer_anim.png");
    texEnemyAnims[13] = LoadTexture("assets/enemy/chicken12_anim.png");
    
    texAsteroid1 = LoadTexture("assets/asteroidNormal.png");
    texAsteroid2 = LoadTexture("assets/asteroidFlame.png"); // asteroidFlame: 7680x2048 = 15col x 4row = 60 frames
    
    std::string eggPath = ShopManager::GetInstance()->GetSelectedEggTexturePath();
    texEnemyBullet = LoadTexture(eggPath.c_str()); 
    texMeat = LoadTexture("assets/meat.png");
    
    texSwordItem = LoadTexture("assets/SwordItem.png");
    texShieldItem = LoadTexture("assets/ShieldItem.png");
    texHeartItem = LoadTexture("assets/HeartItem.png");
    
    // Tải trọn bộ asset đồ họa của các loại vũ khí
    texPlasmaRifle = LoadTexture("assets/spaceship/plasma_rifle.png");
    SetTextureWrap(texPlasmaRifle, TEXTURE_WRAP_REPEAT);
    SetTextureFilter(texPlasmaRifle, TEXTURE_FILTER_BILINEAR);
    texNeutronGun[0] = LoadTexture("assets/spaceship/NeutronGunWeak.png");
    texNeutronGun[1] = LoadTexture("assets/spaceship/NeutronGunMedium.png");
    texNeutronGun[2] = LoadTexture("assets/spaceship/NeutronGunStrong.png");
    texRiddler = LoadTexture("assets/spaceship/Riddler.png");
    texLightningFryer = LoadTexture("assets/spaceship/LightningFryer.png");
    SetTextureWrap(texLightningFryer, TEXTURE_WRAP_REPEAT);
    SetTextureFilter(texLightningFryer, TEXTURE_FILTER_BILINEAR);
    texIonBlaster[0] = LoadTexture("assets/spaceship/IonBlasterSingle.png");
    texIonBlaster[1] = LoadTexture("assets/spaceship/IonBlasterDouble.png");
    texUtensilPoker[0] = LoadTexture("assets/spaceship/UtensilPokerFork.png");
    texUtensilPoker[1] = LoadTexture("assets/spaceship/UtensilPokerCarving.png");
    texLaserCannon[0] = LoadTexture("assets/spaceship/LaserCannonWeak.png");
    texLaserCannon[1] = LoadTexture("assets/spaceship/LaserCannonMediumWeak.png");
    texLaserCannon[2] = LoadTexture("assets/spaceship/LaserCannonMediumStrong.png");
    texLaserCannon[3] = LoadTexture("assets/spaceship/LaserCannonStrong.png");
    
    texGrenade = LoadTexture("assets/enemy/grenade.png");
    texKnife = LoadTexture("assets/enemy/knife.png");
    texSpark = LoadTexture("assets/enemy/spark.png");

    // Selection screen card backgrounds
    texLoi   = LoadTexture("assets/loi.png");
    texChiSo = LoadTexture("assets/chiso.png");
    bgY = 0.0f;
    currentBgIndex = 0;
    
    printf("Initializing Player...\n"); fflush(stdout);
    // Khởi tạo Player
    player = SpaceshipFactory::CreateSpaceship("Hypergun", 1, {(float)screenWidth/2, (float)screenHeight - 100});
    printf("Setting shooting behavior...\n"); fflush(stdout);
    player->SetShootingBehavior(std::make_unique<HypergunShootingBehavior>());
    
    std::string savedWeapon = ShopManager::GetInstance()->GetSelectedWeapon();
    if (!savedWeapon.empty() && savedWeapon != "Hypergun") {
        player->SetWeapon(savedWeapon);
    }
    
    shopUI->Init();

    printf("GameManager::Init finished.\n"); fflush(stdout);
    isRunning = true;
    titleScreen = new TitleScreen(screenWidth, screenHeight);
    ChangeState(GameState::TITLE_SCREEN);
}

void GameManager::Run() {
    printf("GameManager::Run started\n"); fflush(stdout);
    int frameCount = 0;
    while (isRunning && !WindowShouldClose()) {
        if (frameCount < 5) { printf("Frame %d\n", frameCount); fflush(stdout); }
        frameCount++;
        float deltaTime = GetFrameTime();
        Update(deltaTime);
        Draw();
    }
    printf("GameManager::Run ended. isRunning=%d, WindowShouldClose=%d\n", isRunning, WindowShouldClose());
}

bool GameManager::DrawButton(Rectangle bounds, const char* text) {
    bool clicked = false;
    Vector2 mousePoint = GetMousePosition();
    
    // Check collision with mouse
    bool isHovered = CheckCollisionPointRec(mousePoint, bounds);
    
    // Change color on hover
    Color bgColor = isHovered ? LIGHTGRAY : GRAY;
    Color textColor = isHovered ? BLACK : DARKGRAY;
    
    if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        clicked = true;
        bgColor = DARKGRAY; // Pressed visual feedback
    }

    DrawRectangleRec(bounds, bgColor);
    DrawRectangleLinesEx(bounds, 2, BLACK);
    
    // Center text
    int textWidth = FontManager::GetInstance()->MeasureGameText(text, 20, "Modern").x;
    FontManager::GetInstance()->DrawGameText(text, bounds.x + (bounds.width / 2) - (textWidth / 2), bounds.y + (bounds.height / 2) - 10, 20, textColor);
    
    return clicked;
}

void GameManager::EnterSummary(SummaryResult result) {
    int totalW = WaveManager::GetInstance()->GetTotalWaves();
    bool isWin = (result == SummaryResult::WIN);
    CoinManager::GetInstance()->CalculateStageBonus(currentWave, totalW, isWin);
    // Fetch values BEFORE committing (since Commit resets them to 0)
    int sessionCoins = CoinManager::GetInstance()->GetSessionCoins();
    int bonusCoins = CoinManager::GetInstance()->GetStageBonusCoins();
    
    CoinManager::GetInstance()->CommitSessionCoins();
    
    summaryScreen->Show(result, score,
        sessionCoins,
        bonusCoins,
        CoinManager::GetInstance()->GetTotalCoins());
    ChangeState(GameState::SUMMARY);
}

void GameManager::Update(float deltaTime) {
    if (!isRunning) return;
    
    // Toggle Settings via 'P'
    if (IsKeyPressed(KEY_P)) {
        if (currentState == GameState::SETTINGS) {
            ChangeState(previousState);
        } else if (currentState == GameState::TEST_GAMEPLAY) {
            previousState = currentState;
            ChangeState(GameState::SETTINGS);
        }
    }
    
    if (currentState != nextState) {
        currentState = nextState;
    }
    
    SoundManager::GetInstance()->UpdateMusic();

    if (pendingEggSkinLoad) {
        std::string eggPath = ShopManager::GetInstance()->GetSelectedEggTexturePath();
        if (texEnemyBullet.id != 0) UnloadTexture(texEnemyBullet);
        texEnemyBullet = LoadTexture(eggPath.c_str());
        pendingEggSkinLoad = false;
    }

    // UpdateMusicStream(bgMusic);

    // Dynamic background in menus
    if (currentState != GameState::TEST_GAMEPLAY &&
        currentState != GameState::TEST_ENEMY &&
        currentState != GameState::TEST_SPACESHIP &&
        currentState != GameState::WAVE_INTRO &&
        currentState != GameState::STAT_SELECTION &&
        currentState != GameState::ARGUMENT_SELECTION) {
        
        bgY += 30.0f * deltaTime;
        if (bgY >= 2.0f * screenHeight) bgY -= 2.0f * screenHeight;
    }

    // Handle State Transitions and specific state logic
    switch (currentState) {
case GameState::MAIN_MENU:
        case GameState::TEST_MENU:
        case GameState::GAME_OVER:
        case GameState::SETTINGS:
            // Transition logic is handled in Draw() via DrawButton
            break;
            
        case GameState::SHOP:
            if (shopUI) shopUI->Update();
            break;
            
        case GameState::TUTORIAL:
            tutorialUI->Update();
            if (tutorialUI->IsBackRequested()) {
                ChangeState(GameState::MAIN_MENU);
            }
            break;

        case GameState::RUNE_SELECTION:
        {
            if (runeUI) {
                runeUI->Update(deltaTime);
                if (runeUI->IsStartRequested() || IsKeyPressed(KEY_SPACE)) {
                    runeUI->ResetStartFlag();
                    StartStage(pendingStageFromMenu, currentGameMode);
                }
            }
            break;
        }

        case GameState::WAVE_INTRO:
        {
            waveIntroTimer -= deltaTime;
            float totalDur = WARP_DURATION + TEXT_SHOW_DURATION + TEXT_FADE_DURATION;
            float elapsed  = totalDur - waveIntroTimer;

            if (elapsed < WARP_DURATION) {
                // Phase 1: Warp speed, no text
                float t = elapsed / 1.0f; // ease-in for 1s
                if (t > 1.0f) t = 1.0f;
                bgScrollSpeed = 30.0f + (3000.0f - 30.0f) * t;
                waveTextAlpha = 0.0f;
            } else if (elapsed < WARP_DURATION + TEXT_SHOW_DURATION) {
                // Phase 2: Slow down warp, show text
                bgScrollSpeed = std::max(30.0f, bgScrollSpeed - 2000.0f * deltaTime);
                waveTextAlpha = std::min(255.0f, waveTextAlpha + 500.0f * deltaTime);
            } else {
                // Phase 3: Text fades out
                bgScrollSpeed = 30.0f;
                waveTextAlpha = std::max(0.0f, waveTextAlpha - 500.0f * deltaTime);
            }

            // Cuộn background liên tục trong suốt intro
            bgY += bgScrollSpeed * deltaTime;
            if (bgY >= 2.0f * screenHeight) bgY -= 2.0f * screenHeight;

            // --- Cho phép người chơi di chuyển (nhưng không bắn) ---
            if (player && player->IsActive()) {
                Vector2 pPos = player->GetPosition();
                if (IsKeyDown(KEY_W)) pPos.y -= player->GetMoveSpeed() * deltaTime;
                if (IsKeyDown(KEY_S)) pPos.y += player->GetMoveSpeed() * deltaTime;
                if (IsKeyDown(KEY_A)) pPos.x -= player->GetMoveSpeed() * deltaTime;
                if (IsKeyDown(KEY_D)) pPos.x += player->GetMoveSpeed() * deltaTime;
                
                if (pPos.x < 0) pPos.x = 0;
                if (pPos.x > screenWidth) pPos.x = screenWidth;
                if (pPos.y < 0) pPos.y = 0;
                if (pPos.y > screenHeight) pPos.y = screenHeight;
                player->SetPosition(pPos);
                
                player->Update(deltaTime);
            }
            if (player2 && player2->IsActive()) {
                if (currentGameMode == GameMode::TWO_PLAYERS) {
                    Vector2 mousePos = GetMousePosition();
                    player2->SetPosition(mousePos);
                } else if (currentGameMode == GameMode::PLAYER_AND_AI && aiController) {
                    aiController->Update(player2.get(), deltaTime, activeBullets, activeEnemies, true, screenWidth, screenHeight);
                }
                player2->Update(deltaTime);
            }

            // --- Kết thúc intro: bắt đầu wave thật ---
            if (waveIntroTimer <= 0.0f) {
                bgScrollSpeed     = 30.0f;
                waveTextAlpha     = 0.0f;
                isWaveTransitioning = false;
                ChangeState(GameState::TEST_GAMEPLAY);
                if (currentStage != 7) {
                    // Spawn batch đầu tiên của wave
                    bool spawned = SpawnWaveBatch(currentWave, currentBatch);
                    if (!spawned) {
                        // Nếu wave không có enemy (cạn batch), chuyển thẳng sang chọn chỉ số
                        EnterStatSelection(currentWave + 1);
                    }
                }
            }
            break;
        }

        case GameState::TEST_GAMEPLAY:
        case GameState::TEST_ENEMY:
        case GameState::TEST_SPACESHIP:
        {
            WaveManager::GetInstance()->Update(deltaTime);
            RuneManager::GetInstance()->UpdateAll(player.get(), player2.get(), deltaTime);
            
            // --- Background Scrolling ---
            bgY += bgScrollSpeed * deltaTime; // Tốc độ cuộn background động
            // Cập nhật Damage Texts
            for (auto it = activeDamageTexts.begin(); it != activeDamageTexts.end(); ) {
                it->timer -= deltaTime;
                it->position.y -= 40.0f * deltaTime; // Bay lên từ từ
                if (it->timer <= 0) {
                    it = activeDamageTexts.erase(it);
                } else {
                    ++it;
                }
            }
            if (bgY >= 2.0f * screenHeight) bgY -= 2.0f * screenHeight;
            
            auto ProcessBeamWeapon = [&](Spaceship* p, bool isFiring, Vector2& lockTargetPos, float& textTimer, bool& isLocked) {
                if (!p || !p->IsActive() || !isFiring) return;
                std::string wpn = p->GetWeapon();
                bool isBeam = (wpn == "Lightning_Fryer" || wpn == "Plasma_Rifle" || wpn == "Laser_Cannon");
                if (!isBeam) {
                    if (p->CanFire()) p->Fire();
                    return;
                }
                if (p->CanFire()) p->Fire();
                Vector2 origin = { p->GetPosition().x, p->GetPosition().y - 20.0f };
                Vector2 endPos = { origin.x, -100.0f };
                float beamWidth = 30.0f;
                float damageRate = (p->GetDamage() + 40.0f) * 4.0f * 0.85f;
                
                isLocked = false;
                if (wpn == "Lightning_Fryer") {
                    static LightningFryerBehavior fryerAim;
                    float aimAngle = 0.0f;
                    isLocked = fryerAim.FindNearestTarget(origin, activeEnemies, lockTargetPos, aimAngle);
                    if (isLocked) endPos = lockTargetPos;
                    beamWidth = 25.0f;
                } else if (wpn == "Plasma_Rifle") {
                    beamWidth = 30.0f + p->GetLevel() * 8.0f;
                } else if (wpn == "Laser_Cannon") {
                    beamWidth = 20.0f + p->GetLevel() * 4.0f;
                    damageRate *= 1.2f;
                }
                lockTargetPos = endPos;

                if (currentState == GameState::TEST_GAMEPLAY || currentState == GameState::TEST_ENEMY) {
                    Vector2 v = { endPos.x - origin.x, endPos.y - origin.y };
                    float vLenSq = v.x * v.x + v.y * v.y;
                    if (vLenSq > 0.0f) {
                        bool popBeamText = false;
                        textTimer += deltaTime;
                        if (textTimer >= 0.25f) {
                            popBeamText = true;
                            textTimer = 0.0f;
                        }
                        for (auto& enemy : activeEnemies) {
                            if (!enemy || !enemy->IsActive()) continue;
                            Vector2 ePos = enemy->GetPosition();
                            Vector2 w = { ePos.x - origin.x, ePos.y - origin.y };
                            float t = std::max(0.0f, std::min(1.0f, (w.x * v.x + w.y * v.y) / vLenSq));
                            Vector2 closest = { origin.x + t * v.x, origin.y + t * v.y };
                            float dx = ePos.x - closest.x;
                            float dy = ePos.y - closest.y;
                            if ((dx * dx + dy * dy) <= (35.0f + beamWidth / 2.0f) * (35.0f + beamWidth / 2.0f)) {
                                float finalDmg = damageRate * deltaTime;
                                bool isCrit = (GetRandomValue(0, 100) < p->GetCritChance());
                                if (isCrit) finalDmg *= (p->GetCritDamage() / 100.0f);
                                if (p->HasArgument(3) && enemy->role == EnemyRole::BOSS) finalDmg *= 1.8f;
                                if (p->HasArgument(4)) finalDmg += enemy->GetHp() * 0.03f * deltaTime;
                                enemy->TakeDamage(finalDmg);
                                if (popBeamText) {
                                    float displayDmg = damageRate * 0.25f;
                                    if (isCrit) displayDmg *= (p->GetCritDamage() / 100.0f);
                                    if (p->HasArgument(3) && enemy->role == EnemyRole::BOSS) displayDmg *= 1.8f;
                                    float offsetX = (float)GetRandomValue(-20, 20) + 20.0f;
                                    float lifetime = isCrit ? 0.7f : 0.45f;
                                    activeDamageTexts.push_back({{enemy->GetPosition().x + offsetX, enemy->GetPosition().y}, (int)displayDmg, isCrit, lifetime, lifetime});
                                }
                                if (!enemy->IsActive()) {
                                    WaveManager::GetInstance()->AddKill();
                                    AddScore(enemy->GetPointValue());
                                    p->GainExp(enemy->GetPointValue() * 0.5f);
                                    int coin = CoinManager::GetInstance()->GetKillCoin(enemy.get());
                                    Notify(EventType::ENEMY_DIED, std::to_string(coin));
                                    if (p->HasArgument(5)) p->AddPermanentDamage(2.0f);
                                    if (p->HasArgument(6)) p->Heal(p->GetMaxHp() * 0.05f);
                                    PlayExplosionSound();
                                    auto meat = std::make_shared<Meat>(enemy->GetPosition(), Vector2{(float)GetRandomValue(-100, 100), -200.0f});
                                    activeItems.push_back(meat);
                                    if (enemy->role != EnemyRole::BOSS) {
                                        auto drop = ItemDropManager::GetInstance()->TryDrop(enemy.get(), p, currentStage);
                                        if (drop) activeItems.push_back(drop);
                                    }
                                }
                            }
                        }
                    }
                }
            };

            // --- Player Logic ---
            // --- WEAPON SANDBOX CONTROLS & PLAYER LOGIC ---
            if (enableDevTools) {
                if (IsKeyPressed(KEY_F1)) debugSandboxMode = !debugSandboxMode; // Bật / tắt sandbox
                if (debugSandboxMode && player && player->IsActive()) {
                    if (IsKeyPressed(KEY_ONE))   player->SetWeapon("Hypergun");
                    if (IsKeyPressed(KEY_TWO))   player->SetWeapon("Plasma_Rifle");
                    if (IsKeyPressed(KEY_THREE)) player->SetWeapon("Absolver_Beam");
                    if (IsKeyPressed(KEY_FOUR))  player->SetWeapon("Neutron_Gun");
                    if (IsKeyPressed(KEY_FIVE))  player->SetWeapon("Riddler");
                    if (IsKeyPressed(KEY_SIX))   player->SetWeapon("Lightning_Fryer");
                    if (IsKeyPressed(KEY_SEVEN)) player->SetWeapon("Ion_Blaster");
                    if (IsKeyPressed(KEY_EIGHT)) player->SetWeapon("Utensil_Poker");
                    if (IsKeyPressed(KEY_NINE))  player->SetWeapon("Laser_Cannon");

                    if (IsKeyPressed(KEY_UP))    player->SetLevel(std::min(player->GetLevel() + 1, 11));
                    if (IsKeyPressed(KEY_DOWN))  player->SetLevel(std::max(player->GetLevel() - 1, 1));
                    
                    if (IsKeyPressed(KEY_H))     showDebugHitboxes = !showDebugHitboxes;
                    if (IsKeyDown(KEY_F))        deltaTime *= 0.1f; // Slow-motion
                }
            }

            if (player && player->IsActive()) {
                Vector2 pos = player->GetPosition();
                if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) pos.y -= player->GetMoveSpeed() * deltaTime;
                if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) pos.y += player->GetMoveSpeed() * deltaTime;
                if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) pos.x -= player->GetMoveSpeed() * deltaTime;
                if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) pos.x += player->GetMoveSpeed() * deltaTime;
                
                if (IsKeyPressed(KEY_M)) player->ActivateMana();
                
                if (enableDevTools) {
                    if (IsKeyPressed(KEY_L)) player->LevelUp();
                    if (IsKeyPressed(KEY_H) && !debugSandboxMode) showDebugHitboxes = !showDebugHitboxes;
                }
                
                if (pos.x < 0) pos.x = 0;
                if (pos.x > screenWidth) pos.x = screenWidth;
                if (pos.y < 0) pos.y = 0;
                if (pos.y > screenHeight) pos.y = screenHeight;
                player->SetPosition(pos);

                player->Update(deltaTime);
                
                std::string weapon = player->GetWeapon();
                bool isBeamWeapon = (weapon == "Lightning_Fryer" || weapon == "Plasma_Rifle" || weapon == "Laser_Cannon");
                bool isFiring = IsKeyDown(KEY_SPACE);
                
                if (!isBossCutscene && isBeamWeapon) {
                    SoundManager::GetInstance()->UpdateBeamSound(weapon, isFiring);
                } else {
                    SoundManager::GetInstance()->UpdateBeamSound("", false);
                }

                isAutoLocked = false;
                if (!isBossCutscene) {
                    ProcessBeamWeapon(player.get(), isFiring, autoLockTargetPos, beamTextTimer, isAutoLocked);
                }
            }

            if (player2 && player2->IsActive()) {
                if (currentGameMode == GameMode::TWO_PLAYERS) {
                    Vector2 mousePos = GetMousePosition();
                    player2->SetPosition(mousePos);
                    
                    if (!isBossCutscene) {
                        ProcessBeamWeapon(player2.get(), IsMouseButtonDown(MOUSE_LEFT_BUTTON), autoLockTargetPos2, beamTextTimer2, isAutoLocked2);
                    }
                    
                    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                        player2->ActivateMana();
                    }
                } else if (currentGameMode == GameMode::PLAYER_AND_AI && aiController) {
                    aiController->Update(player2.get(), deltaTime, activeBullets, activeEnemies, isBossCutscene, screenWidth, screenHeight);
                    if (!isBossCutscene) {
                        ProcessBeamWeapon(player2.get(), aiController->HasValidTarget(), autoLockTargetPos2, beamTextTimer2, isAutoLocked2);
                    }
                }
                player2->Update(deltaTime);
            }

            // Sync Health System (Shared HP)
            static float lastSharedHp = -1.0f;
            if (player && player2 && player->IsActive() && player2->IsActive()) {
                float hp1 = player->GetHp();
                float hp2 = player2->GetHp();
                if (lastSharedHp < 0.0f) lastSharedHp = hp1; // Khởi tạo ban đầu

                if (hp1 != hp2) {
                    if (hp1 != lastSharedHp && hp2 == lastSharedHp) {
                        player2->SetHp(hp1);
                        lastSharedHp = hp1;
                    } else if (hp2 != lastSharedHp && hp1 == lastSharedHp) {
                        player->SetHp(hp2);
                        lastSharedHp = hp2;
                    } else {
                        // Cả hai cùng đổi (hoặc lỗi logic), ưu tiên máu thấp hơn (bị sát thương)
                        float minHp = std::min(hp1, hp2);
                        player->SetHp(minHp);
                        player2->SetHp(minHp);
                        lastSharedHp = minHp;
                    }
                } else {
                    lastSharedHp = hp1;
                }
            } else {
                lastSharedHp = -1.0f; // Reset khi không có đủ 2 người chơi
            }

            // Update Cutscene Timer if active
            if (isBossCutscene) {
                cutsceneTimer += deltaTime;
                if (cutsceneTimer > 8.0f) {
                    isBossCutscene = false;
                }
            }

            // --- Wave Logic ---
            if (currentState == GameState::TEST_GAMEPLAY) {
                if (isWaveTransitioning) {
                    waveTimer -= deltaTime;
                    if (waveTimer <= 0.0f) {
                        isWaveTransitioning = false;
                        
                        if (currentStage == 7) {
                            // Bosses are already spawned during WAVE_SELECTION, so no need to spawn batches or enter stat selection.
                        } else {
                            // Cố gắng spawn batch hiện tại
                            bool batchSpawned = SpawnWaveBatch(currentWave, currentBatch);
                            
                            // Nếu spawn thất bại (hết batch của wave này)
                            if (!batchSpawned) {
                                EnterStatSelection(currentWave + 1);
                            }
                        }
                    }
                } else if (activeEnemies.empty() && !WaveManager::GetInstance()->IsContinuousStream()) {
                    if (currentStage == 7) {
                        if (activeItems.empty()) {
                            ChangeState(GameState::GAME_OVER); // Chiến thắng Stage 7
                        }
                    } else {
                        int maxBatch = WaveManager::GetInstance()->GetMaxBatchForWave(currentWave);
                        if (currentBatch < maxBatch) {
                            currentBatch++;
                            isWaveTransitioning = true;
                            waveTimer = 3.0f;
                        } else {
                            if (activeItems.empty()) {
                                if (currentWave < WaveManager::GetInstance()->GetTotalWaves()) {
                                    EnterStatSelection(currentWave + 1);
                                } else {
                                    ProgressManager::GetInstance()->UnlockStage(currentStage + 1);
                                    if (mainMenuUI) mainMenuUI->UpdateStageStatus();
                                    
                                    if (isTestMode) {
                                        if (player) {
                                            player->Heal(player->GetMaxHp());
                                            player->ClearAllBuffs();
                                        }
                                        if (player2) {
                                            player2->Heal(player2->GetMaxHp());
                                            player2->ClearAllBuffs();
                                        }
                                        ChangeState(GameState::MAIN_MENU);
                                    } else {
                                        EnterSummary(SummaryResult::WIN);
                                    }
                                }
                            }
                        }
                    }
                }
            } else if (currentState == GameState::TEST_ENEMY) {
                // Spawner đơn giản cho TEST_ENEMY (để người chơi test bắn gà)
                if (activeEnemies.empty()) {
                    float x = GetRandomValue(200, screenWidth - 200);
                    auto enemy = EnemyFactory::CreateEnemy(1, EnemyRole::NORMAL, EnemyStats(), {x, 100.0f});
                    enemy->SetMovementBehavior(std::make_unique<SineZigzagMovement>());
                    enemy->ResetEggTimer();
                    AddEnemy(std::move(enemy));
                }
            }

            // --- Update Entities ---
            for (auto& item : activeItems) {
                if (item->IsActive()) item->Update(deltaTime);
            }
            for (auto& enemy : activeEnemies) {
                if (enemy->IsActive()) enemy->Update(deltaTime);
            }
            for (auto& bullet : activeBullets) {
                if (bullet->IsActive()) bullet->Update(deltaTime);
            }

            // --- Collision Detection (áp dụng cho cả GAMEPLAY và TEST_ENEMY) ---
            if (currentState == GameState::TEST_GAMEPLAY || currentState == GameState::TEST_ENEMY) {
                for (auto& bullet : activeBullets) {
                    if (!bullet->IsActive()) continue;
                    if (bullet->IsPlayerBullet()) {
                        for (auto& enemy : activeEnemies) {
                            if (!enemy || !enemy->IsActive()) continue;
                            
                            Vector2 currCenter = bullet->GetCenter();
                            float radius = bullet->GetRadius();
                            Rectangle enemyBox = enemy->GetHitbox();
                            
                            // Prevent destroying enemies before they even enter the screen
                            if (enemyBox.y + enemyBox.height < 0) continue;

                            
                            // 1. Kiểm tra va chạm hình tròn tĩnh cơ bản
                            bool hit = CheckCollisionCircleRec(currCenter, radius, enemyBox);
                            
                            // 2. Continuous Collision Detection (CCD) - Xử lý triệt để đạn tốc độ cao trôi thẳng xuyên qua gà (Tunneling Fix)
                            if (!hit) {
                                Vector2 prevCenter = bullet->GetPrevCenter();
                                Vector2 eCenter = { enemyBox.x + enemyBox.width / 2.0f, enemyBox.y + enemyBox.height / 2.0f };
                                float eRad = std::max(enemyBox.width, enemyBox.height) / 2.0f + radius;
                                
                                Vector2 v = { currCenter.x - prevCenter.x, currCenter.y - prevCenter.y };
                                float vLenSq = v.x * v.x + v.y * v.y;
                                if (vLenSq > 0.0f) {
                                    Vector2 w = { eCenter.x - prevCenter.x, eCenter.y - prevCenter.y };
                                    float t = std::max(0.0f, std::min(1.0f, (w.x * v.x + w.y * v.y) / vLenSq));
                                    Vector2 closest = { prevCenter.x + t * v.x, prevCenter.y + t * v.y };
                                    float dx = eCenter.x - closest.x;
                                    float dy = eCenter.y - closest.y;
                                    if ((dx * dx + dy * dy) <= (eRad * eRad)) {
                                        hit = true;
                                    }
                                }
                            }
                            
                            if (hit) {
                                Spaceship* shooter = bullet->GetShooter() ? bullet->GetShooter() : player.get();
                                bool isCrit = (GetRandomValue(0, 100) < shooter->GetCritChance());
                                float finalDamage = bullet->GetDamage();
                                if (isCrit) {
                                    finalDamage *= (shooter->GetCritDamage() / 100.0f);
                                }
                                if (shooter->HasArgument(3) && enemy->role == EnemyRole::BOSS) { // 3: Boss Hunter
                                    finalDamage *= 1.8f;
                                }
                                if (shooter->HasArgument(4)) { // 4: Armor Crusher
                                    finalDamage += enemy->GetHp() * 0.03f;
                                }
                                bullet->SetActive(false);
                                enemy->TakeDamage(finalDamage);
                                
                                float offsetX = (float)GetRandomValue(-20, 20) + 20.0f;
                                float lifetime = isCrit ? 0.7f : 0.45f;
                                activeDamageTexts.push_back({{enemy->GetPosition().x + offsetX, enemy->GetPosition().y}, 
                                                            (int)finalDamage, isCrit, lifetime, lifetime});
                                if (!enemy->IsActive()) {
                                    WaveManager::GetInstance()->AddKill();
                                    AddScore(enemy->GetPointValue());
                                    shooter->GainExp(enemy->GetPointValue() * 0.5f);
                                    int coin = CoinManager::GetInstance()->GetKillCoin(enemy.get());
                                    Notify(EventType::ENEMY_DIED, std::to_string(coin));
                                    // Blood Fury (Arg 5)
                                    if (shooter->HasArgument(5)) shooter->AddPermanentDamage(2.0f);
                                    // Bloodthirst (Arg 6)
                                    if (shooter->HasArgument(6)) shooter->Heal(shooter->GetMaxHp() * 0.05f);
                                    
                                    PlayExplosionSound();
                                    auto meat = std::make_shared<Meat>(enemy->GetPosition(),
                                        Vector2{(float)GetRandomValue(-100, 100), -200.0f});
                                    activeItems.push_back(meat);

                                    if (enemy->role != EnemyRole::BOSS) {
                                        auto drop = ItemDropManager::GetInstance()->TryDrop(
                                            enemy.get(), shooter, currentStage);
                                        if (drop) activeItems.push_back(drop);
                                    }
                                }
                                break; // Chống một viên đạn ăn trúng 2 mục tiêu cùng lúc
                            }
                        }
                    } else {
                        if (player && player->IsActive()) {
                            if (CheckCollisionCircleRec(bullet->GetCenter(), bullet->GetRadius(), player->GetHitbox())) {
                                bullet->SetActive(false);
                                player->TakeDamage(bullet->GetDamage());
                            }
                        }
                        if (player2 && player2->IsActive()) {
                            if (CheckCollisionCircleRec(bullet->GetCenter(), bullet->GetRadius(), player2->GetHitbox())) {
                                bullet->SetActive(false);
                                player2->TakeDamage(bullet->GetDamage());
                            }
                        }
                    }
                }

                // Va chạm trực tiếp quái và Player
                for (auto& enemy : activeEnemies) {
                    if (!enemy->IsActive()) continue;
                    if (player && player->IsActive() && CheckCollisionRecs(player->GetHitbox(), enemy->GetHitbox())) {
                        enemy->TakeDamage(100);
                        player->TakeDamage(enemy->role == EnemyRole::ASTEROID ? 50 : 20);
                    }
                    if (player2 && player2->IsActive() && CheckCollisionRecs(player2->GetHitbox(), enemy->GetHitbox())) {
                        enemy->TakeDamage(100);
                        player2->TakeDamage(enemy->role == EnemyRole::ASTEROID ? 50 : 20);
                    }
                }

                // Va chạm Item và Player
                for (auto& item : activeItems) {
                    if (!item->IsActive()) continue;
                    Rectangle itemRect = {item->GetPosition().x - 20, item->GetPosition().y - 20, 40, 40};
                    bool pickedUp = false;
                    Spaceship* whoPicked = nullptr;
                    if (player && player->IsActive() && CheckCollisionRecs(itemRect, player->GetHitbox())) {
                        pickedUp = true; whoPicked = player.get();
                    } else if (player2 && player2->IsActive() && CheckCollisionRecs(itemRect, player2->GetHitbox())) {
                        pickedUp = true; whoPicked = player2.get();
                    }
                    
                    if (pickedUp && whoPicked) {
                        item->SetActive(false);
                        if (item->GetType() == ItemType::DRUMSTICK) {
                            AddScore(50);
                            whoPicked->Heal(50.0f);
                            whoPicked->GainExp(10.0f);
                            PlayPickupSound();
                        } else if (item->GetType() == ItemType::SWORD ||
                                   item->GetType() == ItemType::SHIELD ||
                                   item->GetType() == ItemType::HEART_POWERUP) {
                            auto* powerUp = dynamic_cast<PowerUpItem*>(item.get());
                            if (powerUp) powerUp->OnPickup(whoPicked);
                        }
                    }
                }

                // Game Over Check & Revive
                if (player && player->GetHp() <= 0) {
                    RuneManager::GetInstance()->TryRevive(player.get(), false);
                }
                if (player2 && player2->GetHp() <= 0) {
                    RuneManager::GetInstance()->TryRevive(player2.get(), true);
                }

                bool p1Dead = !player || player->GetHp() <= 0;
                bool p2Dead = (currentGameMode != GameMode::SINGLE_PLAYER) ? (!player2 || player2->GetHp() <= 0) : true;
                
                if (p1Dead && p2Dead) {
                    if (player) player->ClearAllBuffs();
                    if (player2) player2->ClearAllBuffs();
                    
                    if (isTestMode) {
                        currentState = GameState::GAME_OVER;
                    } else {
                        EnterSummary(SummaryResult::LOSE);
                    }
                }
            }

            // --- Cleanup Inactive Entities ---
            activeBullets.erase(std::remove_if(activeBullets.begin(), activeBullets.end(),
                [](const std::shared_ptr<Bullet>& b) { return !b->IsActive(); }), activeBullets.end());
            activeEnemies.erase(std::remove_if(activeEnemies.begin(), activeEnemies.end(),
                [](const std::shared_ptr<Enemy>& e) { return !e->IsActive(); }), activeEnemies.end());
            activeItems.erase(std::remove_if(activeItems.begin(), activeItems.end(),
                [](const std::shared_ptr<Item>& i) { return !i->IsActive(); }), activeItems.end());
            break;
        }
        
        case GameState::SUMMARY:
        {
            if (summaryScreen) {
                summaryScreen->Update();
                if (summaryScreen->IsBackRequested()) {
                    if (player) {
                        player->ResetToBaseStats();
                        player->ClearAllBuffs();
                    }
                    if (player2) {
                        player2->ResetToBaseStats();
                        player2->ClearAllBuffs();
                    }
                    RuneManager::GetInstance()->ResetForNewStage();
                    ChangeState(GameState::MAIN_MENU);
                }
            }
            break;
        }

        // --- Xử lý màn hình chọn Chỉ Số ---
        case GameState::STAT_SELECTION:
        {
            selectionAnimTimer += deltaTime;
            if (selectionAnimTimer > 0.4f) selectionAnimTimer = 0.4f;

            if (currentNumChoices == 0) {
                if (extraStatSelectionsPending > 0) {
                    extraStatSelectionsPending--;
                    GenerateSelectionPool(true);
                    isStatSelection = true;
                    selectionAnimTimer = 0.0f;
                    ChangeState(GameState::STAT_SELECTION);
                } else if (pendingArgumentAfterStat) {
                    isStatSelection = false;
                    selectionAnimTimer = 0.0f;
                    GenerateSelectionPool(false);
                    ChangeState(GameState::ARGUMENT_SELECTION);
                } else {
                    StartWave(nextWaveAfterSelection);
                }
                break;
            }

            int numChoices = (player && player->HasArgument(2)) ? 4 : 3;
            const float CARD_W   = 320.0f;
            const float CARD_H   = 490.0f;
            const float GAP      = 40.0f;
            const float totalW   = numChoices * CARD_W + (numChoices - 1) * GAP;
            const float startX   = (screenWidth  - totalW) / 2.0f;
            const float finalY   = (screenHeight - CARD_H) / 2.0f;
            float t = selectionAnimTimer / 0.4f;
            t = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
            float cardY = finalY + (1.0f - t) * 350.0f;

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                for (int i = 0; i < currentNumChoices; i++) {
                    Rectangle cardRect = { startX + i * (CARD_W + GAP), cardY, CARD_W, CARD_H };
                    if (CheckCollisionPointRec(GetMousePosition(), cardRect)) {
                        // Áp dụng chỉ số
                        int statIdx = shownCardIndices[i];
                        if (player) {
                            switch (statIdx) {
                                case 0: player->AddPermanentMaxHp(30.0f); if (player2) player2->AddPermanentMaxHp(30.0f); break;
                                case 1: player->AddPermanentDamage(5.0f); if (player2) player2->AddPermanentDamage(5.0f); break;
                                case 2: player->AddPermanentArmor(3.0f); if (player2) player2->AddPermanentArmor(3.0f); break;
                                case 3: player->AddPermanentFireRate(0.10f); if (player2) player2->AddPermanentFireRate(0.10f); break;
                                case 4: player->AddPermanentCritChance(10.0f); if (player2) player2->AddPermanentCritChance(10.0f); break;
                                case 5: player->AddPermanentCritDamage(30.0f); if (player2) player2->AddPermanentCritDamage(30.0f); break;
                            }
                        }
                        if (extraStatSelectionsPending > 0) {
                            extraStatSelectionsPending--;
                            GenerateSelectionPool(true);
                            isStatSelection = true;
                            selectionAnimTimer = 0.0f;
                            ChangeState(GameState::STAT_SELECTION);
                        } else if (pendingArgumentAfterStat) {
                            isStatSelection = false;
                            selectionAnimTimer = 0.0f;
                            GenerateSelectionPool(false);
                            ChangeState(GameState::ARGUMENT_SELECTION);
                        } else {
                            StartWave(nextWaveAfterSelection);
                        }
                        break;
                    }
                }
            }
            break;
        }

        // --- Xử lý màn hình chọn Argument ---
        case GameState::ARGUMENT_SELECTION:
        {
            selectionAnimTimer += deltaTime;
            if (selectionAnimTimer > 0.4f) selectionAnimTimer = 0.4f;

            if (currentNumChoices == 0) {
                StartWave(nextWaveAfterSelection);
                break;
            }

            int numChoices = (player && player->HasArgument(2)) ? 4 : 3;
            const float CARD_W = 320.0f;
            const float CARD_H = 490.0f;
            const float GAP    = 40.0f;
            const float totalW = numChoices * CARD_W + (numChoices - 1) * GAP;
            const float startX = (screenWidth  - totalW) / 2.0f;
            const float finalY = (screenHeight - CARD_H) / 2.0f;
            float t = selectionAnimTimer / 0.4f;
            t = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
            float cardY = finalY + (1.0f - t) * 350.0f;

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                for (int i = 0; i < currentNumChoices; i++) {
                    Rectangle cardRect = { startX + i * (CARD_W + GAP), cardY, CARD_W, CARD_H };
                    if (CheckCollisionPointRec(GetMousePosition(), cardRect)) {
                        int argId = shownCardIndices[i];
                        if (player) {
                            player->AddArgument(argId);
                            if (player2) player2->AddArgument(argId);
                            if (argId == 9) { // Fast Track
                                player->LevelUp();
                                player->LevelUp();
                                player->LevelUp();
                                if (player2) {
                                    player2->LevelUp();
                                    player2->LevelUp();
                                    player2->LevelUp();
                                }
                            } else if (argId == 1) { // Stat Windfall
                                extraStatSelectionsPending += 3;
                            }
                        }
                        
                        if (extraStatSelectionsPending > 0) {
                            extraStatSelectionsPending--;
                            GenerateSelectionPool(true);
                            isStatSelection = true;
                            pendingArgumentAfterStat = false;
                            selectionAnimTimer = 0.0f;
                            ChangeState(GameState::STAT_SELECTION);
                        } else {
                            StartWave(nextWaveAfterSelection);
                        }
                        break;
                    }
                }
            }
            break;
        }

        default:
            break;
    }

    // Flush pending entities (prevent iterator invalidation)
    if (!pendingBullets.empty()) {
        activeBullets.insert(activeBullets.end(), pendingBullets.begin(), pendingBullets.end());
        pendingBullets.clear();
    }
    if (!pendingEnemies.empty()) {
        activeEnemies.insert(activeEnemies.end(), pendingEnemies.begin(), pendingEnemies.end());
        pendingEnemies.clear();
    }
    if (!pendingItems.empty()) {
        activeItems.insert(activeItems.end(), pendingItems.begin(), pendingItems.end());
        pendingItems.clear();
    }
}

void GameManager::Draw() {
    BeginDrawing();
    
    ClearBackground(BLACK);
    
    // Draw scrolling background (Mirrored seamless)
    float texW = (float)texBackgrounds[currentBgIndex].width;
    float texH = (float)texBackgrounds[currentBgIndex].height;

    // Tile 0 (Normal)
    DrawTexturePro(texBackgrounds[currentBgIndex], {0, 0, texW, texH}, 
                   {0, bgY, (float)screenWidth, (float)screenHeight}, {0,0}, 0.0f, WHITE);
    
    // Tile 1 (Flipped vertically - âm texH để lật ngược hình, giúp liền mạch)
    DrawTexturePro(texBackgrounds[currentBgIndex], {0, 0, texW, -texH}, 
                   {0, bgY - screenHeight, (float)screenWidth, (float)screenHeight}, {0,0}, 0.0f, WHITE);
                   
    // Tile 2 (Normal)
    DrawTexturePro(texBackgrounds[currentBgIndex], {0, 0, texW, texH}, 
                   {0, bgY - 2.0f * screenHeight, (float)screenWidth, (float)screenHeight}, {0,0}, 0.0f, WHITE);

    switch (currentState) {
        case GameState::SHOP:
            if (shopUI) shopUI->Draw();
            break;
            
        case GameState::TUTORIAL:
            if (tutorialUI) tutorialUI->Draw();
            break;

        case GameState::RUNE_SELECTION:
        {
            if (runeUI) runeUI->Draw();
            break;
        }
            
        case GameState::TITLE_SCREEN:
        {
            if (titleScreen) {
                TitleAction action = titleScreen->UpdateAndDraw(GetFrameTime());
                if (action == TitleAction::CONTINUE) {
                    titleScreen->Reset();
                    ChangeState(GameState::MAIN_MENU);
                } else if (action == TitleAction::NEW_GAME) {
                    CoinManager::GetInstance()->ResetAllProgress();
                    ProgressManager::GetInstance()->ResetAllProgress();
                    ShopManager::GetInstance()->ResetAllProgress();
                    if (mainMenuUI) mainMenuUI->UpdateStageStatus();
                    titleScreen->Reset();
                    ChangeState(GameState::MAIN_MENU);
                }
            }
            break;
        }
        
        case GameState::MAIN_MENU:
        {
            if (mainMenuUI) {
                int clickedId = mainMenuUI->UpdateAndDraw(GetFrameTime());
                if (clickedId == 100) {
                    ChangeState(GameState::SHOP);
                } else if (clickedId == 101) {
                    previousState = currentState;
                    ChangeState(GameState::SETTINGS);
                } else if (clickedId == 102) {
                    tutorialUI->Init();
                    ChangeState(GameState::TUTORIAL);
                } else if (clickedId >= 1 && clickedId <= 7) {
                    pendingStageFromMenu = clickedId;
                    ChangeState(GameState::PLAYER_SELECT);
                }
            }
            break;
        }

        case GameState::PLAYER_SELECT:
        {
            FontManager::GetInstance()->DrawGameTextCentered("CHOOSE GAME MODE", screenWidth/2, 100, 40, DARKBLUE, "Modern");

            Rectangle singleBtn = { (float)screenWidth/2 - 350, (float)screenHeight/2 - 50, 200, 100 };
            Rectangle doubleBtn = { (float)screenWidth/2 - 100, (float)screenHeight/2 - 50, 200, 100 };
            Rectangle aiBtn     = { (float)screenWidth/2 + 150, (float)screenHeight/2 - 50, 200, 100 };
            Rectangle backBtn   = { (float)screenWidth/2 - 100, (float)screenHeight - 150, 200, 50 };

            if (DrawButton(singleBtn, "1 PLAYER")) {
                currentGameMode = GameMode::SINGLE_PLAYER;
                RuneManager::GetInstance()->ResetForNewStage();
                ChangeState(GameState::RUNE_SELECTION);
            }
            if (DrawButton(doubleBtn, "2 PLAYERS")) {
                currentGameMode = GameMode::TWO_PLAYERS;
                RuneManager::GetInstance()->ResetForNewStage();
                ChangeState(GameState::RUNE_SELECTION);
            }
            if (DrawButton(aiBtn, "1P + AI")) {
                currentGameMode = GameMode::PLAYER_AND_AI;
                RuneManager::GetInstance()->ResetForNewStage();
                ChangeState(GameState::RUNE_SELECTION);
            }
            if (DrawButton(backBtn, "BACK")) {
                ChangeState(GameState::MAIN_MENU);
            }
            break;
        }

        
        case GameState::SETTINGS:
        {
            FontManager::GetInstance()->DrawGameTextCentered("SETTINGS", screenWidth/2, 100, 40, DARKBLUE, "Modern");
            FontManager::GetInstance()->DrawGameText("Backgrounds:", screenWidth/2 - 350, 200, 25, GRAY, "Modern");
            
            if (DrawButton({(float)screenWidth/2 - 350, 250, 150, 50}, "Default")) {
                currentBgIndex = 0;
                SoundManager::GetInstance()->PlayBeep();
            }
            if (DrawButton({(float)screenWidth/2 - 150, 250, 150, 50}, "Bg 1")) {
                currentBgIndex = 1;
                SoundManager::GetInstance()->PlayBeep();
            }
            if (DrawButton({(float)screenWidth/2 + 50, 250, 150, 50}, "Bg 2")) {
                currentBgIndex = 2;
                SoundManager::GetInstance()->PlayBeep();
            }
            if (DrawButton({(float)screenWidth/2 + 250, 250, 150, 50}, "Bg 3")) {
                currentBgIndex = 3;
                SoundManager::GetInstance()->PlayBeep();
            }
            
            // Toggles for Sound and Music
            FontManager::GetInstance()->DrawGameText("Audio:", screenWidth/2 - 350, 350, 25, GRAY, "Modern");
            bool soundOn = SoundManager::GetInstance()->IsSoundOn();
            std::string soundText = soundOn ? "Sound: ON" : "Sound: OFF";
            if (DrawButton({(float)screenWidth/2 - 350, 400, 200, 50}, soundText.c_str())) {
                SoundManager::GetInstance()->ToggleSound(!soundOn);
                SoundManager::GetInstance()->PlayBeep();
            }
            
            bool musicOn = SoundManager::GetInstance()->IsMusicOn();
            std::string musicText = musicOn ? "Music: ON" : "Music: OFF";
            if (DrawButton({(float)screenWidth/2 - 100, 400, 200, 50}, musicText.c_str())) {
                SoundManager::GetInstance()->ToggleMusic(!musicOn);
                SoundManager::GetInstance()->PlayBeep();
            }

            if (DrawButton({(float)screenWidth/2 - 100, 550, 200, 50}, "BACK")) {
                SoundManager::GetInstance()->PlayBeep();
                ChangeState(previousState);
            }
            
            // Vẽ border đỏ bao quanh nút đang chọn
            Rectangle selectedRect;
            if (currentBgIndex == 0) selectedRect = {(float)screenWidth/2 - 350, 250, 150, 50};
            else if (currentBgIndex == 1) selectedRect = {(float)screenWidth/2 - 150, 250, 150, 50};
            else if (currentBgIndex == 2) selectedRect = {(float)screenWidth/2 + 50, 250, 150, 50};
            else if (currentBgIndex == 3) selectedRect = {(float)screenWidth/2 + 250, 250, 150, 50};
            
            DrawRectangleLinesEx(selectedRect, 3, RED);
            break;
        }
        
        case GameState::COMING_SOON:
        {
            FontManager::GetInstance()->DrawGameTextCentered("PLAY MODE - COMING SOON!", screenWidth/2, screenHeight/2, 30, DARKGRAY, "Modern");
            if (DrawButton({(float)screenWidth/2 - 100, (float)screenHeight/2 + 80, 200, 50}, "BACK")) {
                ChangeState(GameState::MAIN_MENU);
            }
            break;
        }

        case GameState::GAME_OVER:
        {
            FontManager::GetInstance()->DrawGameTextCentered("GAME OVER!", screenWidth/2, 200, 40, RED, "Retro");
            FontManager::GetInstance()->DrawGameTextCentered(TextFormat("FINAL SCORE: %d", score), screenWidth/2, 260, 25, DARKGRAY, "Retro");
            
            FontManager::GetInstance()->DrawGameTextCentered(TextFormat("COINS EARNED THIS RUN: %d + %d (stage bonus) = %d",
                CoinManager::GetInstance()->GetSessionCoins(),
                CoinManager::GetInstance()->GetStageBonusCoins(),
                CoinManager::GetInstance()->GetSessionCoins() + CoinManager::GetInstance()->GetStageBonusCoins()),
                screenWidth/2 - 270, 320, 20, GOLD);
            FontManager::GetInstance()->DrawGameTextCentered(TextFormat("TOTAL COINS: %d", CoinManager::GetInstance()->GetTotalCoins()),
                screenWidth/2 - 120, 360, 22, YELLOW);
            DrawTextureEx(texCoin, {(float)screenWidth/2 - 160, 355}, 0.0f, 0.5f, WHITE);
            
            if (DrawButton({(float)screenWidth/2 - 120, 420, 240, 50}, "BACK TO MENU")) {
                RuneManager::GetInstance()->ResetForNewStage();
                ChangeState(GameState::MAIN_MENU);
                // Reset Game
                score = 0;
                activeEnemies.clear();
                activeBullets.clear();
                activeItems.clear();
                if (player) player->ResetToBaseStats();
                if (player2) player2->ResetToBaseStats();
            }
            break;
        }

        case GameState::TEST_MENU:
        {
            FontManager::GetInstance()->DrawGameTextCentered("TEST MODE SELECTION", screenWidth/2, 150, 30, DARKBLUE, "Modern");
            
            if (DrawButton({(float)screenWidth/2 - 150, 250, 300, 50}, "TEST ENEMY")) {
                ChangeState(GameState::TEST_ENEMY);
            }
            
            if (DrawButton({(float)screenWidth/2 - 320, 320, 300, 50}, "TEST SPACESHIP")) {
                ChangeState(GameState::TEST_SPACESHIP);
                if (player) player->SetShootingBehavior(std::make_unique<HypergunShootingBehavior>());
            }
            
            if (DrawButton({ (float)screenWidth/2 + 20, 320, 300, 50 }, "TEST GAMEPLAY")) {
                ChangeState(GameState::WAVE_SELECTION);
            }
            
            if (DrawButton({(float)screenWidth/2 - 150, 460, 300, 50}, "BACK TO MAIN")) {
                ChangeState(GameState::MAIN_MENU);
            }
            
            // Nút Settings (góc trên phải) trong TEST_MENU
            Rectangle settingRect = {(float)screenWidth - 80, 20, 60, 60};
            DrawTexturePro(texSettingIcon, {0, 0, (float)texSettingIcon.width, (float)texSettingIcon.height}, settingRect, {0,0}, 0.0f, WHITE);
            if (CheckCollisionPointRec(GetMousePosition(), settingRect)) {
                DrawRectangleLinesEx(settingRect, 2, RED);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    previousState = currentState;
                    ChangeState(GameState::SETTINGS);
                }
            }
            
            break;
        }

        case GameState::WAVE_SELECTION: {
            FontManager::GetInstance()->DrawGameTextCentered("TEST STAGE & WAVE", screenWidth/2, 120, 40, YELLOW, "Modern");
            
            // Fix Out-of-Bounds Issue: Show warning if trying to start non-existent waves
            bool isValidSelection = (testConfig.stage == 1 && testConfig.wave <= 10) || (testConfig.stage == 2 && testConfig.wave <= 10) || (testConfig.stage == 3 && testConfig.wave <= 1) || (testConfig.stage == 4 && testConfig.wave <= 10) || (testConfig.stage == 5 && testConfig.wave <= 15) || (testConfig.stage == 6 && testConfig.wave <= 5) || (testConfig.stage == 7 && testConfig.wave == 1 && testConfig.batch == 1);
            if (!isValidSelection) {
                FontManager::GetInstance()->DrawGameTextCentered("WARNING: WAVE NOT YET IMPLEMENTED", screenWidth/2, 180, 20, RED, "Modern");
            }

            // Buttons to select Stage
            FontManager::GetInstance()->DrawGameText(TextFormat("STAGE: %d", testConfig.stage), screenWidth/2 - 70, 220, 30, WHITE, "Modern");
            if (DrawButton({ (float)screenWidth/2 - 150, 210, 50, 50 }, "<")) {
                if (testConfig.stage > 1) { testConfig.stage--; testConfig.wave = 1; testConfig.batch = 1; }
            }
            if (DrawButton({ (float)screenWidth/2 + 100, 210, 50, 50 }, ">")) {
                if (testConfig.stage < testConfig.maxStage) { testConfig.stage++; testConfig.wave = 1; testConfig.batch = 1; }
            }

            // Buttons to select Wave
            FontManager::GetInstance()->DrawGameText(TextFormat("WAVE: %d", testConfig.wave), screenWidth/2 - 70, 320, 30, WHITE, "Modern");
            if (DrawButton({ (float)screenWidth/2 - 150, 310, 50, 50 }, "<")) {
                if (testConfig.wave > 1) { testConfig.wave--; testConfig.batch = 1; }
            }
            int currentMaxWave = (testConfig.stage == 7) ? 1 : ((testConfig.stage == 5) ? 15 : ((testConfig.stage == 6) ? 5 : testConfig.maxWave));
            if (DrawButton({ (float)screenWidth/2 + 100, 310, 50, 50 }, ">")) {
                if (testConfig.wave < currentMaxWave) { testConfig.wave++; testConfig.batch = 1; }
            }

            if (DrawButton({ (float)screenWidth/2 - 100, 520, 200, 50 }, "FIGHT BOSS")) {
                testConfig.stage = 7;
                testConfig.wave = 1;
                testConfig.batch = 1;
                isTestMode = true;
                ChangeState(GameState::TEST_GAMEPLAY);
            }
            
            // Select Batch
            FontManager::GetInstance()->DrawGameText(TextFormat("BATCH: %d", testConfig.batch), screenWidth/2 - 70, 420, 30, WHITE, "Modern");
            if (DrawButton({ (float)screenWidth/2 - 150, 410, 50, 50 }, "<")) {
                if (testConfig.batch > 1) testConfig.batch--;
            }
            int currentMaxBatch = (testConfig.stage == 7) ? 1 : testConfig.maxBatch;
            if (DrawButton({ (float)screenWidth/2 + 100, 410, 50, 50 }, ">")) {
                if (testConfig.batch < currentMaxBatch) testConfig.batch++;
            }

            if (DrawButton({ (float)screenWidth/2 - 100, 520, 200, 50 }, "START TEST")) {
                // Prevent starting broken waves
                if (!isValidSelection) return; 

                currentStage = testConfig.stage;
                currentWave = testConfig.wave;
                currentBatch = testConfig.batch;
                isWaveTransitioning = true;
                if (currentStage == 7) {
                    activeEnemies.clear();
                    activeBullets.clear();
                    
                    // Khởi tạo trạng thái Cutscene
                    isBossCutscene = true;
                    cutsceneTimer = 0.0f;
                    
                    // Khởi tạo Boss ở vị trí phía trên cùng ngoài màn hình để chuẩn bị bay xuống
                    auto mcb = std::make_shared<MilitaryChickenBoss>(12, EnemyStats{150000.0f, 50.0f, 20.0f, 150.0f, 0.0f, 100}, Vector2{(float)screenWidth/2 - 150, -200.0f});
                    auto scb = std::make_shared<SuperChickBoss>(13, EnemyStats{150000.0f, 50.0f, 20.0f, 150.0f, 0.0f, 100}, Vector2{(float)screenWidth/2 + 150, -200.0f});
                    activeEnemies.push_back(mcb);
                    activeEnemies.push_back(scb);
                } else {
                    WaveManager::GetInstance()->LoadStage("data/stage" + std::to_string(currentStage) + ".json");
                }
                waveTimer = 3.0f; 
                if (currentStage != 7) {
                    activeEnemies.clear();
                    activeBullets.clear();
                }
                activeItems.clear();
                extraStatSelectionsPending = 0;
                pendingArgumentAfterStat = false;
                score = 0;
                CoinManager::GetInstance()->ResetSession();
                
                // Initialize player for testing
                if (player) {
                    player = SpaceshipFactory::CreateSpaceship("Hypergun", 1, {(float)screenWidth/2, (float)screenHeight - 100});
                    player->SetShootingBehavior(std::make_unique<HypergunShootingBehavior>());
                    if (currentStage == 7) {
                        player->MultiplyHp(3.0f);
                    }
                }
                
                if (IsKeyPressed(KEY_L)) {
                    if (player) {
                        player->LevelUp();
                        if (player->GetLevel() > 11) {
                            // Capping at 11 for Hypergun
                            // We can just keep it or reset it
                        }
                    }
                }

                if (currentStage == 6 && currentWave == 1) {
                    EnterStatSelection(1);
                } else {
                    isTestMode = true;
                    ChangeState(GameState::TEST_GAMEPLAY);
                }
            }
            if (DrawButton({ (float)screenWidth/2 - 100, 600, 200, 50 }, "BACK")) {
                ChangeState(GameState::TEST_MENU);
            }
            break;
        }

        case GameState::WAVE_INTRO:
        {
            float totalDur2 = WARP_DURATION + TEXT_SHOW_DURATION + TEXT_FADE_DURATION;
            float wElapsed2 = totalDur2 - waveIntroTimer;
            float overlayAlpha2 = 0.0f;
            
            if (wElapsed2 < WARP_DURATION) {
                overlayAlpha2 = (wElapsed2 / WARP_DURATION) * 80.0f;
            } else if (wElapsed2 < WARP_DURATION + TEXT_SHOW_DURATION) {
                float pct2 = (wElapsed2 - WARP_DURATION) / TEXT_SHOW_DURATION;
                overlayAlpha2 = (1.0f - pct2) * 80.0f;
            }
            DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, (unsigned char)overlayAlpha2});
            
            // Vẽ tàu của người chơi
            if (player && player->IsActive()) {
                player->Draw();
            }
            if (player2 && player2->IsActive()) {
                player2->Draw();
            }

            if (waveTextAlpha > 1.0f) {
                unsigned char wAlpha = (unsigned char)waveTextAlpha;
                char waveStr[32];
                snprintf(waveStr, sizeof(waveStr), "WAVE %d", currentWave);
                int wFontSize = 90;
                
                int wTw = FontManager::GetInstance()->MeasureGameText(waveStr, wFontSize, "Retro").x;
                int wCx = screenWidth/2 - wTw/2;
                int wCy = screenHeight/2 - wFontSize/2;
                
                FontManager::GetInstance()->DrawGameText(waveStr, wCx + 4, wCy + 4, wFontSize, {0, 0, 0, (unsigned char)((int)wAlpha / 2)}, "Retro");
                
                int wTwG = FontManager::GetInstance()->MeasureGameText(waveStr, wFontSize + 6, "Retro").x;
                FontManager::GetInstance()->DrawGameText(waveStr, screenWidth/2 - wTwG/2, wCy - 3, wFontSize + 6, {80, 220, 255, (unsigned char)((int)wAlpha * 3 / 10)}, "Retro");
                FontManager::GetInstance()->DrawGameText(waveStr, wCx, wCy, wFontSize, {255, 255, 255, wAlpha}, "Retro");
            }
            break;
        }

        case GameState::TEST_GAMEPLAY:
        case GameState::TEST_ENEMY:
        case GameState::TEST_SPACESHIP:
        {
            // Draw Bullets FIRST (so they render UNDER enemies)

            // Vẽ Bullets
            for (const auto& bullet : activeBullets) {
                if (bullet && bullet->IsActive()) {
                    bullet->Draw();
                }
            }

            // --- VẼ HIỆU ỨNG ĐỘNG ĐẠN TIA (ADVANCED BEAM & LASER VFX ENGINE) ---
            auto DrawBeamVFX = [&](Spaceship* p, bool isFiring, const Vector2& lockTargetPos) {
                if (!p || !p->IsActive() || !isFiring) return;
                std::string weapon = p->GetWeapon();
                if (weapon != "Lightning_Fryer" && weapon != "Plasma_Rifle" && weapon != "Laser_Cannon") return;

                Vector2 origin = { p->GetPosition().x, p->GetPosition().y - 25.0f };
                Vector2 endPos = lockTargetPos;
                Vector2 diff = { endPos.x - origin.x, endPos.y - origin.y };
                float len = sqrt(diff.x * diff.x + diff.y * diff.y);
                float angleDeg = atan2(diff.y, diff.x) * 180.0f / 3.14159265f - 90.0f;
                Vector2 dir = (len > 0.0f) ? Vector2{ diff.x / len, diff.y / len } : Vector2{ 0.0f, -1.0f };
                float timeVal = (float)GetTime();
                int level = p->GetLevel();

                // Bật Blend Mode Additive để các dòng năng lượng phát vầng sáng thực tế
                BeginBlendMode(BLEND_ADDITIVE);

                if (weapon == "Lightning_Fryer") {
                    // 1. LIGHTNING FRYER: Dòng hồ quang điện cuộn UV tốc độ cao liên tục (Real-Time UV Scrolling)
                    if (texLightningFryer.id != 0) {
                        float scrollSpeed1 = 850.0f;
                        float scrollSpeed2 = 1350.0f;
                        float texRatio = (float)texLightningFryer.height > 0 ? (len / (float)texLightningFryer.height) * 1.5f : 2.0f;

                        float rotDeg = atan2(diff.y, diff.x) * 180.0f / 3.14159265f + 90.0f;

                        float baseWidth = 32.0f + level * 4.0f;
                        float width1 = baseWidth + sin(timeVal * 42.0f) * 8.0f;
                        float width2 = (baseWidth * 0.75f) + cos(timeVal * 58.0f) * 6.0f;

                        float uOffsetY1 = fmod(timeVal * (scrollSpeed1 / (float)texLightningFryer.height), 1.0f) * (float)texLightningFryer.height;
                        float uOffsetY2 = fmod(timeVal * (scrollSpeed2 / (float)texLightningFryer.height), 1.0f) * (float)texLightningFryer.height;

                        Rectangle src1 = { 0.0f, uOffsetY1, (float)texLightningFryer.width, ((float)texLightningFryer.height) * texRatio };
                        Rectangle dst1 = { origin.x, origin.y, width1, len };
                        DrawTexturePro(texLightningFryer, src1, dst1, { width1 / 2.0f, len }, rotDeg, ColorAlpha(SKYBLUE, 0.95f));

                        Rectangle src2 = { 0.0f, uOffsetY2, -(float)texLightningFryer.width, ((float)texLightningFryer.height) * texRatio * 1.3f };
                        Rectangle dst2 = { origin.x, origin.y, width2, len };
                        DrawTexturePro(texLightningFryer, src2, dst2, { width2 / 2.0f, len }, rotDeg, WHITE);
                    } else {
                        DrawLineEx(origin, endPos, 8.0f, SKYBLUE);
                    }
                } 
                else if (weapon == "Plasma_Rifle") {
                    // 2. PLASMA RIFLE: Sóng nhiệt Ion hóa cuộn UV tốc độ cao rực rỡ
                    float baseW = 38.0f + level * 6.5f;
                    float pulseW1 = baseW + sin(timeVal * 34.0f) * 12.0f;
                    float pulseW2 = (baseW * 0.72f) + cos(timeVal * 52.0f) * 8.0f;

                    if (texPlasmaRifle.id != 0) {
                        float scrollSpeed1 = 780.0f;
                        float scrollSpeed2 = 1200.0f;
                        float texRatio = (float)texPlasmaRifle.height > 0 ? (len / (float)texPlasmaRifle.height) * 1.35f : 2.0f;
                        float rotDeg = atan2(diff.y, diff.x) * 180.0f / 3.14159265f + 90.0f;

                        float uOffsetY1 = fmod(timeVal * (scrollSpeed1 / (float)texPlasmaRifle.height), 1.0f) * (float)texPlasmaRifle.height;
                        float uOffsetY2 = fmod(timeVal * (scrollSpeed2 / (float)texPlasmaRifle.height), 1.0f) * (float)texPlasmaRifle.height;

                        Rectangle src1 = { 0.0f, uOffsetY1, (float)texPlasmaRifle.width, ((float)texPlasmaRifle.height) * texRatio };
                        Rectangle dst1 = { origin.x, origin.y, pulseW1, len };
                        DrawTexturePro(texPlasmaRifle, src1, dst1, { pulseW1 / 2.0f, len }, rotDeg, ColorAlpha(LIME, 0.9f));

                        Rectangle src2 = { 0.0f, uOffsetY2, -(float)texPlasmaRifle.width, ((float)texPlasmaRifle.height) * texRatio * 1.2f };
                        Rectangle dst2 = { origin.x, origin.y, pulseW2, len };
                        DrawTexturePro(texPlasmaRifle, src2, dst2, { pulseW2 / 2.0f, len }, rotDeg, WHITE);
                    } else {
                        DrawRectanglePro({ origin.x, origin.y, pulseW1, len }, { pulseW1 / 2.0f, 0.0f }, angleDeg, ColorAlpha(GREEN, 0.35f));
                    }
                } 
                else if (weapon == "Laser_Cannon") {
                    // 3. LASER CANNON: Tia lade quang học cường độ cao cuộn UV
                    int idx = std::min(3, (level - 1) / 3);
                    Texture2D tex = texLaserCannon[idx];
                    float beamW = 20.0f + level * 4.0f;
                    float animW = beamW + sin(timeVal * 50.0f) * 6.0f;

                    if (tex.id != 0 && len > 0) {
                        float scrollSpeed = 1000.0f;
                        float texRatio = (float)tex.height > 0 ? (len / (float)tex.height) * 2.0f : 2.0f;
                        float rotDeg = atan2(diff.y, diff.x) * 180.0f / 3.14159265f + 90.0f;
                        float uOffsetY = fmod(timeVal * (scrollSpeed / (float)tex.height), 1.0f) * (float)tex.height;
                        
                        Rectangle src = { 0.0f, uOffsetY, (float)tex.width, ((float)tex.height) * texRatio };
                        Rectangle dst = { origin.x, origin.y, animW, len };
                        DrawTexturePro(tex, src, dst, { animW / 2.0f, len }, rotDeg, ColorAlpha(WHITE, 0.95f));
                    } else {
                        DrawLineEx(origin, endPos, animW, ColorAlpha(RED, 0.85f));
                    }
                }

                // Trả về Blend Mode chuẩn cho các đối tượng khác
                EndBlendMode();
            };
            
            DrawBeamVFX(player.get(), IsKeyDown(KEY_SPACE), autoLockTargetPos);
            bool p2FiringForVFX = false;
            if (currentGameMode == GameMode::TWO_PLAYERS) {
                p2FiringForVFX = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
            } else if (currentGameMode == GameMode::PLAYER_AND_AI && aiController) {
                p2FiringForVFX = aiController->HasValidTarget();
            }
            DrawBeamVFX(player2.get(), p2FiringForVFX, autoLockTargetPos2);

        // Vẽ Enemies ĐÈ LÊN đạn và tia lade
        if (currentState == GameState::TEST_GAMEPLAY) {
            for (const auto& enemy : activeEnemies) {
                if (enemy && enemy->IsActive()) {
                    enemy->Draw();
                }
            }
        }

        // Vẽ Player TRÊN CÙNG để không bị đạn/tia che khuất
        if (player && player->IsActive()) {
            if (RuneManager::GetInstance()->GetReviveBlinkAlpha(false) > 0.5f) {
                player->Draw();
            }
        }
        if (player2 && player2->IsActive()) {
            if (RuneManager::GetInstance()->GetReviveBlinkAlpha(true) > 0.5f) {
                player2->Draw();
            }
        }

        // --- VẼ FLOATING DAMAGE TEXT ---
        for (const auto& dt : activeDamageTexts) {
            float alpha = 1.0f;
            if (dt.timer < dt.maxLifetime * 0.4f) {
                alpha = dt.timer / (dt.maxLifetime * 0.4f); 
            }
            Color color = dt.isCrit ? RED : WHITE;
            color.a = (unsigned char)(255 * alpha);
            int fontSize = dt.isCrit ? 26 : 20; 

            const char* text = TextFormat("%d", dt.amount);
            FontManager::GetInstance()->DrawGameTextCentered(text, (int)dt.position.x - 1, (int)dt.position.y - 1, fontSize, ColorAlpha(BLACK, alpha), "Modern"); // Viền
            FontManager::GetInstance()->DrawGameTextCentered(text, (int)dt.position.x, (int)dt.position.y, fontSize, color, "Modern"); // Chữ
        }

            // --- HÌNH ẢNH QUAN SÁT DEBUG HITBOX & CIRCLE HITBOXES (PHÍM H) ---
            if (showDebugHitboxes) {
                for (const auto& enemy : activeEnemies) {
                    if (enemy->IsActive()) {
                        DrawRectangleLinesEx(enemy->GetHitbox(), 2.0f, RED);
                        FontManager::GetInstance()->DrawGameTextCentered("RECT HITBOX", (int)enemy->GetPosition().x, (int)enemy->GetPosition().y - 45, 12, RED, "Modern");
                    }
                }
                for (const auto& bullet : activeBullets) {
                    if (bullet->IsActive()) {
                        DrawCircleLines((int)bullet->GetCenter().x, (int)bullet->GetCenter().y, bullet->GetRadius(), GREEN);
                        DrawCircle((int)bullet->GetCenter().x, (int)bullet->GetCenter().y, 2.0f, YELLOW);
                    }
                }
                if (player && player->IsActive()) {
                    DrawRectangleLinesEx(player->GetHitbox(), 2.0f, BLUE);
                }
                if (player2 && player2->IsActive()) {
                    DrawRectangleLinesEx(player2->GetHitbox(), 2.0f, ORANGE);
                }
            }
            
            // Vẽ Items
            if (currentState == GameState::TEST_GAMEPLAY) {
                for (const auto& item : activeItems) {
                    if (item->IsActive()) {
                        item->Draw();
                    }
                }
                
                // Bảng Hướng dẫn Quan Sát Trực Quan & Sandbox HUD
                if (enableDevTools) {
                    DrawRectangle(150, 10, 960, 58, ColorAlpha(BLACK, 0.85f));
                    DrawRectangleLines(150, 10, 960, 58, GREEN);
                    std::string wNameStr = player ? player->GetWeapon() : "Hypergun";
                    const char* wName = wNameStr.c_str();
                    int wLv = player ? player->GetLevel() : 1;
                    const char* sboxStatus = debugSandboxMode ? "ON (Keys 1-8: Weapon, UP/DOWN: Level, F: Slow-Mo)" : "OFF [F1 to Enable Sandbox]";
                    const char* hStatus = showDebugHitboxes ? "ON [H]" : "OFF [H]";
                    FontManager::GetInstance()->DrawGameText(TextFormat("SANDBOX MODE: %s | Hitboxes: %s", sboxStatus, hStatus), 160, 18, 15, YELLOW, "Modern");
                    FontManager::GetInstance()->DrawGameText(TextFormat("Vu khi hien tai: [ %s ] - Level: [ %d / 11 ]", wName, wLv), 160, 42, 16, GREEN, "Modern");
                }
            }
            
            // Draw BACK button in playing state (only in TEST_GAMEPLAY or when dev tools are enabled)
            if ((currentState == GameState::TEST_GAMEPLAY) || enableDevTools) {
                if (DrawButton({ 20, 20, 100, 40 }, "BACK")) {
                    if (isTestMode) {
                        ChangeState(GameState::TEST_MENU);
                        activeEnemies.clear();
                        activeBullets.clear();
                        activeItems.clear();
                        if (player) {
                            player->SetPosition({(float)screenWidth/2, (float)screenHeight - 100});
                        }
                    } else {
                        EnterSummary(SummaryResult::QUIT);
                    }
                }
            }
            
            // Thêm nút Settings cho các màn TEST_GAMEPLAY
            Rectangle settingRect2 = {(float)screenWidth - 80, 20, 60, 60};
            DrawTexturePro(texSettingIcon, {0, 0, (float)texSettingIcon.width, (float)texSettingIcon.height}, settingRect2, {0,0}, 0.0f, WHITE);
            if (CheckCollisionPointRec(GetMousePosition(), settingRect2)) {
                DrawRectangleLinesEx(settingRect2, 2, RED);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    previousState = currentState;
                    ChangeState(GameState::SETTINGS);
                }
            }
            
            if (currentState == GameState::TEST_GAMEPLAY) {
                if (isWaveTransitioning) {
                    FontManager::GetInstance()->DrawGameTextCentered(TextFormat("WAVE %d - BATCH %d", currentWave, currentBatch), screenWidth/2, screenHeight/2, 50, YELLOW, "Retro");
                }
                
                if (currentStage == 7 && !isBossCutscene) {
                    int barCount = 0;
                    for (const auto& enemy : activeEnemies) {
                        if (enemy && enemy->IsActive() && enemy->role == EnemyRole::BOSS) {
                            float hpRatio = enemy->GetHp() / enemy->GetMaxHp();
                            if (hpRatio < 0.0f) hpRatio = 0.0f;
                            int barY = 15 + barCount * 30; // Đặt ở mép trên cùng của màn hình
                            DrawRectangle(screenWidth/2 - 300, barY, 600, 20, GRAY);
                            DrawRectangle(screenWidth/2 - 300, barY, 600 * hpRatio, 20, (barCount == 0) ? RED : ORANGE);
                            DrawRectangleLines(screenWidth/2 - 300, barY, 600, 20, WHITE);
                            FontManager::GetInstance()->DrawGameText(enemy->visualId == 13 ? "SUPER CHICK" : "MILITARY CHICKEN", screenWidth/2 - 295, barY + 3, 14, WHITE, "Retro");
                            FontManager::GetInstance()->DrawGameText(TextFormat("%.2f%%", hpRatio * 100.0f), screenWidth/2 + 250, barY + 3, 14, WHITE, "Modern");
                            barCount++;
                        }
                    }
                }
                
                // Di chuyển Score sang bên trái một chút để tránh đè lên nút Setting
                FontManager::GetInstance()->DrawGameText(TextFormat("Score: %d", score), screenWidth - 250, 40, 20, WHITE, "Retro");
                
                if (WaveManager::GetInstance()->IsContinuousStream()) {
                    int cKills = WaveManager::GetInstance()->GetCurrentKills();
                    int tKills = WaveManager::GetInstance()->GetTargetKills();
                    FontManager::GetInstance()->DrawGameTextCentered(TextFormat("KILLS: %d / %d", cKills, tKills), screenWidth/2, 40, 24, ORANGE, "Retro");
                }

                if (player) {
                    // --- Draw HUD for Player 1 ---
                    if (player->IsActive()) {
                        float hpRatio = player->GetHp() / player->GetMaxHp();
                        if (hpRatio < 0.0f) hpRatio = 0.0f;
                        if (hpRatio > 1.0f) hpRatio = 1.0f; // Fix HP bar going out of bounds
                        
                        DrawRectangle(20, 80, 200, 20, GRAY);
                        Color hpColor = (hpRatio > 0.5f) ? GREEN : ((hpRatio > 0.2f) ? YELLOW : RED);
                        DrawRectangle(20, 80, (int)(200 * hpRatio), 20, hpColor);
                        DrawRectangleLines(20, 80, 200, 20, DARKGRAY);
                        FontManager::GetInstance()->DrawGameText(TextFormat("P1 HP: %.0f/%.0f", player->GetHp(), player->GetMaxHp()), 25, 82, 16, BLACK, "Modern");
                        
                        float iconX = 230.0f;
                        if (player->HasSwordBuff()) {
                            DrawTexturePro(texSwordItem, {0, 0, (float)texSwordItem.width, (float)texSwordItem.height}, {iconX, 75.0f, 30.0f, 30.0f}, {0, 0}, 0.0f, WHITE);
                            FontManager::GetInstance()->DrawGameText(TextFormat("%.0fs", player->GetSwordTimer()), (int)iconX, 108, 12, ORANGE, "Modern");
                            iconX += 40.0f;
                        }
                        if (player->HasShieldBuff()) {
                            DrawTexturePro(texShieldItem, {0, 0, (float)texShieldItem.width, (float)texShieldItem.height}, {iconX, 75.0f, 30.0f, 30.0f}, {0, 0}, 0.0f, WHITE);
                            FontManager::GetInstance()->DrawGameText(TextFormat("%.0fs", player->GetShieldTimer()), (int)iconX, 108, 12, SKYBLUE, "Modern");
                        }
                        
                        // Draw EXP Bar
                        float expRatio = player->GetCurrentExp() / player->GetMaxExp();
                        if (expRatio > 1.0f) expRatio = 1.0f;
                        DrawRectangle(20, 110, 200, 15, GRAY);
                        DrawRectangle(20, 110, (int)(200 * expRatio), 15, BLUE);
                        DrawRectangleLines(20, 110, 200, 15, DARKGRAY);
                        FontManager::GetInstance()->DrawGameText(TextFormat("P1 LVL: %d  EXP: %.0f/%.0f", player->GetLevel(), player->GetCurrentExp(), player->GetMaxExp()), 25, 112, 12, WHITE, "Modern");
                        
                        // Draw Mana Bar
                        float manaRatio = player->GetCurrentMana() / player->GetMaxMana();
                        if (manaRatio > 1.0f) manaRatio = 1.0f;
                        DrawRectangle(20, 135, 200, 15, GRAY);
                        
                        Color manaColor = PURPLE;
                        if (player->IsManaActive()) {
                            manaColor = (GetTime() * 10.0 - (int)(GetTime() * 10.0) > 0.5) ? MAGENTA : PURPLE;
                        }
                        
                        DrawRectangle(20, 135, (int)(200 * manaRatio), 15, manaColor);
                        DrawRectangleLines(20, 135, 200, 15, DARKGRAY);
                        FontManager::GetInstance()->DrawGameText(TextFormat("MANA: %.0f/%.0f%s", player->GetCurrentMana(), player->GetMaxMana(), player->IsManaActive() ? " (ACTIVE)" : ""), 25, 137, 12, WHITE, "Modern");
                    }
                    
                    // --- Draw HUD for Player 2 ---
                    if (player2 && player2->IsActive()) {
                        float baseX = screenWidth - 250.0f;
                        
                        float hpRatio2 = player2->GetHp() / player2->GetMaxHp();
                        if (hpRatio2 < 0.0f) hpRatio2 = 0.0f;
                        if (hpRatio2 > 1.0f) hpRatio2 = 1.0f; // Fix HP bar going out of bounds
                        
                        DrawRectangle((int)baseX, 80, 200, 20, GRAY);
                        Color hpColor2 = (hpRatio2 > 0.5f) ? GREEN : ((hpRatio2 > 0.2f) ? YELLOW : RED);
                        DrawRectangle((int)baseX, 80, (int)(200 * hpRatio2), 20, hpColor2);
                        DrawRectangleLines((int)baseX, 80, 200, 20, DARKGRAY);
                        FontManager::GetInstance()->DrawGameText(TextFormat("P2 HP: %.0f/%.0f", player2->GetHp(), player2->GetMaxHp()), (int)baseX + 5, 82, 16, BLACK, "Modern");
                        
                        float iconX = baseX - 40.0f;
                        if (player2->HasSwordBuff()) {
                            DrawTexturePro(texSwordItem, {0, 0, (float)texSwordItem.width, (float)texSwordItem.height}, {iconX, 75.0f, 30.0f, 30.0f}, {0, 0}, 0.0f, WHITE);
                            FontManager::GetInstance()->DrawGameText(TextFormat("%.0fs", player2->GetSwordTimer()), (int)iconX, 108, 12, ORANGE, "Modern");
                            iconX -= 40.0f;
                        }
                        if (player2->HasShieldBuff()) {
                            DrawTexturePro(texShieldItem, {0, 0, (float)texShieldItem.width, (float)texShieldItem.height}, {iconX, 75.0f, 30.0f, 30.0f}, {0, 0}, 0.0f, WHITE);
                            FontManager::GetInstance()->DrawGameText(TextFormat("%.0fs", player2->GetShieldTimer()), (int)iconX, 108, 12, SKYBLUE, "Modern");
                        }
                        
                        // Draw EXP Bar
                        float expRatio2 = player2->GetCurrentExp() / player2->GetMaxExp();
                        if (expRatio2 > 1.0f) expRatio2 = 1.0f;
                        DrawRectangle((int)baseX, 110, 200, 15, GRAY);
                        DrawRectangle((int)baseX, 110, (int)(200 * expRatio2), 15, BLUE);
                        DrawRectangleLines((int)baseX, 110, 200, 15, DARKGRAY);
                        FontManager::GetInstance()->DrawGameText(TextFormat("P2 LVL: %d  EXP: %.0f/%.0f", player2->GetLevel(), player2->GetCurrentExp(), player2->GetMaxExp()), (int)baseX + 5, 112, 12, WHITE, "Modern");
                        
                        // Draw Mana Bar
                        float manaRatio2 = player2->GetCurrentMana() / player2->GetMaxMana();
                        if (manaRatio2 > 1.0f) manaRatio2 = 1.0f;
                        DrawRectangle((int)baseX, 135, 200, 15, GRAY);
                        
                        Color manaColor2 = PURPLE;
                        if (player2->IsManaActive()) {
                            manaColor2 = (GetTime() * 10.0 - (int)(GetTime() * 10.0) > 0.5) ? MAGENTA : PURPLE;
                        }
                        
                        DrawRectangle((int)baseX, 135, (int)(200 * manaRatio2), 15, manaColor2);
                        DrawRectangleLines((int)baseX, 135, 200, 15, DARKGRAY);
                        FontManager::GetInstance()->DrawGameText(TextFormat("MANA: %.0f/%.0f%s", player2->GetCurrentMana(), player2->GetMaxMana(), player2->IsManaActive() ? " (ACTIVE)" : ""), (int)baseX + 5, 137, 12, WHITE, "Modern");
                    }
                }
                // --- Stats Panel (Tab overlay) ---
                statsPanel.Update(GetFrameTime(), IsKeyDown(KEY_TAB));
                if (player) statsPanel.Draw(player.get(), screenHeight);
            }
            break;
        }

        // --- Vẽ màn hình chọn Chỉ Số (STAT) và Argument (ARGUMENT) ---
        case GameState::STAT_SELECTION:
        case GameState::ARGUMENT_SELECTION:
        {
            // 1. Vẽ game entities phía sau (đã đóng băng)
            if (player && player->IsActive()) player->Draw();
            for (const auto& enemy : activeEnemies) {
                if (enemy->IsActive()) enemy->Draw();
            }
            for (const auto& bullet : activeBullets) {
                if (bullet->IsActive()) bullet->Draw();
            }

            // 2. Lớp phủ tối mờ
            DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 170});

            // 3. Tính toán layout và animation
            const float CARD_W  = 320.0f;
            const float CARD_H  = 490.0f;
            const float GAP     = 40.0f;
            const float totalW  = currentNumChoices * CARD_W + (currentNumChoices - 1) * GAP;
            const float startX  = (screenWidth  - totalW) / 2.0f;
            const float finalY  = (screenHeight - CARD_H) / 2.0f;

            float t = selectionAnimTimer / 0.4f;
            t = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t); // ease-out cubic
            float cardY = finalY + (1.0f - t) * 350.0f;

            // 4. Tiêu đề màn hình
            bool isStat = isStatSelection;
            const char* titleText = isStat ? "CHOOSE A STAT UPGRADE" : "CHOOSE AN ARGUMENT";
            Color titleColor = isStat ? (Color){80, 200, 120, 255} : (Color){255, 180, 50, 255};
            FontManager::GetInstance()->DrawGameTextCentered(titleText, screenWidth/2 + 2, (int)(cardY - 72), 34, {0, 0, 0, 120}, "Retro");
            FontManager::GetInstance()->DrawGameTextCentered(titleText, screenWidth/2, (int)(cardY - 74), 34, titleColor, "Retro");

            // Phụ đề
            const char* subText = isStat ? "Choose 1 option to power up your spaceship"
                                         : "Choose a special Argument that will shape your journey";
            FontManager::GetInstance()->DrawGameTextCentered(subText, screenWidth/2, (int)(cardY - 38), 16, (Color){200, 200, 200, 200}, "Modern");

            // 5. Vẽ thẻ card
            Texture2D& cardTex = isStat ? texChiSo : texLoi;
            Vector2 mouse = GetMousePosition();

            for (int i = 0; i < currentNumChoices; i++) {
                float cx = startX + i * (CARD_W + GAP);
                Rectangle cardRect = { cx, cardY, CARD_W, CARD_H };
                bool hovered = CheckCollisionPointRec(mouse, cardRect);

                // 5a. Nền card (ảnh loi.png / chiso.png lấy phần khung)
                // Cắt phần khung thực sự của ảnh (dựa trên bounding box)
                Rectangle srcRect = { 244, 29, 186, 287 }; 
                
                // Phân biệt vùng viền bên ngoài và vùng nội dung text (để text không đè lên viền kim loại)
                Rectangle borderArea = cardRect;
                // Vùng viết chữ được định vị theo tỉ lệ mới của thẻ 320x490
                Rectangle textArea = {
                    cardRect.x + 40.0f,
                    cardRect.y + 110.0f,
                    cardRect.width - 80.0f,
                    cardRect.height - 180.0f
                };

                DrawTexturePro(cardTex,
                    srcRect,
                    borderArea,
                    {0, 0}, 0.0f,
                    hovered ? WHITE : (Color){200, 200, 200, 255});

                // 5c. Dữ liệu card
                int idx = shownCardIndices[i];
                const CardDef& def = isStat ? ALL_STATS[idx] : ALL_ARGUMENTS[idx];

                // Số thứ tự card (góc trên trái nhỏ) - đưa vào trong một chút để không nằm ở vùng viền bị vát
                FontManager::GetInstance()->DrawGameText(TextFormat("%d", i + 1), (int)(cx + 35), (int)(cardY + 35), 16, (Color){180, 180, 180, 180}, "Modern");

                // Tên card (dòng đầu, lớn, tô đậm bằng double draw)
                int nameFontSize = 20; // Giảm lại font 20 theo yêu cầu
                int nameW = FontManager::GetInstance()->MeasureGameText(def.name, nameFontSize, "Modern").x;
                float nameX = textArea.x + textArea.width / 2.0f - nameW / 2.0f;
                float nameY = textArea.y + 10.0f;
                FontManager::GetInstance()->DrawGameText(def.name, (int)nameX + 1, (int)nameY + 1, nameFontSize, {0, 0, 0, 100}, "Modern"); // shadow
                FontManager::GetInstance()->DrawGameText(def.name, (int)nameX, (int)nameY, nameFontSize,
                         isStat ? (Color){120, 240, 150, 255} : (Color){255, 210, 80, 255}, "Modern");

                // Đường phân cách
                DrawLineEx(
                    {textArea.x + 15, nameY + 35},
                    {textArea.x + textArea.width - 15, nameY + 35},
                    2.0f,
                    isStat ? (Color){120, 240, 150, 100} : (Color){255, 210, 80, 100}
                );

                // Mô tả (font nhỏ, nhiều dòng bằng \n trong chuỗi)
                // Căn lề trái dịch sang phải (textArea.x + 35) để text gói trong khoảng 200px
                FontManager::GetInstance()->DrawGameText(def.description, (int)(textArea.x + 35), (int)(nameY + 50), 18, (Color){220, 220, 220, 230}, "Modern");

                // 5d. Hiệu ứng hover: viền vàng/xanh sáng
                if (hovered) {
                    DrawRectangleLinesEx(cardRect, 4,
                        isStat ? (Color){80, 255, 130, 255} : (Color){255, 200, 50, 255});
                    // Text gợi ý bấm
                    const char* clickHint = "Click to select";
                    FontManager::GetInstance()->DrawGameTextCentered(clickHint, (int)(cx + CARD_W/2), (int)(cardY + CARD_H - 28), 14,
                             isStat ? (Color){80, 255, 130, 220} : (Color){255, 200, 50, 220}, "Modern");
                }
            }
            break;
        }

        case GameState::SUMMARY:
        {
            if (summaryScreen) {
                summaryScreen->Draw();
            }
            break;
        }

        default:
            break;
    }

    EndDrawing();
}

void GameManager::CleanUp() {
    SoundManager::GetInstance()->CleanUp();
    SoundManager::DestroyInstance();

    for (int i = 0; i < 4; i++) {
        UnloadTexture(texBackgrounds[i]);
    }
    if (IsWindowReady()) {
        UnloadTexture(texSettingIcon);
    }
    
    if (mainMenuUI) {
        delete mainMenuUI;
        mainMenuUI = nullptr;
    }
    if (shopUI) { delete shopUI; shopUI = nullptr; }
    if (tutorialUI) { delete tutorialUI; tutorialUI = nullptr; }
    if (runeUI) { delete runeUI; runeUI = nullptr; }
    if (summaryScreen) {
        delete summaryScreen;
        summaryScreen = nullptr;
    }
    if (titleScreen) {
        delete titleScreen;
        titleScreen = nullptr;
    }

    RuneManager::DestroyInstance();
    ShopManager::GetInstance()->Save();
    ShopManager::DestroyInstance();
    ItemDropManager::DestroyInstance();
    CoinManager::DestroyInstance();

    UnloadTexture(texSpaceshipHypergun);
    UnloadTexture(texSpaceship2);
    for (int i = 0; i < 20; i++) UnloadTexture(texEnemyAnims[i]);
    UnloadTexture(texAsteroid1);
    UnloadTexture(texAsteroid2);
    for(int i=0; i<3; i++) UnloadTexture(texNeutronGun[i]);
    UnloadTexture(texRiddler);
    UnloadTexture(texLightningFryer);
    for(int i=0; i<2; i++) { UnloadTexture(texIonBlaster[i]); UnloadTexture(texUtensilPoker[i]); }
    for(int i=0; i<4; i++) UnloadTexture(texLaserCannon[i]);
    UnloadTexture(texGrenade);
    UnloadTexture(texKnife);
    UnloadTexture(texLoi);
    UnloadTexture(texChiSo);
    
    if (player2) player2.reset();

    FontManager::GetInstance()->DestroyInstance();
        
    if (IsWindowReady()) {
        CloseWindow();
    }
}

void GameManager::DrawTextCustom(const char* text, int posX, int posY, int fontSize, Color color) {
    FontManager::GetInstance()->DrawGameText(text, posX, posY, (float)fontSize, color, "Modern");
}

int GameManager::MeasureTextCustom(const char* text, int fontSize) {
    Vector2 size = FontManager::GetInstance()->MeasureGameText(text, (float)fontSize, "Modern");
    return (int)size.x;
}

