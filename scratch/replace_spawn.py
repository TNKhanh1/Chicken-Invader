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
            // Wave 1.3: 20 chickens (10 left, 10 right), sweep across horizontally
            for (int i = 0; i < 10; ++i) {
                // Cột trái bay sang phải
                float x_left = -100.0f - (i % 2) * 100.0f;
                float y_left = 100.0f + (i / 2) * 100.0f;
                auto enemyL = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {x_left, y_left}, wave);
                enemyL->SetMovementBehavior(std::make_unique<HorizontalSweepMovement>(1.0f));
                enemyL->ResetEggTimer();
                AddEnemy(std::move(enemyL));
                
                // Cột phải bay sang trái
                float x_right = screenWidth + 100.0f + (i % 2) * 100.0f;
                float y_right = 100.0f + (i / 2) * 100.0f;
                auto enemyR = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {x_right, y_right}, wave);
                enemyR->SetMovementBehavior(std::make_unique<HorizontalSweepMovement>(-1.0f));
                enemyR->ResetEggTimer();
                AddEnemy(std::move(enemyR));
            }
            return true;
        }
    } else if (wave == 2) {
        if (batch == 1) {
            // Wave 2: Asteroid rain for 15 seconds
            // Spawn 50 asteroids spread out vertically
            for (int i = 0; i < 50; i++) {
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
    } else if (wave == 3) {
        if (batch == 1) {
            // Wave 3.1: 2 V shapes (one down, one up), intersecting and forming a grid
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
    } else if (wave == 4) {
        if (batch == 1) {
            // Wave 4.1: 2 rows of chickens as bait
            float startX = screenWidth / 2.0f;
            for (int r = 0; r < 2; ++r) {
                for (int c = 0; c < 10; ++c) {
                    float offsetX = (c - 4.5f) * 120.0f;
                    float x = startX + offsetX;
                    float y = -100.0f; 
                    float targetY = 100.0f + r * 100.0f;
                    auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {x, y}, wave);
                    enemy->SetMovementBehavior(std::make_unique<HorizontalBounceMovement>(targetY, 150.0f, 1.0f));
                    enemy->ResetEggTimer();
                    AddEnemy(std::move(enemy));
                }
            }
            return true;
        } else if (batch == 2) {
            // Wave 4.2: Targeted asteroid drops on player + some Tank Chickens
            if (player) {
                float px = player->GetPosition().x;
                auto asteroid = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::ASTEROID, {px, -100.0f}, wave);
                asteroid->SetMovementBehavior(std::make_unique<MeteorDiveMovement>());
                asteroid->asteroidVariant = 2; // Flame asteroid
                asteroid->canShoot = false;
                AddEnemy(std::move(asteroid));
            }
            
            // Spawn some Tank chickens too
            for (int i = 0; i < 5; ++i) {
                float x = 200.0f + i * 300.0f;
                auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::TANK_CHICKEN, {x, -100.0f}, wave);
                enemy->SetMovementBehavior(std::make_unique<StraightMovement>());
                enemy->ResetEggTimer();
                AddEnemy(std::move(enemy));
            }
            return true;
        }
    }
    
    // Nếu wave > 4 hoặc batch > 3, trả về false để biết là wave này đã kết thúc
    return false;
}"""

    # We need to replace the content of SpawnWaveBatch
    pattern = re.compile(r'bool GameManager::SpawnWaveBatch\(int wave, int batch\) \{.*?\n\}\n', re.DOTALL)
    new_content = pattern.sub(new_func + '\n', content)

    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(new_content)
    
    print("Updated SpawnWaveBatch successfully.")

if __name__ == "__main__":
    update_game_manager()
