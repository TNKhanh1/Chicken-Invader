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
#include "../include/Bullet.h"
#include "../include/Item.h"
#include "../include/Meat.h"
#include "../include/SpaceshipDataManager.h"
#include "../include/HypergunShootingBehavior.h"
#include <iostream>
#include <cmath>
#include <algorithm>

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
      "Killing an enemy\nrestores 15 HP\nto your spaceship." },
    { "Energy Flow",
      "Each shot fired\nrestores 2 Mana\nto your reserves." },
    { "Round Recovery",
      "Restore 80 HP\nat the start\nof each new Wave." },
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

// --- Implement methods for entities ---
void Enemy::Update(float deltaTime) {
    // Áp dụng Strategy di chuyển
    if (movementBehavior) {
        auto gm = GameManager::GetInstance();
        movementBehavior->Move(position, moveSpeed, deltaTime, gm->GetScreenWidth(), gm->GetScreenHeight());
    }

    // Cập nhật animation và trail của ASTEROID
    if (enemyType == 4) {
        // --- Cập nhật frame (có 30 frames, chỉ lấy từ hàng 1 - thân thiên thạch) ---
        frameTimer += deltaTime;
        if (frameTimer >= 0.04f) { // ~25 FPS animation
            frameTimer = 0.0f;
            currentFrame = (currentFrame + 1) % 30; // 30 frames trong hàng 1
        }

        // --- Spawn trail point tại vị trí hiện tại (mỗi 40ms) ---
        trailSpawnTimer += deltaTime;
        if (trailSpawnTimer >= 0.04f) {
            trailSpawnTimer = 0.0f;
            TrailPoint tp;
            tp.pos   = position;
            tp.alpha = 1.0f;
            tp.frame = currentFrame;
            trailPoints.push_back(std::move(tp));
        }

        // --- Fade các trail point, xóa những cái đã biến mất ---
        // fade speed = 2.0f (biến mất sau 0.5 giây)
        const float fadeSpeed = 2.0f;
        for (auto& tp : trailPoints) {
            tp.alpha -= fadeSpeed * deltaTime;
        }
        trailPoints.erase(
            std::remove_if(trailPoints.begin(), trailPoints.end(),
                [](const Enemy::TrailPoint& t) { return t.alpha <= 0.0f; }),
            trailPoints.end()
        );
    }
    
    // Logic thả trứng (bắn đạn)
    if (canShoot) {
        eggDropTimer -= deltaTime;
        if (eggDropTimer <= 0.0f) {
            ResetEggTimer();
            auto egg = std::make_shared<Bullet>(position, damage, 150.0f, false);
            GameManager::GetInstance()->AddBullet(egg);
        }
    }
    
    // Hủy enemy nếu đi quá xa khỏi màn hình
    if (position.y > GameManager::GetInstance()->GetScreenHeight() + 300.0f ||
        position.x > GameManager::GetInstance()->GetScreenWidth() + 300.0f ||
        position.x < -1000.0f) {
        isActive = false;
    }
}

void Bullet::Draw() {
    if (!isActive) return;
    
    if (isPlayerBullet) {
        if (bulletType == 1) { // Strong
            Texture2D tex = GameManager::GetInstance()->GetTexBulletStrong();
            if (tex.id != 0) {
                DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height},
                               {position.x, position.y, 20.0f, 40.0f}, {10.0f, 20.0f}, 0.0f, WHITE);
                return;
            } else {
                DrawCircle(position.x, position.y, 8, ORANGE);
                return;
            }
        } else if (bulletType == 2) { // Weak
            Texture2D tex = GameManager::GetInstance()->GetTexBulletWeak();
            if (tex.id != 0) {
                DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height},
                               {position.x, position.y, 10.0f, 20.0f}, {5.0f, 10.0f}, 0.0f, WHITE);
                return;
            } else {
                DrawCircle(position.x, position.y, 4, YELLOW);
                return;
            }
        }
        
        // Default Player Bullet
        Texture2D tex = GameManager::GetInstance()->GetTexBulletPlayer();
        if (tex.id != 0) {
            DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, 
                           {position.x, position.y, 10.0f, 30.0f}, {5.0f, 15.0f}, 0.0f, WHITE);
        } else {
            DrawCircle(position.x, position.y, 6, SKYBLUE);
        }
    } else {
        // Enemy Bullet
        Texture2D tex = GameManager::GetInstance()->GetTexEnemyBullet();
        if (tex.id != 0) {
            DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, 
                           {position.x, position.y, 20.0f, 25.0f}, {10.0f, 12.5f}, 0.0f, WHITE);
        } else {
            DrawCircle(position.x, position.y, 8, RED);
        }
    }
}

void Enemy::Draw() {
    auto gm = GameManager::GetInstance();
    
    if (enemyType == 4) { // ASTEROID
        Texture2D tex = (asteroidVariant == 1) ? gm->GetTexAsteroid1() : gm->GetTexAsteroid2();
        // Layout: 7680x2048, 30 cột x 2 hàng. Mỗi frame = 256x1024 px.
        // Hàng 0 (trên, y=0..1023): trail/lửa phía trên thiên thạch
        // Hàng 1 (dưới, y=1024..2047): thân thiên thạch
        const int   COLS    = 30;
        const float FRAME_W = (float)tex.width  / COLS;  // 256 px
        const float FULL_H  = (float)tex.height;           // 2048 px (cả 2 hàng)
        const int   bodyCol = currentFrame % COLS;

        // Khi vẽ 100px rộng: mỗi hàng cao 100*(1024/256)=400px, tổng 800px
        // Thân đá ở 400px dưới, trail lửa ở 400px trên → tự nhiên
        const float DEST_W      = 100.0f;
        const float DEST_ROW_H  = DEST_W * (FULL_H * 0.5f / FRAME_W); // 400px
        const float DEST_FULL_H = DEST_ROW_H * 2.0f;                   // 800px
        // Neo ở đáy (cuối hàng 1) → trail lửa bay lên trên tự nhiên
        Vector2 fullOrigin = { DEST_W * 0.5f, DEST_FULL_H };

        // 1) Particle motion echo tại các vị trí cũ: vòng tròn nhỏ xiu
        //    THAY THẾP ghost sprite → loại bỏ hiệu ứng "tàng ảnh"
        for (const auto& tp : trailPoints) {
            float radius = 10.0f * tp.alpha;        // thu nhỏ dần
            unsigned char a = (unsigned char)(tp.alpha * 160.0f);
            if (asteroidVariant == 2) { // asteroidFlame: cam/lửa
                DrawCircleGradient((int)tp.pos.x, (int)tp.pos.y,
                                   radius, {255, 160, 30, a}, {255, 80, 0, 0});
            } else {                    // asteroidNormal: xám/đá
                DrawCircleGradient((int)tp.pos.x, (int)tp.pos.y,
                                   radius, {200, 200, 210, a}, {120, 120, 130, 0});
            }
        }

        // 2) Vẽ toàn bộ sprite (hàng trail + hàng thân) gộp làm 1 draw call
        //    Nguồn: cột bodyCol, từ y=0 đến hết (cả 2 hàng)
        Rectangle fullSrc = { bodyCol * FRAME_W, 0.0f, FRAME_W, FULL_H };
        Rectangle fullDst = { position.x, position.y, DEST_W, DEST_FULL_H };
        DrawTexturePro(tex, fullSrc, fullDst, fullOrigin, 0.0f, WHITE);

    } else {
        Texture2D tex = gm->GetTexEnemy();
        Vector2 origin = { (float)tex.width / 2, (float)tex.height / 2 };
        Rectangle sourceRec = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
        Rectangle destRec = { position.x, position.y, (float)tex.width, (float)tex.height };
        DrawTexturePro(tex, sourceRec, destRec, origin, 0.0f, WHITE);
    }
                   
    // Hiện thanh máu nếu chuột di vào hitbox
    if (CheckCollisionPointRec(GetMousePosition(), GetHitbox())) {
        float hpRatio = GetHp() / GetMaxHp();
        if (hpRatio < 0.0f) hpRatio = 0.0f;
        DrawRectangle(position.x - 25, position.y - 50, 50, 6, RED);
        DrawRectangle(position.x - 25, position.y - 50, 50 * hpRatio, 6, GREEN);
        DrawRectangleLines(position.x - 25, position.y - 50, 50, 6, BLACK);
    }
}

void Spaceship::Draw() {
    if (!isActive) return;
    
    // Mặc định luôn vẽ Hypergun vì ảnh cũ đã bị xóa
    Texture2D tex = GameManager::GetInstance()->GetTexSpaceshipHypergun();
    if (tex.id != 0) {
        DrawTexturePro(tex, {36.0f, 201.0f, 108.0f, 95.0f}, 
                       {position.x, position.y, 90.0f, 90.0f}, {45.0f, 45.0f}, 0.0f, WHITE);
    }
}
// -------------------------------------------

GameManager::GameManager() 
    : previousState(GameState::MAIN_MENU), currentState(GameState::MAIN_MENU), screenWidth(1600), screenHeight(900), isRunning(false), score(0), currentWave(0), currentBatch(0), waveTimer(0.0f), isWaveTransitioning(false) {
    texSpaceship = {0};
    texSpaceshipHypergun = {0};
    texEnemy = {0};
    texAsteroid1 = {0};
    texAsteroid2 = {0};
    texBulletStrong = {0};
    texBulletWeak = {0};
    texBulletPlayer = {0};
    texEnemyBullet = {0};
    texMeat = {0};
    texLoi = {0};
    texChiSo = {0};
    texSettingIcon = {0};
    for(int i=0; i<4; i++) texBackgrounds[i] = {0};
}

GameManager::~GameManager() {
    CleanUp();
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

void GameManager::StartWave(int waveIndex) {
    currentWave = waveIndex;
    currentBatch = 1;
    isWaveTransitioning = true;
    waveTimer = 3.0f; // 3 seconds delay before wave starts
    
    // Round Recovery (Arg 8)
    if (player && player->HasArgument(8)) {
        player->Heal(80.0f);
    }
}

void GameManager::EnterStatSelection(int nextWave) {
    nextWaveAfterSelection  = nextWave;
    pendingArgumentAfterStat = (nextWave == 5 || nextWave == 10 || nextWave == 15);
    isStatSelection         = true;
    selectionAnimTimer      = 0.0f;

    // Chọn 3 stat ngẫu nhiên không trùng nhau
    int pool[6] = {0, 1, 2, 3, 4, 5};
    for (int i = 0; i < 3; i++) {
        int j = i + GetRandomValue(0, 5 - i);
        int tmp = pool[i]; pool[i] = pool[j]; pool[j] = tmp;
        shownCardIndices[i] = pool[i];
    }
    ChangeState(GameState::STAT_SELECTION);
}

bool GameManager::SpawnWaveBatch(int wave, int batch) {
    if (wave == 1) {
        if (batch == 1) {
            for (int i = 0; i < 10; ++i) {
                float x = screenWidth + 100.0f + i * 150.0f;
                float y = 150.0f + (i % 2) * 100.0f;
                auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {x, y}, wave);
                enemy->SetMovementBehavior(std::make_unique<HorizontalSweepMovement>(-1.0f));
                enemy->ResetEggTimer();
                AddEnemy(std::move(enemy));
            }
            return true;
        } else if (batch == 2) {
            for (int i = 0; i < 15; ++i) {
                float x = -100.0f - i * 150.0f;
                float y = 150.0f + (i % 3) * 100.0f; 
                auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {x, y}, wave);
                enemy->SetMovementBehavior(std::make_unique<HorizontalSweepMovement>(1.0f));
                enemy->ResetEggTimer();
                AddEnemy(std::move(enemy));
            }
            return true;
        } else if (batch == 3) {
            float startX = screenWidth / 2.0f;
            for (int r = 0; r < 3; ++r) {
                for (int c = 0; c < 5; ++c) {
                    float offsetX = (c - 2) * 150.0f;
                    float x = startX + offsetX;
                    float y = -300.0f + r * 100.0f; 
                    float targetY = 100.0f + r * 100.0f;
                    auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {x, y}, wave);
                    enemy->SetMovementBehavior(std::make_unique<HorizontalBounceMovement>(targetY, 300.0f, 1.0f));
                    enemy->ResetEggTimer();
                    AddEnemy(std::move(enemy));
                }
            }
            return true;
        }
    } else if (wave == 2) {
        if (batch == 1) {
            for (int i = 0; i < 15; ++i) {
                float x = 150.0f + (i % 5) * 250.0f;
                float y = -100.0f - (i / 5) * 150.0f;
                auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {x, y}, wave);
                enemy->SetMovementBehavior(std::make_unique<VerticalZigzagMovement>());
                enemy->ResetEggTimer();
                AddEnemy(std::move(enemy));
            }
            return true;
        } else if (batch == 2) {
            for (int i = 0; i < 10; ++i) {
                float x = 200.0f + i * 120.0f;
                float y = -100.0f - i * 50.0f;
                auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::SWARM_CHICKEN, {x, y}, wave);
                enemy->SetMovementBehavior(std::make_unique<SineZigzagMovement>());
                enemy->ResetEggTimer();
                AddEnemy(std::move(enemy));
            }
            return true;
        } else if (batch == 3) {
            float startX = screenWidth / 2.0f;
            for (int i = 0; i < 15; ++i) {
                float offsetX = (i % 5 - 2) * 200.0f;
                float offsetY = -200.0f - (i / 5) * 150.0f;
                auto type = (i % 3 == 0) ? EnemyFactory::EnemyType::SWARM_CHICKEN : EnemyFactory::EnemyType::NORMAL_CHICKEN;
                auto enemy = EnemyFactory::CreateEnemy(type, {startX + offsetX, offsetY}, wave);
                enemy->SetMovementBehavior(std::make_unique<VerticalZigzagMovement>());
                enemy->ResetEggTimer();
                AddEnemy(std::move(enemy));
            }
            return true;
        }
    } else if (wave == 3) {
        if (batch == 1) {
            for (int i = 0; i < 20; i++) {
                float x = GetRandomValue(100, screenWidth - 100);
                float y = -100.0f - i * 150.0f; 
                auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::ASTEROID, {x, y}, wave);
                enemy->SetMovementBehavior(std::make_unique<MeteorDiveMovement>());
                enemy->asteroidVariant = 1;
                enemy->canShoot = false;
                AddEnemy(std::move(enemy));
            }
            return true;
        } else if (batch == 2) {
            for (int i = 0; i < 5; ++i) {
                float x = 200.0f + i * 300.0f;
                auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::TANK_CHICKEN, {x, -100.0f}, wave);
                enemy->SetMovementBehavior(std::make_unique<StraightMovement>());
                enemy->ResetEggTimer();
                AddEnemy(std::move(enemy));
            }
            return true;
        } else if (batch == 3) {
            // Mưa thiên thạch lửa (asteroidFlame) từ 2 góc + 3 Tank Chicken
            // Spawn trong vùng hợp lý để không bị kẹt off-screen
            // Cột trái: 5 asteroid
            for (int i = 0; i < 5; ++i) {
                float ax = 100.0f + i * 80.0f;
                float ay = -200.0f - i * 120.0f; // max -680
                auto asteroid = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::ASTEROID, {ax, ay}, wave);
                asteroid->SetMovementBehavior(std::make_unique<MeteorDiveMovement>());
                asteroid->asteroidVariant = 2;
                asteroid->canShoot = false;
                AddEnemy(std::move(asteroid));
            }
            // Cột phải: 5 asteroid
            for (int i = 0; i < 5; ++i) {
                float ax = (float)(screenWidth - 100) - i * 80.0f;
                float ay = -300.0f - i * 120.0f; // max -780, lệch pha cột trái
                auto asteroid = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::ASTEROID, {ax, ay}, wave);
                asteroid->SetMovementBehavior(std::make_unique<MeteorDiveMovement>());
                asteroid->asteroidVariant = 2;
                asteroid->canShoot = false;
                AddEnemy(std::move(asteroid));
            }
            // 3 Tank Chicken: dùng StraightMovement để không kẹt off-screen
            for (int i = 0; i < 3; ++i) {
                float tx = 250.0f + i * 450.0f;
                float ty = -150.0f - i * 200.0f; // max -550, nhanh xuất hiện
                auto tank = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::TANK_CHICKEN, {tx, ty}, wave);
                tank->SetMovementBehavior(std::make_unique<StraightMovement>());
                tank->ResetEggTimer();
                AddEnemy(std::move(tank));
            }
            return true;

        }
    } else if (wave == 4) {
        if (batch == 1) {
            for (int i = 0; i < 20; ++i) {
                auto type = (i % 2 == 0) ? EnemyFactory::EnemyType::NORMAL_CHICKEN : EnemyFactory::EnemyType::SWARM_CHICKEN;
                float x = (i % 2 == 0) ? -100.0f - i * 100.0f : screenWidth + 100.0f + i * 100.0f;
                float y = 100.0f + (i % 5) * 80.0f;
                auto enemy = EnemyFactory::CreateEnemy(type, {x, y}, wave);
                enemy->SetMovementBehavior(std::make_unique<HorizontalSweepMovement>((i % 2 == 0) ? 1.0f : -1.0f));
                enemy->ResetEggTimer();
                AddEnemy(std::move(enemy));
            }
            return true;
        } else if (batch == 2) {
            for (int i = 0; i < 8; ++i) {
                float x = 200.0f + i * 170.0f;
                auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::TANK_CHICKEN, {x, -100.0f}, wave);
                enemy->SetMovementBehavior(std::make_unique<HorizontalBounceMovement>(150.0f, 100.0f, 1.0f));
                enemy->ResetEggTimer();
                AddEnemy(std::move(enemy));
            }
            return true;
        } else if (batch == 3) {
            for (int i = 0; i < 20; ++i) {
                EnemyFactory::EnemyType type = EnemyFactory::EnemyType::NORMAL_CHICKEN;
                if (i % 4 == 0) type = EnemyFactory::EnemyType::TANK_CHICKEN;
                else if (i % 4 == 1) type = EnemyFactory::EnemyType::SWARM_CHICKEN;
                
                float x = GetRandomValue(100, screenWidth - 100);
                float y = -100.0f - i * 100.0f;
                auto enemy = EnemyFactory::CreateEnemy(type, {x, y}, wave);
                if (type == EnemyFactory::EnemyType::TANK_CHICKEN) {
                    enemy->SetMovementBehavior(std::make_unique<StraightMovement>());
                } else {
                    enemy->SetMovementBehavior(std::make_unique<SineZigzagMovement>());
                }
                enemy->ResetEggTimer();
                AddEnemy(std::move(enemy));
            }
            return true;
        }
    }
    
    // Nếu wave > 4 hoặc batch > 3, trả về false để biết là wave này đã kết thúc
    return false;
}

void GameManager::Init(int width, int height, const char* title) {
    screenWidth = width;
    screenHeight = height;
    
    // Khởi tạo cửa sổ Raylib
    InitWindow(screenWidth, screenHeight, title);
    SetTargetFPS(60); // Đặt tốc độ khung hình 60 FPS
    
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
    
    // Tải dữ liệu phi thuyền
    SpaceshipDataManager::GetInstance()->LoadCSV("assets/spaceship/spaceship.csv");
    SpaceshipDataManager::GetInstance()->LoadJSON("Hypergun", "assets/spaceship/hypergun.json");

    // Bỏ load spaceship01.png cũ đã bị xóa
    texSpaceshipHypergun = LoadTexture("assets/spaceship/hypergun_spaceship.png");
    texBulletStrong = LoadTexture("assets/spaceship/hypergun_strong.png");
    texBulletWeak = LoadTexture("assets/spaceship/hypergun_weak.png");
    texEnemy = LoadTexture("assets/enemy/chicken03.png");
    texAsteroid1 = LoadTexture("assets/asteroidNormal.png");
    texAsteroid2 = LoadTexture("assets/asteroidFlame.png"); // asteroidFlame: 7680x2048 = 15col x 4row = 60 frames
    // Bỏ load Bullet01_1.png đã bị xóa
    texEnemyBullet = LoadTexture("assets/egg.png"); 
    texMeat = LoadTexture("assets/meat.png");
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
    
    printf("GameManager::Init finished.\n"); fflush(stdout);
    isRunning = true;
    currentState = GameState::MAIN_MENU; // Bắt đầu ở MAIN_MENU
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
    int textWidth = MeasureText(text, 20);
    DrawText(text, bounds.x + (bounds.width / 2) - (textWidth / 2), bounds.y + (bounds.height / 2) - 10, 20, textColor);
    
    return clicked;
}

void GameManager::Update(float deltaTime) {
    // UpdateMusicStream(bgMusic);

    // Handle State Transitions and specific state logic
    switch (currentState) {
        case GameState::MAIN_MENU:
        case GameState::TEST_MENU:
        case GameState::GAME_OVER:
        case GameState::SETTINGS:
            // Transition logic is handled in Draw() via DrawButton
            break;

        case GameState::TEST_GAMEPLAY:
        case GameState::TEST_ENEMY:
        case GameState::TEST_SPACESHIP:
        {
            // --- Background Scrolling ---
            bgY += 30.0f * deltaTime; // Tăng tốc độ background lên 15%
            if (bgY >= 2.0f * screenHeight) bgY -= 2.0f * screenHeight;
            
            // --- Player Logic ---
            if (currentState != GameState::TEST_ENEMY) {
                if (player && player->IsActive()) {
                    Vector2 pos = player->GetPosition();
                    if (IsKeyDown(KEY_W)) pos.y -= player->GetMoveSpeed() * deltaTime;
                    if (IsKeyDown(KEY_S)) pos.y += player->GetMoveSpeed() * deltaTime;
                    if (IsKeyDown(KEY_A)) pos.x -= player->GetMoveSpeed() * deltaTime;
                    if (IsKeyDown(KEY_D)) pos.x += player->GetMoveSpeed() * deltaTime;
                    
                    if (IsKeyPressed(KEY_L)) {
                        player->LevelUp();
                    }
                    
                    if (pos.x < 0) pos.x = 0;
                    if (pos.x > screenWidth) pos.x = screenWidth;
                    if (pos.y < 0) pos.y = 0;
                    if (pos.y > screenHeight) pos.y = screenHeight;
                    
                    player->SetPosition(pos);

                    // Player input & update
                    player->Update(deltaTime); // Cập nhật thời gian hồi chiêu
                    static int frameCountDebug = 0;
                    frameCountDebug++;
                    if (frameCountDebug == 5 || IsKeyDown(KEY_SPACE)) {
                        if (player->CanFire()) {
                            player->Fire();
                        }
                    }
                }
            }

            // --- Wave Logic ---
            if (currentState == GameState::TEST_GAMEPLAY) {
                if (isWaveTransitioning) {
                    waveTimer -= deltaTime;
                    if (waveTimer <= 0.0f) {
                        isWaveTransitioning = false;
                        
                        // Cố gắng spawn batch hiện tại
                        bool batchSpawned = SpawnWaveBatch(currentWave, currentBatch);
                        
                        // Nếu spawn thất bại (hết batch của wave này)
                        if (!batchSpawned) {
                            EnterStatSelection(currentWave + 1);
                        }
                    }
                } else if (activeEnemies.empty()) {
                    // Cấu trúc wave hiện tại đều có 3 batch (từ wave 1 đến wave 4)
                    int maxBatch = 3; 
                    if (currentBatch < maxBatch) {
                        // Khi dọn sạch batch hiện tại, chuẩn bị sang batch tiếp theo
                        currentBatch++;
                        isWaveTransitioning = true;
                        waveTimer = 3.0f;
                    } else {
                        // Nếu đã xong batch cuối của wave, chuyển sang chọn chỉ số ngay lập tức
                        EnterStatSelection(currentWave + 1);
                    }
                }
            } else if (currentState == GameState::TEST_ENEMY) {
                // Spawner đơn giản cho TEST_ENEMY (để người chơi test bắn gà)
                if (activeEnemies.empty()) {
                    float x = GetRandomValue(200, screenWidth - 200);
                    auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {x, 100.0f});
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

            // --- Collision Detection (chỉ khi đang gameplay thực sự) ---
            if (currentState == GameState::TEST_GAMEPLAY) {
                for (auto& bullet : activeBullets) {
                    if (!bullet->IsActive()) continue;
                    if (bullet->IsPlayerBullet()) {
                        for (auto& enemy : activeEnemies) {
                            if (!enemy->IsActive()) continue;
                            if (CheckCollisionRecs(bullet->GetHitbox(), enemy->GetHitbox())) {
                                float finalDamage = bullet->GetDamage() + player->GetPermanentDamageBonus();
                                if (player->HasArgument(3) && enemy->enemyType == 4) { // 3: Boss Hunter (assume asteroid enemyType 4 is boss-like for now since no bosses yet)
                                    finalDamage *= 1.8f;
                                }
                                if (player->HasArgument(4)) { // 4: Armor Crusher
                                    finalDamage += enemy->GetHp() * 0.03f;
                                }
                                bullet->SetActive(false);
                                enemy->TakeDamage(finalDamage);
                                if (!enemy->IsActive()) {
                                    AddScore(enemy->GetPointValue());
                                    // Blood Fury (Arg 5)
                                    if (player->HasArgument(5)) player->AddPermanentDamage(2.0f);
                                    // Bloodthirst (Arg 6)
                                    if (player->HasArgument(6)) player->Heal(15.0f);
                                    
                                    PlayExplosionSound();
                                    auto meat = std::make_shared<Meat>(enemy->GetPosition(),
                                        Vector2{(float)GetRandomValue(-100, 100), -200.0f});
                                    activeItems.push_back(meat);
                                }
                            }
                        }
                    } else {
                        if (player && player->IsActive()) {
                            if (CheckCollisionRecs(bullet->GetHitbox(), player->GetHitbox())) {
                                bullet->SetActive(false);
                                player->TakeDamage(bullet->GetDamage());
                            }
                        }
                    }
                }

                // Va chạm trực tiếp quái và Player
                for (auto& enemy : activeEnemies) {
                    if (!enemy->IsActive() || !player || !player->IsActive()) continue;
                    if (CheckCollisionRecs(player->GetHitbox(), enemy->GetHitbox())) {
                        enemy->TakeDamage(100);
                        player->TakeDamage(enemy->enemyType == 4 ? 50 : 20);
                    }
                }

                // Va chạm Item và Player
                if (player && player->IsActive()) {
                    for (auto& item : activeItems) {
                        if (!item->IsActive()) continue;
                        Rectangle itemRect = {item->GetPosition().x - 20, item->GetPosition().y - 20, 40, 40};
                        if (CheckCollisionRecs(itemRect, player->GetHitbox())) {
                            item->SetActive(false);
                            if (item->GetType() == ItemType::DRUMSTICK) {
                                AddScore(50);
                                player->GainExp(10.0f);
                                PlayPickupSound();
                            }
                        }
                    }
                }

                // Game Over Check
                if (player && player->GetHp() <= 0) {
                    currentState = GameState::GAME_OVER;
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


        // --- Xử lý màn hình chọn Chỉ Số ---
        case GameState::STAT_SELECTION:
        {
            selectionAnimTimer += deltaTime;
            if (selectionAnimTimer > 0.4f) selectionAnimTimer = 0.4f;

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
                for (int i = 0; i < numChoices; i++) {
                    Rectangle cardRect = { startX + i * (CARD_W + GAP), cardY, CARD_W, CARD_H };
                    if (CheckCollisionPointRec(GetMousePosition(), cardRect)) {
                        // Áp dụng chỉ số ở đây (nếu có logic cộng chỉ số, tuỳ ý)
                        
                        if (pendingArgumentAfterStat) {
                            isStatSelection = false;
                            selectionAnimTimer = 0.0f;
                            int pool[10] = {0,1,2,3,4,5,6,7,8,9};
                            int argChoices = (player && player->HasArgument(2)) ? 4 : 3;
                            for (int k = 0; k < argChoices; k++) {
                                int j = k + GetRandomValue(0, 9 - k);
                                int tmp = pool[k]; pool[k] = pool[j]; pool[j] = tmp;
                                shownCardIndices[k] = pool[k];
                            }
                            ChangeState(GameState::ARGUMENT_SELECTION);
                        } else {
                            StartWave(nextWaveAfterSelection);
                            ChangeState(GameState::TEST_GAMEPLAY);
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
                for (int i = 0; i < numChoices; i++) {
                    Rectangle cardRect = { startX + i * (CARD_W + GAP), cardY, CARD_W, CARD_H };
                    if (CheckCollisionPointRec(GetMousePosition(), cardRect)) {
                        int argId = shownCardIndices[i];
                        if (player) {
                            player->AddArgument(argId);
                            if (argId == 9) { // Fast Track
                                player->LevelUp();
                                player->LevelUp();
                                player->LevelUp();
                            } else if (argId == 1) { // Stat Windfall
                                // Thay vì quay lại màn hình chọn stat (phức tạp logic vòng lặp),
                                // Nhận ngay 3 level hoặc 3 exp/hp bonus để bù
                                player->Heal(100.0f);
                                player->GainMana(100.0f);
                                player->LevelUp();
                            }
                        }
                        StartWave(nextWaveAfterSelection);
                        ChangeState(GameState::TEST_GAMEPLAY);
                        break;
                    }
                }
            }
            break;
        }

        default:
            break;
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
        case GameState::MAIN_MENU:
        {
            DrawText("CHICKEN INVADERS (OOP)", screenWidth/2 - 180, 150, 30, DARKBLUE);
            
            if (DrawButton({(float)screenWidth/2 - 100, 300, 200, 50}, "PLAY")) {
                currentState = GameState::COMING_SOON;
            }
            
            if (DrawButton({(float)screenWidth/2 - 100, 380, 200, 50}, "TEST")) {
                currentState = GameState::TEST_MENU;
            }
            
            // Nút Settings (góc trên phải)
            Rectangle settingRect = {(float)screenWidth - 80, 20, 60, 60};
            DrawTexturePro(texSettingIcon, {0, 0, (float)texSettingIcon.width, (float)texSettingIcon.height}, settingRect, {0,0}, 0.0f, WHITE);
            if (CheckCollisionPointRec(GetMousePosition(), settingRect)) {
                DrawRectangleLinesEx(settingRect, 2, RED);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    previousState = currentState;
                    currentState = GameState::SETTINGS;
                }
            }
            break;
        }
        
        case GameState::SETTINGS:
        {
            DrawText("SETTINGS", screenWidth/2 - 80, 100, 40, DARKBLUE);
            DrawText("Backgrounds:", screenWidth/2 - 350, 200, 25, GRAY);
            
            if (DrawButton({(float)screenWidth/2 - 350, 250, 150, 50}, "Default")) currentBgIndex = 0;
            if (DrawButton({(float)screenWidth/2 - 150, 250, 150, 50}, "Bg 1")) currentBgIndex = 1;
            if (DrawButton({(float)screenWidth/2 + 50, 250, 150, 50}, "Bg 2")) currentBgIndex = 2;
            if (DrawButton({(float)screenWidth/2 + 250, 250, 150, 50}, "Bg 3")) currentBgIndex = 3;
            
            if (DrawButton({(float)screenWidth/2 - 100, 450, 200, 50}, "BACK")) {
                currentState = previousState;
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
            DrawText("PLAY MODE - COMING SOON!", screenWidth/2 - 200, screenHeight/2, 30, DARKGRAY);
            if (DrawButton({(float)screenWidth/2 - 100, (float)screenHeight/2 + 80, 200, 50}, "BACK")) {
                currentState = GameState::MAIN_MENU;
            }
            break;
        }

        case GameState::GAME_OVER:
        {
            DrawText("GAME OVER!", screenWidth/2 - 120, 200, 40, RED);
            DrawText(TextFormat("FINAL SCORE: %d", score), screenWidth/2 - 100, 260, 25, DARKGRAY);
            
            if (DrawButton({(float)screenWidth/2 - 120, 350, 240, 50}, "BACK TO MENU")) {
                currentState = GameState::MAIN_MENU;
                // Reset Game
                score = 0;
                activeEnemies.clear();
                activeBullets.clear();
                activeItems.clear();
                player = SpaceshipFactory::CreateSpaceship("Hypergun", 1, {(float)screenWidth/2, (float)screenHeight - 100});
                player->SetShootingBehavior(std::make_unique<HypergunShootingBehavior>());
            }
            break;
        }

        case GameState::TEST_MENU:
        {
            DrawText("TEST MODE SELECTION", screenWidth/2 - 150, 150, 30, DARKBLUE);
            
            if (DrawButton({(float)screenWidth/2 - 150, 250, 300, 50}, "TEST ENEMY")) {
                currentState = GameState::TEST_ENEMY;
            }
            
            if (DrawButton({(float)screenWidth/2 - 320, 320, 300, 50}, "TEST SPACESHIP")) {
                currentState = GameState::TEST_SPACESHIP;
                if (player) player->SetShootingBehavior(std::make_unique<HypergunShootingBehavior>());
            }
            
            if (DrawButton({ (float)screenWidth/2 + 20, 320, 300, 50 }, "TEST GAMEPLAY")) {
                ChangeState(GameState::WAVE_SELECTION);
            }
            
            if (DrawButton({(float)screenWidth/2 - 150, 460, 300, 50}, "BACK TO MAIN")) {
                currentState = GameState::MAIN_MENU;
            }
            
            // Nút Settings (góc trên phải) trong TEST_MENU
            Rectangle settingRect = {(float)screenWidth - 80, 20, 60, 60};
            DrawTexturePro(texSettingIcon, {0, 0, (float)texSettingIcon.width, (float)texSettingIcon.height}, settingRect, {0,0}, 0.0f, WHITE);
            if (CheckCollisionPointRec(GetMousePosition(), settingRect)) {
                DrawRectangleLinesEx(settingRect, 2, RED);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    previousState = currentState;
                    currentState = GameState::SETTINGS;
                }
            }
            
            break;
        }

        case GameState::WAVE_SELECTION: {
            DrawText("WAVE SELECTION", screenWidth/2 - MeasureText("WAVE SELECTION", 40)/2, 200, 40, YELLOW);
            
            // Buttons to select Wave
            DrawText(TextFormat("WAVE: %d", testSelectedWave), screenWidth/2 - 70, 300, 30, WHITE);
            if (DrawButton({ (float)screenWidth/2 - 150, 290, 50, 50 }, "<")) {
                if (testSelectedWave > 1) { testSelectedWave--; testSelectedBatch = 1; }
            }
            if (DrawButton({ (float)screenWidth/2 + 100, 290, 50, 50 }, ">")) {
                if (testSelectedWave < 4) { testSelectedWave++; testSelectedBatch = 1; }
            }

            // Buttons to select Batch
            DrawText(TextFormat("BATCH: %d", testSelectedBatch), screenWidth/2 - 70, 400, 30, WHITE);
            if (DrawButton({ (float)screenWidth/2 - 150, 390, 50, 50 }, "<")) {
                if (testSelectedBatch > 1) testSelectedBatch--;
            }
            if (DrawButton({ (float)screenWidth/2 + 100, 390, 50, 50 }, ">")) {
                int maxBatch = 3; 
                if (testSelectedBatch < maxBatch) testSelectedBatch++;
            }

            if (DrawButton({ (float)screenWidth/2 - 100, 500, 200, 50 }, "START TEST")) {
                currentWave = testSelectedWave;
                currentBatch = testSelectedBatch;
                isWaveTransitioning = true;
                waveTimer = 3.0f; 
                activeEnemies.clear();
                activeBullets.clear();
                
                // Initialize player for testing
                if (player) {
                    player = SpaceshipFactory::CreateSpaceship("Hypergun", 1, {(float)screenWidth/2, (float)screenHeight - 100});
                    player->SetShootingBehavior(std::make_unique<HypergunShootingBehavior>());
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

                ChangeState(GameState::TEST_GAMEPLAY);
            }
            if (DrawButton({ (float)screenWidth/2 - 100, 600, 200, 50 }, "BACK")) {
                ChangeState(GameState::TEST_MENU);
            }
            break;
        }

        case GameState::TEST_GAMEPLAY:
        case GameState::TEST_ENEMY:
        case GameState::TEST_SPACESHIP:
        {
            // Vẽ Player
            if (currentState != GameState::TEST_ENEMY) {
                if (player && player->IsActive()) {
                    player->Draw();
                }
            }

            // Vẽ Enemies
            if (currentState != GameState::TEST_SPACESHIP) {
                for (const auto& enemy : activeEnemies) {
                    if (enemy->IsActive()) {
                        enemy->Draw();
                    }
                }
            }

            // Vẽ Bullets
            if (currentState != GameState::TEST_ENEMY) {
                for (const auto& bullet : activeBullets) {
                    if (bullet->IsActive()) {
                        bullet->Draw();
                    }
                }
            }
            
            // Vẽ Items
            if (currentState == GameState::TEST_GAMEPLAY) {
                for (const auto& item : activeItems) {
                    if (item->IsActive()) {
                        item->Draw();
                    }
                }
            }
            
            // Nút BACK
            if (DrawButton({20, 20, 100, 40}, "BACK")) {
                currentState = GameState::TEST_MENU;
                activeEnemies.clear();
                activeBullets.clear();
                activeItems.clear();
                if (player) {
                    player->SetPosition({(float)screenWidth/2, (float)screenHeight - 100});
                }
            }
            
            // Thêm nút Settings cho các màn TEST_GAMEPLAY
            Rectangle settingRect2 = {(float)screenWidth - 80, 20, 60, 60};
            DrawTexturePro(texSettingIcon, {0, 0, (float)texSettingIcon.width, (float)texSettingIcon.height}, settingRect2, {0,0}, 0.0f, WHITE);
            if (CheckCollisionPointRec(GetMousePosition(), settingRect2)) {
                DrawRectangleLinesEx(settingRect2, 2, RED);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    previousState = currentState;
                    currentState = GameState::SETTINGS;
                }
            }
            
            if (currentState == GameState::TEST_GAMEPLAY) {
                if (isWaveTransitioning) {
                    DrawText(TextFormat("WAVE %d - BATCH %d", currentWave, currentBatch), screenWidth/2 - 200, screenHeight/2, 50, YELLOW);
                }
                
                // Di chuyển Score sang bên trái một chút để tránh đè lên nút Setting
                DrawText(TextFormat("Score: %d", score), screenWidth - 250, 40, 20, WHITE);

                if (player) {
                    float hpRatio = player->GetHp() / player->GetMaxHp();
                    if (hpRatio < 0.0f) hpRatio = 0.0f;
                    
                    DrawRectangle(20, 80, 200, 20, GRAY);
                    Color hpColor = (hpRatio > 0.5f) ? GREEN : ((hpRatio > 0.2f) ? YELLOW : RED);
                    DrawRectangle(20, 80, (int)(200 * hpRatio), 20, hpColor);
                    DrawRectangleLines(20, 80, 200, 20, DARKGRAY);
                    
                    DrawText(TextFormat("HP: %.0f/%.0f", player->GetHp(), player->GetMaxHp()), 25, 82, 16, BLACK);
                    
                    // Draw EXP Bar
                    float expRatio = player->GetCurrentExp() / player->GetMaxExp();
                    if (expRatio > 1.0f) expRatio = 1.0f;
                    DrawRectangle(20, 110, 200, 15, GRAY);
                    DrawRectangle(20, 110, (int)(200 * expRatio), 15, BLUE);
                    DrawRectangleLines(20, 110, 200, 15, DARKGRAY);
                    DrawText(TextFormat("LVL: %d  EXP: %.0f/%.0f", player->GetLevel(), player->GetCurrentExp(), player->GetMaxExp()), 25, 112, 12, WHITE);
                    
                    // Draw Mana Bar
                    float manaRatio = player->GetCurrentMana() / player->GetMaxMana();
                    if (manaRatio > 1.0f) manaRatio = 1.0f;
                    DrawRectangle(20, 135, 200, 15, GRAY);
                    DrawRectangle(20, 135, (int)(200 * manaRatio), 15, PURPLE);
                    DrawRectangleLines(20, 135, 200, 15, DARKGRAY);
                    DrawText(TextFormat("MANA: %.0f/%.0f", player->GetCurrentMana(), player->GetMaxMana()), 25, 137, 12, WHITE);
                }
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
            int numChoices = (player && player->HasArgument(2)) ? 4 : 3;
            const float CARD_W  = 320.0f;
            const float CARD_H  = 490.0f;
            const float GAP     = 40.0f;
            const float totalW  = numChoices * CARD_W + (numChoices - 1) * GAP;
            const float startX  = (screenWidth  - totalW) / 2.0f;
            const float finalY  = (screenHeight - CARD_H) / 2.0f;

            float t = selectionAnimTimer / 0.4f;
            t = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t); // ease-out cubic
            float cardY = finalY + (1.0f - t) * 350.0f;

            // 4. Tiêu đề màn hình
            bool isStat = (currentState == GameState::STAT_SELECTION);
            const char* titleText = isStat ? "CHOOSE A STAT UPGRADE" : "CHOOSE AN ARGUMENT";
            Color titleColor = isStat ? (Color){80, 200, 120, 255} : (Color){255, 180, 50, 255};
            int titleW = MeasureText(titleText, 34);
            // Vẽ bóng đổ nhẹ
            DrawText(titleText, screenWidth/2 - titleW/2 + 2, (int)(cardY - 72), 34, {0, 0, 0, 120});
            DrawText(titleText, screenWidth/2 - titleW/2, (int)(cardY - 74), 34, titleColor);

            // Phụ đề
            const char* subText = isStat ? "Choose 1 option to power up your spaceship"
                                         : "Choose a special Argument that will shape your journey";
            int subW = MeasureText(subText, 16);
            DrawText(subText, screenWidth/2 - subW/2, (int)(cardY - 38), 16, (Color){200, 200, 200, 200});

            // 5. Vẽ thẻ card
            Texture2D& cardTex = isStat ? texChiSo : texLoi;
            Vector2 mouse = GetMousePosition();

            for (int i = 0; i < numChoices; i++) {
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
                DrawText(TextFormat("%d", i + 1), (int)(cx + 35), (int)(cardY + 35), 16, (Color){180, 180, 180, 180});

                // Tên card (dòng đầu, lớn, tô đậm bằng double draw)
                int nameFontSize = 20; // Giảm lại font 20 theo yêu cầu
                int nameW = MeasureText(def.name, nameFontSize);
                float nameX = textArea.x + textArea.width / 2.0f - nameW / 2.0f;
                float nameY = textArea.y + 10.0f;
                DrawText(def.name, (int)nameX + 1, (int)nameY + 1, nameFontSize, {0, 0, 0, 100}); // shadow
                DrawText(def.name, (int)nameX, (int)nameY, nameFontSize,
                         isStat ? (Color){120, 240, 150, 255} : (Color){255, 210, 80, 255});

                // Đường phân cách
                DrawLineEx(
                    {textArea.x + 15, nameY + 35},
                    {textArea.x + textArea.width - 15, nameY + 35},
                    2.0f,
                    isStat ? (Color){120, 240, 150, 100} : (Color){255, 210, 80, 100}
                );

                // Mô tả (font nhỏ, nhiều dòng bằng \n trong chuỗi)
                // Căn lề trái của mô tả thụt vào sâu hơn (textArea.x + 25) để né hoàn toàn phần cánh trang trí
                DrawText(def.description, (int)(textArea.x + 25), (int)(nameY + 50), 16, (Color){220, 220, 220, 230});

                // 5d. Hiệu ứng hover: viền vàng/xanh sáng
                if (hovered) {
                    DrawRectangleLinesEx(cardRect, 4,
                        isStat ? (Color){80, 255, 130, 255} : (Color){255, 200, 50, 255});
                    // Text gợi ý bấm
                    const char* clickHint = "Click to select";
                    int hintW = MeasureText(clickHint, 14);
                    DrawText(clickHint, (int)(cx + CARD_W/2 - hintW/2), (int)(cardY + CARD_H - 28), 14,
                             isStat ? (Color){80, 255, 130, 220} : (Color){255, 200, 50, 220});
                }
            }
            break;
        }

        default:
            break;
    }

    EndDrawing();
}

void GameManager::CleanUp() {
    for (int i = 0; i < 4; i++) {
        UnloadTexture(texBackgrounds[i]);
    }
    if (IsWindowReady()) {
        UnloadTexture(texSettingIcon);
    UnloadTexture(texSpaceshipHypergun);
    UnloadTexture(texEnemy);
    UnloadTexture(texAsteroid1);
    UnloadTexture(texAsteroid2);
        UnloadTexture(texEnemyBullet);
        UnloadTexture(texMeat);
        UnloadTexture(texLoi);
        UnloadTexture(texChiSo);
        
        // UnloadSound(sfxShoot);
        // UnloadSound(sfxExplosion);
        // UnloadSound(sfxPickup);
        // UnloadMusicStream(bgMusic);
        // CloseAudioDevice();
        
        CloseWindow();
    }
}
