import re

def update_game_manager():
    file_path = r'e:\ChickenInvader_local\ChickenInvader\src\GameManager.cpp'
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    new_func = """bool GameManager::SpawnWaveBatch(int wave, int batch) {
    if (wave == 1) {
        if (batch == 1) {
            // Wave 1.1: 15 chickens in 3 rows, fly down and hover
            float startX = screenWidth / 2.0f;
            for (int r = 0; r < 3; ++r) {
                for (int c = 0; c < 5; ++c) {
                    float offsetX = (c - 2) * 150.0f;
                    float x = startX + offsetX;
                    float y = -300.0f + r * 100.0f; 
                    float targetY = 100.0f + r * 100.0f;
                    auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {x, y}, wave);
                    // bounceRange = 0 -> hover in place
                    enemy->SetMovementBehavior(std::make_unique<HorizontalBounceMovement>(targetY, 0.0f, 1.0f));
                    enemy->ResetEggTimer();
                    AddEnemy(std::move(enemy));
                }
            }
            return true;
        } else if (batch == 2) {
            // Wave 1.2: 15 chickens in 3 rows, fly down and move left/right
            float startX = screenWidth / 2.0f;
            for (int r = 0; r < 3; ++r) {
                for (int c = 0; c < 5; ++c) {
                    float offsetX = (c - 2) * 150.0f;
                    float x = startX + offsetX;
                    float y = -300.0f + r * 100.0f; 
                    float targetY = 100.0f + r * 100.0f;
                    auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {x, y}, wave);
                    // bounceRange = 300 -> sweep left/right
                    enemy->SetMovementBehavior(std::make_unique<HorizontalBounceMovement>(targetY, 300.0f, 1.0f));
                    enemy->ResetEggTimer();
                    AddEnemy(std::move(enemy));
                }
            }
            return true;
        } else if (batch == 3) {
            // Wave 1.3: 20 chickens (10 left, 10 right) vertically aligned with a gap in the middle.
            for (int col = 0; col < 2; ++col) {
                float startX = (col == 0) ? -100.0f : screenWidth + 100.0f;
                float dir = (col == 0) ? 1.0f : -1.0f;
                
                for (int i = 0; i < 10; ++i) {
                    float y = 50.0f + i * 80.0f;
                    if (i >= 5) {
                        y += 200.0f; // Vùng trống ở giữa cho phi thuyền né
                    }
                    auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {startX, y}, wave);
                    enemy->SetMovementBehavior(std::make_unique<HorizontalSweepMovement>(dir));
                    enemy->ResetEggTimer();
                    AddEnemy(std::move(enemy));
                }
            }
            return true;
        }
    } else if (wave == 2) {
        if (batch == 1) {
            // Wave 2.1: Double V-shape (2 lớp), úp ngược xuống, hover
            for (int layer = 0; layer < 2; ++layer) {
                for (int i = 0; i < 11; ++i) { // 11 chickens per V
                    float x = screenWidth / 2.0f + (i - 5.0f) * 100.0f;
                    float y = -100.0f - std::abs(i - 5.0f) * 80.0f - layer * 150.0f;
                    
                    float targetY = 150.0f + std::abs(i - 5.0f) * 80.0f + layer * 150.0f;
                    
                    auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {x, y}, wave);
                    enemy->SetMovementBehavior(std::make_unique<HorizontalBounceMovement>(targetY, 0.0f, 1.0f)); // Hover
                    enemy->ResetEggTimer();
                    AddEnemy(std::move(enemy));
                }
            }
            return true;
        } else if (batch == 2) {
            // Wave 2.2: Intersecting V-shapes (Top and Bottom) into grid
            for (int i = 0; i < 10; ++i) {
                // Top V
                float topStartX = screenWidth / 2.0f + (i - 4.5f) * 80.0f;
                float topStartY = -100.0f - std::abs(i - 4.5f) * 80.0f;
                float intersectX = topStartX;
                float intersectY = screenHeight / 2.0f;
                float gridX_top = screenWidth / 2.0f - 200.0f + (i % 5) * 100.0f;
                float gridY_top = 100.0f + (i / 5) * 100.0f;
                
                auto enemyTop = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::SWARM_CHICKEN, {topStartX, topStartY}, wave);
                enemyTop->SetMovementBehavior(std::make_unique<WaypointMovement>(std::vector<Vector2>{{intersectX, intersectY}, {gridX_top, gridY_top}}));
                enemyTop->ResetEggTimer();
                AddEnemy(std::move(enemyTop));
                
                // Bottom V
                float botStartX = screenWidth / 2.0f + (i - 4.5f) * 80.0f;
                float botStartY = screenHeight + 100.0f + std::abs(i - 4.5f) * 80.0f;
                float gridX_bot = screenWidth / 2.0f - 200.0f + (i % 5) * 100.0f;
                float gridY_bot = 300.0f + (i / 5) * 100.0f;
                
                auto enemyBot = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::SWARM_CHICKEN, {botStartX, botStartY}, wave);
                enemyBot->SetMovementBehavior(std::make_unique<WaypointMovement>(std::vector<Vector2>{{intersectX, intersectY}, {gridX_bot, gridY_bot}}));
                enemyBot->ResetEggTimer();
                AddEnemy(std::move(enemyBot));
            }
            return true;
        }
    } else if (wave == 3) {
        if (batch == 1) {
            // Wave 3: Asteroid Rain (Reduced density: 30 asteroids over 15 seconds)
            for (int i = 0; i < 30; i++) {
                float x = GetRandomValue(100, screenWidth - 100);
                float y = -100.0f - GetRandomValue(0, 4500); // 15 seconds * 300 speed
                auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::ASTEROID, {x, y}, wave);
                enemy->SetMovementBehavior(std::make_unique<MeteorDiveMovement>());
                enemy->asteroidVariant = GetRandomValue(1, 2);
                enemy->canShoot = false;
                AddEnemy(std::move(enemy));
            }
            return true;
        }
    } else if (wave == 4) {
        if (batch == 1) {
            // Wave 4.1: Massive V-shape of 15 chickens
            for (int i = 0; i < 15; ++i) {
                float x = screenWidth / 2.0f + (i - 7.0f) * 100.0f;
                float y = -100.0f - std::abs(i - 7.0f) * 80.0f;
                float targetY = 150.0f + std::abs(i - 7.0f) * 80.0f;
                auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {x, y}, wave);
                enemy->SetMovementBehavior(std::make_unique<HorizontalBounceMovement>(targetY, 150.0f, 1.0f));
                enemy->ResetEggTimer();
                AddEnemy(std::move(enemy));
            }
            return true;
        } else if (batch == 2) {
            // Wave 4.2: Many Tank Chickens, and 3 targeted flame asteroids that drop DURING the fight
            // Spawn Tank chickens
            for (int i = 0; i < 8; ++i) {
                float x = 100.0f + i * 200.0f;
                auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::TANK_CHICKEN, {x, -100.0f}, wave);
                enemy->SetMovementBehavior(std::make_unique<StraightMovement>());
                enemy->ResetEggTimer();
                AddEnemy(std::move(enemy));
            }
            
            // Spawn Targeted Asteroids with delays (Y offset)
            if (player) {
                float px = player->GetPosition().x;
                // Asteroid 1 drops at 3 seconds
                auto ast1 = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::ASTEROID, {px, -900.0f}, wave);
                ast1->SetMovementBehavior(std::make_unique<MeteorDiveMovement>());
                ast1->asteroidVariant = 2; ast1->canShoot = false;
                AddEnemy(std::move(ast1));
                
                // Asteroid 2 drops at 6 seconds
                auto ast2 = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::ASTEROID, {screenWidth/2.0f, -1800.0f}, wave);
                ast2->SetMovementBehavior(std::make_unique<MeteorDiveMovement>());
                ast2->asteroidVariant = 2; ast2->canShoot = false;
                AddEnemy(std::move(ast2));
                
                // Asteroid 3 drops at 9 seconds
                auto ast3 = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::ASTEROID, {px + 150.0f, -2700.0f}, wave);
                ast3->SetMovementBehavior(std::make_unique<MeteorDiveMovement>());
                ast3->asteroidVariant = 2; ast3->canShoot = false;
                AddEnemy(std::move(ast3));
            }
            return true;
        }
    }
    
    // Nếu wave > 4 hoặc batch > 3, trả về false để biết là wave này đã kết thúc
    return false;
}"""

    pattern = re.compile(r'bool GameManager::SpawnWaveBatch\(int wave, int batch\) \{.*?\n\}\n', re.DOTALL)
    new_content = pattern.sub(new_func + '\n', content)

    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(new_content)
    
    print("Updated SpawnWaveBatch successfully.")

if __name__ == "__main__":
    update_game_manager()
