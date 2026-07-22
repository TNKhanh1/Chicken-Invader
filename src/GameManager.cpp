#include "../include/GameManager.h"
#include "../include/SpaceshipFactory.h"
#include "../include/EnemyFactory.h"
#include "../include/WeaponStrategy.h"
#include "../include/MovementStrategy.h"
#include "../include/Bullet.h"
#include <iostream>
#include <cmath>

// Khởi tạo instance của Singleton bằng nullptr
GameManager* GameManager::instance = nullptr;

// --- Implement methods for entities ---
void Enemy::Update(float deltaTime) {
    // Áp dụng Strategy di chuyển
    if (movementBehavior) {
        movementBehavior->Move(position, moveSpeed, deltaTime);
    }
    
    // Logic thả trứng (bắn đạn)
    shootTimer += deltaTime;
    if (shootTimer >= 1.5f) { // Thả trứng mỗi 1.5 giây
        shootTimer = 0.0f;
        auto egg = std::make_shared<Bullet>(position, damage, 150.0f, false);
        GameManager::GetInstance()->AddBullet(egg);
    }
}

void Bullet::Draw() {
    if (!isActive) return;
    Texture2D tex = isPlayerBullet ? GameManager::GetInstance()->GetTexBulletPlayer() : GameManager::GetInstance()->GetTexEnemyBullet();
    // Vẽ đạn với kích thước phù hợp (viên đạn/trứng thường nhỏ)
    float width = isPlayerBullet ? 10.0f : 20.0f;
    float height = isPlayerBullet ? 30.0f : 25.0f;
    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, 
                   {position.x, position.y, width, height}, {width/2, height/2}, 0.0f, WHITE);
}

void Enemy::Draw() {
    if (!isActive) return;
    Texture2D tex = GameManager::GetInstance()->GetTexEnemy();
    // Tăng kích thước 50% (lên 80x80)
    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, 
                   {position.x, position.y, 80.0f, 80.0f}, {40.0f, 40.0f}, 0.0f, WHITE);
                   
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
    Texture2D tex = GameManager::GetInstance()->GetTexSpaceship();
    // Phần chứa phi thuyền thực chất nằm ở nửa dưới của ảnh (y: 201, height: 95)
    // Tăng kích thước khoảng 50% lên 90x90
    DrawTexturePro(tex, {36.0f, 201.0f, 108.0f, 95.0f}, 
                   {position.x, position.y, 90.0f, 90.0f}, {45.0f, 45.0f}, 0.0f, WHITE);
}
// -------------------------------------------

GameManager::GameManager() 
    : currentState(GameState::MAIN_MENU), previousState(GameState::MAIN_MENU), screenWidth(1280), screenHeight(720), isRunning(false), score(0), spawnTimer(2.0f) {
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
    
    // Khởi tạo các textures
    texBackgrounds[0] = LoadTexture("assets/background.jpg");
    texBackgrounds[1] = LoadTexture("assets/background.png");
    texBackgrounds[2] = LoadTexture("assets/background1.png");
    texBackgrounds[3] = LoadTexture("assets/background2.png");
    texSettingIcon = LoadTexture("assets/setting.png");
    texSpaceship = LoadTexture("assets/spaceship/SpaceShip01.png");
    texEnemy = LoadTexture("assets/enemy/chicken03.png"); 
    texBulletPlayer = LoadTexture("assets/spaceship/Bullet01_1.png");
    texEnemyBullet = LoadTexture("assets/egg.png"); 
    texMeat = LoadTexture("assets/meat.png");
    bgY = 0.0f;
    currentBgIndex = 0;
    
    // Khởi tạo Player
    player = SpaceshipFactory::CreateSpaceship(SpaceshipFactory::ShipType::FIGHTER, {(float)screenWidth/2, (float)screenHeight - 100});
    player->SetShootingBehavior(std::make_unique<SingleShot>());
    
    isRunning = true;
    currentState = GameState::MAIN_MENU; // Bắt đầu ở MAIN_MENU
}

void GameManager::Run() {
    // Vòng lặp game chính
    while (isRunning && !WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        Update(deltaTime);
        Draw();
    }
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
            bgY += 20.0f * deltaTime; // Giảm tốc độ xuống 20
            if (bgY >= 2.0f * screenHeight) bgY -= 2.0f * screenHeight;
            
            // --- Player Logic ---
            if (currentState != GameState::TEST_ENEMY) {
                if (player && player->IsActive()) {
                    Vector2 pos = player->GetPosition();
                    if (IsKeyDown(KEY_W)) pos.y -= player->GetMoveSpeed() * deltaTime;
                    if (IsKeyDown(KEY_S)) pos.y += player->GetMoveSpeed() * deltaTime;
                    if (IsKeyDown(KEY_A)) pos.x -= player->GetMoveSpeed() * deltaTime;
                    if (IsKeyDown(KEY_D)) pos.x += player->GetMoveSpeed() * deltaTime;
                    
                    if (pos.x < 0) pos.x = 0;
                    if (pos.x > screenWidth) pos.x = screenWidth;
                    if (pos.y < 0) pos.y = 0;
                    if (pos.y > screenHeight) pos.y = screenHeight;
                    
                    player->SetPosition(pos);

                    if (IsKeyPressed(KEY_SPACE)) {
                        player->Fire();
                    }
                }
            }

            // --- Spawner Logic ---
            if (currentState != GameState::TEST_SPACESHIP) {
                spawnTimer -= deltaTime;
                if (spawnTimer <= 0.0f) {
                    spawnTimer = 2.0f; 
                    float randomX = GetRandomValue(50, screenWidth - 50);
                    auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {randomX, -50.0f});
                    enemy->SetMovementBehavior(std::make_unique<StraightMovement>());
                    AddEnemy(std::move(enemy));
                }
            }

            // --- Update Entities ---
            for (auto& meat : activeMeats) {
                if (meat.active) {
                    meat.time += deltaTime;
                    meat.velocity.y += 400.0f * deltaTime; // Gravity
                    meat.position.y += meat.velocity.y * deltaTime;
                    meat.position.x += meat.velocity.x * deltaTime + sin(meat.time * 5.0f) * 60.0f * deltaTime;
                    if (meat.position.y > screenHeight + 50) meat.active = false;
                }
            }

            for (auto& enemy : activeEnemies) {
                if (enemy->IsActive()) enemy->Update(deltaTime);
            }
            
            for (auto& bullet : activeBullets) {
                if (bullet->IsActive()) bullet->Update(deltaTime);
            }

            // --- Collision Detection ---
            if (currentState == GameState::TEST_GAMEPLAY) {
                for (auto& bullet : activeBullets) {
                    if (!bullet->IsActive()) continue;

                    if (bullet->IsPlayerBullet()) {
                        for (auto& enemy : activeEnemies) {
                            if (!enemy->IsActive()) continue;

                            if (CheckCollisionRecs(bullet->GetHitbox(), enemy->GetHitbox())) {
                                bullet->SetActive(false);
                                enemy->TakeDamage(bullet->GetDamage()); 
                                
                                if (!enemy->IsActive()) {
                                    AddScore(enemy->GetPointValue());
                                    activeMeats.push_back({enemy->GetPosition(), { (float)GetRandomValue(-100, 100), -200.0f }, 0.0f, true});
                                }
                            }
                        }
                    } else {
                        // Đạn của quái bắn vào Player
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
                    
                    if (CheckCollisionRecs(enemy->GetHitbox(), player->GetHitbox())) {
                        player->TakeDamage(10.0f); 
                        enemy->SetActive(false);
                    }
                }
                
                // Va chạm Meat và Player
                if (player && player->IsActive()) {
                    for (auto& meat : activeMeats) {
                        if (!meat.active) continue;
                        Rectangle meatRect = {meat.position.x - 20, meat.position.y - 20, 40, 40};
                        if (CheckCollisionRecs(meatRect, player->GetHitbox())) {
                            meat.active = false;
                            AddScore(50); // Ăn đùi gà được 50 điểm
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
                
            activeMeats.erase(std::remove_if(activeMeats.begin(), activeMeats.end(),
                [](const MeatItem& m) { return !m.active; }), activeMeats.end());
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
                activeMeats.clear();
                player = SpaceshipFactory::CreateSpaceship(SpaceshipFactory::ShipType::FIGHTER, {(float)screenWidth/2, (float)screenHeight - 100});
                player->SetShootingBehavior(std::make_unique<SingleShot>());
            }
            break;
        }

        case GameState::TEST_MENU:
        {
            DrawText("TEST MODE SELECTION", screenWidth/2 - 150, 150, 30, DARKBLUE);
            
            if (DrawButton({(float)screenWidth/2 - 150, 250, 300, 50}, "TEST ENEMY")) {
                currentState = GameState::TEST_ENEMY;
            }
            
            if (DrawButton({(float)screenWidth/2 - 150, 320, 300, 50}, "TEST SPACESHIP")) {
                currentState = GameState::TEST_SPACESHIP;
            }
            
            if (DrawButton({(float)screenWidth/2 - 150, 390, 300, 50}, "TEST GAMEPLAY")) {
                currentState = GameState::TEST_GAMEPLAY;
                // Reset máu để chắc chắn không chết ngay
                if (player) {
                    player = SpaceshipFactory::CreateSpaceship(SpaceshipFactory::ShipType::FIGHTER, {(float)screenWidth/2, (float)screenHeight - 100});
                    player->SetShootingBehavior(std::make_unique<SingleShot>());
                }
            }
            
            if (DrawButton({(float)screenWidth/2 - 150, 480, 300, 50}, "BACK TO MAIN")) {
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
            
            // Vẽ Meats
            if (currentState == GameState::TEST_GAMEPLAY) {
                for (const auto& meat : activeMeats) {
                    if (meat.active) {
                        DrawTexturePro(texMeat, {0, 0, (float)texMeat.width, (float)texMeat.height},
                                       {meat.position.x, meat.position.y, 40.0f, 40.0f}, {20.0f, 20.0f}, 0.0f, WHITE);
                    }
                }
            }
            
            // Nút BACK
            if (DrawButton({20, 20, 100, 40}, "BACK")) {
                currentState = GameState::TEST_MENU;
                activeEnemies.clear();
                activeBullets.clear();
                activeMeats.clear();
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
        UnloadTexture(texSpaceship);
        UnloadTexture(texEnemy);
        UnloadTexture(texBulletPlayer);
        UnloadTexture(texEnemyBullet);
        UnloadTexture(texMeat);
        CloseWindow();
    }
}
