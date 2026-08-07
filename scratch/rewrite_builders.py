import os

def rewrite_builders():
    base_dir = r"e:\ChickenInvader_local\ChickenInvader"
    h_path = os.path.join(base_dir, "include", "FormationBuilder.h")
    cpp_path = os.path.join(base_dir, "src", "FormationBuilder.cpp")
    wm_cpp_path = os.path.join(base_dir, "src", "WaveManager.cpp")

    # 1. FormationBuilder.h
    h_content = """#ifndef FORMATIONBUILDER_H
#define FORMATIONBUILDER_H

#include <vector>
#include "raylib.h"
#include <string>
#include "json.hpp"

using json = nlohmann::json;

struct SpawnData {
    Vector2 startPos;
    Vector2 targetPos;
    std::vector<Vector2> waypoints;
    int index; // which enemy is this in the batch (useful for alternating logic)
};

class FormationBuilder {
public:
    static std::vector<SpawnData> BuildFromJSON(const json& layout, int count, float screenWidth, float screenHeight);
};

#endif
"""

    # 2. FormationBuilder.cpp
    cpp_content = """#include "FormationBuilder.h"
#include <cmath>

std::vector<SpawnData> FormationBuilder::BuildFromJSON(const json& layout, int count, float screenWidth, float screenHeight) {
    std::vector<SpawnData> data;
    std::string type = layout["type"];
    
    if (type == "HORIZONTAL_SWEEP") {
        int dir = layout["dir"];
        float spacingX = layout["spacing_x"];
        float spacingY = layout["spacing_y"];
        int rows = layout["rows"];
        float startY = layout["start_y"];
        for (int i = 0; i < count; ++i) {
            float x = (dir == -1) ? (screenWidth + 100.0f + i * spacingX) : (-100.0f - i * spacingX);
            float y = startY + (i % rows) * spacingY;
            data.push_back({{x, y}, {x, y}, {}, i});
        }
    } 
    else if (type == "GRID") {
        int rows = layout["rows"];
        int cols = layout["cols"];
        float spacingX = layout["spacing_x"];
        float spacingY = layout["spacing_y"];
        float startY = layout["start_y"];
        float targetBaseY = layout["target_base_y"];
        float startX = screenWidth / 2.0f;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                float offsetX = (c - (cols - 1) / 2.0f) * spacingX;
                float x = startX + offsetX;
                float y = startY + r * spacingY;
                float targetY = targetBaseY + r * spacingY;
                data.push_back({{x, y}, {x, targetY}, {}, r * cols + c});
            }
        }
    }
    else if (type == "GRID_ZIGZAG") {
        int cols = layout["cols"];
        float spacingX = layout["spacing_x"];
        float spacingY = layout["spacing_y"];
        float startX = layout["start_x"];
        float startY = layout["start_y"];
        for (int i = 0; i < count; ++i) {
            float x = startX + (i % cols) * spacingX;
            float y = startY - (i / cols) * spacingY;
            data.push_back({{x, y}, {x, y}, {}, i});
        }
    }
    else if (type == "DIAGONAL") {
        float spacingX = layout["spacing_x"];
        float spacingY = layout["spacing_y"];
        float startX = layout["start_x"];
        float startY = layout["start_y"];
        for (int i = 0; i < count; ++i) {
            float x = startX + i * spacingX;
            float y = startY - i * spacingY;
            data.push_back({{x, y}, {x, y}, {}, i});
        }
    }
    else if (type == "RANDOM_RAIN") {
        float startY = layout["start_y"];
        for (int i = 0; i < count; ++i) {
            float x = (float)GetRandomValue(100, (int)screenWidth - 100);
            float y = startY - i * 150.0f;
            data.push_back({{x, y}, {x, y}, {}, i});
        }
    }
    else if (type == "LINEAR") {
        float startX = layout["start_x"];
        float spacingX = layout["spacing_x"];
        float startY = layout["start_y"];
        for (int i = 0; i < count; ++i) {
            float x = startX + i * spacingX;
            data.push_back({{x, startY}, {x, startY}, {}, i});
        }
    }
    else if (type == "WAVE_3_3_SPECIAL") {
        // Cột trái: 5 asteroid
        for (int i = 0; i < 5; ++i) {
            float ax = 100.0f + i * 80.0f;
            float ay = -200.0f - i * 120.0f;
            data.push_back({{ax, ay}, {ax, ay}, {}, i});
        }
        // Cột phải: 5 asteroid
        for (int i = 0; i < 5; ++i) {
            float ax = screenWidth - 100.0f - i * 80.0f;
            float ay = -300.0f - i * 120.0f;
            data.push_back({{ax, ay}, {ax, ay}, {}, i + 5});
        }
        // 3 Tank
        for (int i = 0; i < 3; ++i) {
            float tx = 250.0f + i * 450.0f;
            float ty = -150.0f - i * 200.0f;
            data.push_back({{tx, ty}, {tx, ty}, {}, i + 10});
        }
    }
    else if (type == "ALTERNATING_SWEEP") {
        for (int i = 0; i < count; ++i) {
            float x = (i % 2 == 0) ? (-100.0f - i * 100.0f) : (screenWidth + 100.0f + i * 100.0f);
            float y = 100.0f + (i % 5) * 80.0f;
            data.push_back({{x, y}, {x, y}, {}, i});
        }
    }
    else if (type == "RANDOM_X_RAIN") {
        float startY = layout["start_y"];
        float spacingY = layout["spacing_y"];
        for (int i = 0; i < count; ++i) {
            float x = (float)GetRandomValue(100, (int)screenWidth - 100);
            float y = startY - i * spacingY;
            data.push_back({{x, y}, {x, y}, {}, i});
        }
    }
    
    return data;
}
"""

    # 3. Modify WaveManager.cpp to parse the new movement and roles properly.
    with open(wm_cpp_path, "r", encoding="utf-8") as f:
        wm_content = f.read()
    
    # We need to completely replace WaveManager::SpawnBatch
    import re
    spawn_batch_pattern = re.compile(r'bool WaveManager::SpawnBatch\(int waveId, int batchId\).*', re.DOTALL)
    
    new_spawn_batch = """bool WaveManager::SpawnBatch(int waveId, int batchId) {
    if (stageData.empty() || !stageData.contains("waves")) return false;
    
    auto gm = GameManager::GetInstance();
    float sw = gm->GetScreenWidth();
    float sh = gm->GetScreenHeight();

    for (const auto& w : stageData["waves"]) {
        if (w["wave_id"] == waveId) {
            for (const auto& b : w["batches"]) {
                if (b["batch_id"] == batchId) {
                    
                    int visualId = b["visual_id"];
                    std::string roleStr = b["role"];
                    auto layout = b["layout"];
                    auto mov = b["movement"];
                    
                    std::vector<SpawnData> spawnPoints = FormationBuilder::BuildFromJSON(layout, b["count"], sw, sh);
                    
                    std::string movType = mov["type"];

                    for (const auto& pt : spawnPoints) {
                        EnemyRole role = EnemyRole::NORMAL;
                        if (roleStr == "SWARM") role = EnemyRole::SWARM;
                        else if (roleStr == "TANK") role = EnemyRole::TANK;
                        else if (roleStr == "ASTEROID") role = EnemyRole::ASTEROID;
                        else if (roleStr == "MIXED") {
                            // Custom logic based on the original code
                            if (layout["type"] == "GRID" && waveId == 2 && batchId == 3) {
                                role = (pt.index % 3 == 0) ? EnemyRole::SWARM : EnemyRole::NORMAL;
                            } else if (layout["type"] == "ALTERNATING_SWEEP" && waveId == 4 && batchId == 1) {
                                role = (pt.index % 2 == 0) ? EnemyRole::NORMAL : EnemyRole::SWARM;
                            } else if (layout["type"] == "RANDOM_X_RAIN" && waveId == 4 && batchId == 3) {
                                if (pt.index % 4 == 0) role = EnemyRole::TANK;
                                else if (pt.index % 4 == 1) role = EnemyRole::SWARM;
                                else role = EnemyRole::NORMAL;
                            }
                        }
                        else if (roleStr == "ASTEROID_FLAME_TANK_MIX") {
                            role = (pt.index < 10) ? EnemyRole::ASTEROID : EnemyRole::TANK;
                        }

                        EnemyStats stats;
                        auto s = b["stats"];
                        stats.hp = s["hp"];
                        stats.damage = s["damage"];
                        stats.armor = s["armor"];
                        stats.speed = s["speed"];
                        stats.eggRate = s["egg_rate"];
                        stats.score = s["score"];

                        auto enemy = EnemyFactory::CreateEnemy(visualId, role, stats, pt.startPos);
                        
                        // Assign Movement
                        if (movType == "HORIZONTAL_SWEEP") {
                            enemy->SetMovementBehavior(std::make_unique<HorizontalSweepMovement>(mov["dir"]));
                        } else if (movType == "HORIZONTAL_BOUNCE") {
                            float drift = mov.contains("drift") ? (float)mov["drift"] : 150.0f;
                            enemy->SetMovementBehavior(std::make_unique<HorizontalBounceMovement>(pt.targetPos.y, drift, 1.0f));
                        } else if (movType == "VERTICAL_ZIGZAG") {
                            enemy->SetMovementBehavior(std::make_unique<VerticalZigzagMovement>());
                        } else if (movType == "SINE_ZIGZAG") {
                            enemy->SetMovementBehavior(std::make_unique<SineZigzagMovement>());
                        } else if (movType == "METEOR_DIVE") {
                            enemy->SetMovementBehavior(std::make_unique<MeteorDiveMovement>());
                        } else if (movType == "STRAIGHT") {
                            enemy->SetMovementBehavior(std::make_unique<StraightMovement>());
                        } else if (movType == "ALTERNATING_SWEEP") {
                            float dir = (pt.index % 2 == 0) ? 1.0f : -1.0f;
                            enemy->SetMovementBehavior(std::make_unique<HorizontalSweepMovement>(dir));
                        } else if (movType == "SPECIAL") {
                            if (pt.index < 10) enemy->SetMovementBehavior(std::make_unique<MeteorDiveMovement>());
                            else enemy->SetMovementBehavior(std::make_unique<StraightMovement>());
                        } else if (movType == "MIXED_RAIN") {
                            if (role == EnemyRole::TANK) enemy->SetMovementBehavior(std::make_unique<StraightMovement>());
                            else enemy->SetMovementBehavior(std::make_unique<SineZigzagMovement>());
                        }

                        // Specific tweaks
                        if (role == EnemyRole::ASTEROID) {
                            if (layout.contains("asteroid_variant")) {
                                enemy->asteroidVariant = layout["asteroid_variant"];
                            } else if (roleStr == "ASTEROID_FLAME_TANK_MIX") {
                                enemy->asteroidVariant = 2; // Flame asteroids
                            }
                            enemy->canShoot = false;
                        }
                        if (roleStr == "ASTEROID_FLAME_TANK_MIX" && role == EnemyRole::TANK) {
                            visualId = 5; // force visual to Tank
                            // Hack: Re-create enemy if visualId changes for mixed batches
                            enemy = EnemyFactory::CreateEnemy(visualId, role, stats, pt.startPos);
                            enemy->SetMovementBehavior(std::make_unique<StraightMovement>());
                        }
                        
                        enemy->ResetEggTimer();
                        gm->AddEnemy(std::move(enemy));
                    }
                    return true;
                }
            }
        }
    }
    return false;
}
"""

    wm_content = re.sub(spawn_batch_pattern, new_spawn_batch, wm_content)
    with open(wm_cpp_path, "w", encoding="utf-8") as f:
        f.write(wm_content)

    with open(h_path, "w", encoding="utf-8") as f:
        f.write(h_content)
    with open(cpp_path, "w", encoding="utf-8") as f:
        f.write(cpp_content)
        
    print("Rewritten FormationBuilder and WaveManager successfully.")

if __name__ == "__main__":
    rewrite_builders()
