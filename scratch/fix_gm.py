import os
import re

def fix_game_manager():
    base_dir = r"e:\ChickenInvader_local\ChickenInvader"
    cpp_path = os.path.join(base_dir, "src", "GameManager.cpp")

    with open(cpp_path, "r", encoding="utf-8") as f:
        content = f.read()

    # Ensure WaveManager.h is included at the top
    if '#include "WaveManager.h"' not in content:
        content = '#include "WaveManager.h"\n' + content

    # Replace enemyType == 4 with role == EnemyRole::ASTEROID
    content = content.replace("enemyType == 4", "role == EnemyRole::ASTEROID")
    
    # Fix the hardcoded CreateEnemy call (which looks like debug code)
    # auto enemy = EnemyFactory::CreateEnemy(EnemyFactory::EnemyType::NORMAL_CHICKEN, {x, 100.0f});
    old_hardcode = r"EnemyFactory::CreateEnemy\(EnemyFactory::EnemyType::NORMAL_CHICKEN, \{x, 100\.0f\}\);"
    new_hardcode = r"EnemyFactory::CreateEnemy(1, EnemyRole::NORMAL, EnemyStats(), {x, 100.0f});"
    content = re.sub(old_hardcode, new_hardcode, content)

    # Let's also check if there are other EnemyType:: uses
    content = re.sub(r"EnemyFactory::EnemyType::[A-Z_]+", "1", content)

    with open(cpp_path, "w", encoding="utf-8") as f:
        f.write(content)
    
    print("Fixed GameManager.cpp")

if __name__ == "__main__":
    fix_game_manager()
