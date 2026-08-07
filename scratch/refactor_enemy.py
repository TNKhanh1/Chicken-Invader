import os

def refactor_enemy():
    base_dir = r"e:\ChickenInvader_local\ChickenInvader"
    enemy_h = os.path.join(base_dir, "include", "Enemy.h")
    enemy_cpp = os.path.join(base_dir, "src", "Enemy.cpp")
    factory_h = os.path.join(base_dir, "include", "EnemyFactory.h")
    factory_cpp = os.path.join(base_dir, "src", "EnemyFactory.cpp")

    # 1. Update Enemy.h
    with open(enemy_h, "r", encoding="utf-8") as f:
        content = f.read()
    
    new_structs = """
#include <vector>

enum class EnemyRole {
    NORMAL,
    SWARM,
    TANK,
    BOSS,
    ASTEROID
};

struct EnemyStats {
    float hp = 100.0f;
    float damage = 20.0f;
    float armor = 0.0f;
    float speed = 100.0f;
    float eggRate = 3.0f;
    int score = 10;
};
"""
    if "enum class EnemyRole" not in content:
        content = content.replace("#include <vector>", new_structs)
    
    content = content.replace("Enemy(Vector2 pos, float hp, float dmg, float arm, float spd, int points);", 
                              "Enemy(int visualId, EnemyRole role, const EnemyStats& stats, Vector2 pos);")
    content = content.replace("int enemyType = 0;", 
                              "int visualId = 1;\n    EnemyRole role = EnemyRole::NORMAL;\n    EnemyStats stats;")
    
    with open(enemy_h, "w", encoding="utf-8") as f:
        f.write(content)
        
    # 2. Update EnemyFactory.h
    with open(factory_h, "r", encoding="utf-8") as f:
        content = f.read()
        
    content = content.replace("""    enum class EnemyType {
        NORMAL_CHICKEN,
        SWARM_CHICKEN, // Máu ít, rớt trứng siêu chậm (bay lượn khó)
        TANK_CHICKEN,
        ASTEROID
    };""", "")
    
    content = content.replace("""    static std::unique_ptr<Enemy> CreateEnemy(EnemyType type, Vector2 position, int currentWave = 1);""", 
                              """    static std::unique_ptr<Enemy> CreateEnemy(int visualId, EnemyRole role, const EnemyStats& stats, Vector2 position);""")
                              
    with open(factory_h, "w", encoding="utf-8") as f:
        f.write(content)
        
    # 3. Update EnemyFactory.cpp
    factory_cpp_content = """#include "EnemyFactory.h"

std::unique_ptr<Enemy> EnemyFactory::CreateEnemy(int visualId, EnemyRole role, const EnemyStats& stats, Vector2 position) {
    auto enemy = std::make_unique<Enemy>(visualId, role, stats, position);
    return enemy;
}
"""
    with open(factory_cpp, "w", encoding="utf-8") as f:
        f.write(factory_cpp_content)

    print("Refactored Enemy headers and factory.")

if __name__ == "__main__":
    refactor_enemy()
