#include "../include/GameManager.h"
#include "../include/SpaceshipFactory.h"
#include "../include/EnemyFactory.h"
#include "../include/WeaponStrategy.h"
#include "../include/MovementStrategy.h"
#include "../include/Bullet.h"
#include <iostream>

// Khởi tạo instance của Singleton bằng nullptr
GameManager* GameManager::instance = nullptr;

GameManager::GameManager() 
    : currentState(GameState::PLAYING), screenWidth(1280), screenHeight(720), isRunning(false), spawnTimer(2.0f) {
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

void GameManager::Init(int width, int height, const char* title) {
    screenWidth = width;
    screenHeight = height;
    
    // Khởi tạo cửa sổ Raylib
    InitWindow(screenWidth, screenHeight, title);
    SetTargetFPS(60); // Đặt tốc độ khung hình 60 FPS
    
    // Khởi tạo Player
    player = SpaceshipFactory::CreateSpaceship(SpaceshipFactory::ShipType::FIGHTER, {(float)screenWidth/2, (float)screenHeight - 100});
    player->SetShootingBehavior(std::make_unique<SingleShot>());
    
    isRunning = true;
    currentState = GameState::PLAYING; 
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
    if (currentState != GameState::PLAYING) return;

    // --- Player Logic ---
    if (player && player->IsActive()) {
        // Simple WASD movement
        Vector2 pos = player->GetPosition();
        if (IsKeyDown(KEY_W)) pos.y -= player->GetMoveSpeed() * deltaTime;
        if (IsKeyDown(KEY_S)) pos.y += player->GetMoveSpeed() * deltaTime;
        if (IsKeyDown(KEY_A)) pos.x -= player->GetMoveSpeed() * deltaTime;
        if (IsKeyDown(KEY_D)) pos.x += player->GetMoveSpeed() * deltaTime;
        
        // Boundaries
        if (pos.x < 0) pos.x = 0;
        if (pos.x > screenWidth) pos.x = screenWidth;
        if (pos.y < 0) pos.y = 0;
        if (pos.y > screenHeight) pos.y = screenHeight;
        
        player->SetPosition(pos);

        // Shooting
        if (IsKeyPressed(KEY_SPACE)) {
            player->Fire();
        }
    }

    // --- Spawner Logic ---
    spawnTimer -= deltaTime;
    if (spawnTimer <= 0.0f) {
        spawnTimer = 1.0f; // Spawn every 1 second
        float randomX = GetRandomValue(50, screenWidth - 50);
        auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {randomX, -50.0f});
        enemy->SetMovementBehavior(std::make_unique<StraightMovement>());
        AddEnemy(std::move(enemy));
    }

    // --- Update Entities ---
    for (auto& enemy : activeEnemies) {
        if (enemy->IsActive()) enemy->Update(deltaTime);
    }
    
    for (auto& bullet : activeBullets) {
        if (bullet->IsActive()) bullet->Update(deltaTime);
    }

    // --- Collision Detection ---
    for (auto& bullet : activeBullets) {
        if (!bullet->IsActive() || !bullet->IsPlayerBullet()) continue;

        for (auto& enemy : activeEnemies) {
            if (!enemy->IsActive()) continue;

            if (CheckCollisionRecs(bullet->GetHitbox(), enemy->GetHitbox())) {
                bullet->SetActive(false);
                enemy->TakeDamage(bullet->GetDamage()); // Assume this checks if HP <= 0 and calls Die()
            }
        }
    }

    for (auto& enemy : activeEnemies) {
        if (!enemy->IsActive() || !player || !player->IsActive()) continue;
        
        if (CheckCollisionRecs(enemy->GetHitbox(), player->GetHitbox())) {
            player->TakeDamage(10.0f); // Collision damage
            enemy->SetActive(false);
        }
    }

    // --- Cleanup Inactive Entities ---
    activeBullets.erase(std::remove_if(activeBullets.begin(), activeBullets.end(),
        [](const std::shared_ptr<Bullet>& b) { return !b->IsActive(); }), activeBullets.end());
        
    activeEnemies.erase(std::remove_if(activeEnemies.begin(), activeEnemies.end(),
        [](const std::shared_ptr<Enemy>& e) { return !e->IsActive(); }), activeEnemies.end());
}

void GameManager::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (currentState == GameState::PLAYING) {
        // Vẽ Player
        if (player && player->IsActive()) {
            DrawRectangleRec(player->GetHitbox(), BLUE); // Temporary visual
        }

        // Vẽ Enemies
        for (const auto& enemy : activeEnemies) {
            if (enemy->IsActive()) {
                DrawRectangleRec(enemy->GetHitbox(), RED); // Temporary visual
            }
        }

        // Vẽ Bullets
        for (const auto& bullet : activeBullets) {
            if (bullet->IsActive()) {
                bullet->Draw();
            }
        }
        
        DrawText("WASD: Move | SPACE: Shoot", 10, 10, 20, DARKGRAY);
        if (player) {
            DrawText(TextFormat("Player HP: %.0f", player->GetHp()), 10, 40, 20, RED);
        }
    }

    EndDrawing();
}

void GameManager::CleanUp() {
    if (IsWindowReady()) {
        CloseWindow();
    }
}
