import re

def update_game_manager():
    file_path = r'e:\ChickenInvader_local\ChickenInvader\src\GameManager.cpp'
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    old_wave13 = r"""        \} else if \(batch == 3\) \{
            // Wave 1\.3: 20 chickens \(10 left, 10 right\) vertically aligned with a gap in the middle\.
            for \(int col = 0; col < 2; \+\+col\) \{
                float startX = \(col == 0\) \? -100\.0f : screenWidth \+ 100\.0f;
                float dir = \(col == 0\) \? 1\.0f : -1\.0f;
                
                for \(int i = 0; i < 10; \+\+i\) \{
                    float y = 50\.0f \+ i \* 80\.0f;
                    if \(i >= 5\) \{
                        y \+= 200\.0f; // Vùng trống ở giữa cho phi thuyền né
                    \}
                    auto enemy = EnemyFactory::CreateEnemy\(EnemyFactory::EnemyType::NORMAL_CHICKEN, \{startX, y\}, wave\);
                    enemy->SetMovementBehavior\(std::make_unique<HorizontalSweepMovement>\(dir\)\);
                    enemy->ResetEggTimer\(\);
                    AddEnemy\(std::move\(enemy\)\);
                \}
            \}
            return true;
        \}"""

    new_wave13 = """        } else if (batch == 3) {
            // Wave 1.3: 8 chickens (4 left, 4 right). Fly horizontally then up to form 2 rows.
            for (int col = 0; col < 2; ++col) {
                float startX = (col == 0) ? -100.0f : screenWidth + 100.0f;
                
                for (int i = 0; i < 4; ++i) {
                    float startY = 300.0f + i * 150.0f; // Start low
                    
                    float intersectX = (col == 0) ? (screenWidth / 2.0f - 150.0f) : (screenWidth / 2.0f + 150.0f);
                    float intersectY = startY;
                    
                    // Top row (y=100) and Bottom row (y=200)
                    float gridY = 100.0f + (i / 2) * 100.0f;
                    
                    // Left chickens take x = -300 and -100 from center
                    // Right chickens take x = 100 and 300 from center
                    float gridX = screenWidth / 2.0f + ((col == 0) ? (-300.0f + (i % 2) * 200.0f) : (100.0f + (i % 2) * 200.0f));
                    
                    auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {startX, startY}, wave);
                    enemy->SetMovementBehavior(std::make_unique<WaypointMovement>(std::vector<Vector2>{{intersectX, intersectY}, {gridX, gridY}}));
                    enemy->ResetEggTimer();
                    AddEnemy(std::move(enemy));
                }
            }
            return true;
        }"""

    new_content = re.sub(old_wave13, new_wave13, content)

    if new_content != content:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print("Updated Wave 1.3 successfully.")
    else:
        print("Could not find the target block to replace.")

if __name__ == "__main__":
    update_game_manager()
