#include "WaveManager.h"
#include "GameManager.h"
#include "Spaceship.h"
#include "EnemyFactory.h"
#include "FormationBuilder.h"
#include "StraightMovement.h"
#include "HorizontalBounceMovement.h"
#include "WaypointMovement.h"
#include "MeteorDiveMovement.h"
#include "HorizontalSweepMovement.h"
#include <fstream>
#include <iostream>

WaveManager* WaveManager::instance = nullptr;

WaveManager::WaveManager() {}

WaveManager* WaveManager::GetInstance() {
    if (!instance) instance = new WaveManager();
    return instance;
}

bool WaveManager::LoadStage(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filepath << std::endl;
        return false;
    }
    try {
        file >> stageData;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "JSON Parsing error: " << e.what() << std::endl;
        return false;
    }
}

void WaveManager::Update(float deltaTime) {
    auto gm = GameManager::GetInstance();
    
    for (auto it = delayedSpawns.begin(); it != delayedSpawns.end();) {
        it->delayTimer -= deltaTime;
        if (it->delayTimer <= 0.0f) {
            // Parse and spawn
            auto b = it->batchData;
            int visualId = b["visual_id"];
            
            std::string roleStr = b["role"];
            EnemyRole role = EnemyRole::NORMAL;
            if (roleStr == "SWARM") role = EnemyRole::SWARM;
            else if (roleStr == "TANK") role = EnemyRole::TANK;
            else if (roleStr == "BOSS") role = EnemyRole::BOSS;
            else if (roleStr == "ASTEROID") role = EnemyRole::ASTEROID;

            EnemyStats stats;
            auto s = b["stats"];
            stats.hp = s["hp"];
            stats.damage = s["damage"];
            stats.armor = s["armor"];
            stats.speed = s["speed"];
            stats.eggRate = s["egg_rate"];
            stats.score = s["score"];

            auto layout = b["layout"];
            std::string type = layout["type"];
            
            std::vector<SpawnData> spawnPoints;
            float sw = gm->GetScreenWidth();
            
            if (type == "TARGETED_PLAYER") {
                float startY = layout["start_y"];
                float px = sw / 2.0f; // fallback
                if (gm->GetPlayer()) {
                    px = gm->GetPlayer()->GetPosition().x;
                }
                spawnPoints = FormationBuilder::BuildTargetedPlayer(startY, px);
            }
            
            for (const auto& pt : spawnPoints) {
                auto enemy = EnemyFactory::CreateEnemy(visualId, role, stats, pt.startPos);
                
                auto mov = b["movement"];
                std::string movType = mov["type"];
                if (movType == "METEOR_DIVE") {
                    enemy->SetMovementBehavior(std::make_unique<MeteorDiveMovement>());
                }
                
                if (role == EnemyRole::ASTEROID && layout.contains("asteroid_variant")) {
                    enemy->asteroidVariant = layout["asteroid_variant"];
                    enemy->canShoot = false;
                }
                
                enemy->ResetEggTimer();
                gm->AddEnemy(std::move(enemy));
            }
            
            it = delayedSpawns.erase(it);
        } else {
            ++it;
        }
    }
}

bool WaveManager::SpawnBatch(int waveId, int batchId) {
    if (stageData.empty() || !stageData.contains("waves")) return false;
    
    auto gm = GameManager::GetInstance();
    float sw = gm->GetScreenWidth();
    float sh = gm->GetScreenHeight();
    bool found = false;

    for (const auto& w : stageData["waves"]) {
        if (w["wave_id"] == waveId) {
            for (const auto& b : w["batches"]) {
                if (b["batch_id"] == batchId) {
                    
                    int visualId = b["visual_id"];
                    std::string roleStr = b["role"];
                    EnemyRole role = EnemyRole::NORMAL;
                    if (roleStr == "SWARM") role = EnemyRole::SWARM;
                    else if (roleStr == "TANK") role = EnemyRole::TANK;
                    else if (roleStr == "BOSS") role = EnemyRole::BOSS;
                    else if (roleStr == "ASTEROID") role = EnemyRole::ASTEROID;

                    EnemyStats stats;
                    auto s = b["stats"];
                    stats.hp = s["hp"];
                    stats.damage = s["damage"];
                    stats.armor = s["armor"];
                    stats.speed = s["speed"];
                    stats.eggRate = s["egg_rate"];
                    stats.score = s["score"];

                    auto layout = b["layout"];
                    std::string type = layout["type"];
                    
                    // Handle delayed spawning
                    if (layout.contains("spawn_delay") && layout["spawn_delay"] > 0.0f) {
                        float delay = layout["spawn_delay"];
                        delayedSpawns.push_back({delay, b, waveId});
                        found = true;
                        continue; // Do not spawn now
                    }

                    std::vector<SpawnData> spawnPoints;
                    
                    if (type == "V_SHAPE") {
                        spawnPoints = FormationBuilder::BuildVShape(b["count"], layout["spacing_x"], layout["spacing_y"], layout["start_y"], layout["target_base_y"], layout["layers"], layout["layer_spacing"], sw);
                    } else if (type == "GRID") {
                        spawnPoints = FormationBuilder::BuildGrid(layout["rows"], layout["cols"], layout["spacing_x"], layout["spacing_y"], layout["start_y"], layout["target_base_y"], sw);
                    } else if (type == "SWEEP_TO_GRID") {
                        spawnPoints = FormationBuilder::BuildSweepToGrid(layout["rows"], layout["cols_per_side"], layout["spacing_x"], layout["spacing_y"], layout["target_base_y"], sw);
                    } else if (type == "INTERSECTING_V") {
                        spawnPoints = FormationBuilder::BuildIntersectingV(b["count"], layout["spacing_x"], layout["spacing_y"], layout["target_base_y"], sw, sh);
                    } else if (type == "RANDOM_RAIN") {
                        spawnPoints = FormationBuilder::BuildRandomRain(b["count"], layout["duration"], layout["start_y"], stats.speed, sw);
                    } else if (type == "TARGETED_PLAYER") {
                        float px = sw / 2.0f;
                        if (gm->GetPlayer()) px = gm->GetPlayer()->GetPosition().x;
                        spawnPoints = FormationBuilder::BuildTargetedPlayer(layout["start_y"], px);
                    }
                    
                    auto mov = b["movement"];
                    std::string movType = mov["type"];

                    for (const auto& pt : spawnPoints) {
                        auto enemy = EnemyFactory::CreateEnemy(visualId, role, stats, pt.startPos);
                        
                        if (movType == "HORIZONTAL_BOUNCE") {
                            float drift = mov["drift"];
                            enemy->SetMovementBehavior(std::make_unique<HorizontalBounceMovement>(pt.targetPos.y, drift, 1.0f));
                        } else if (movType == "WAYPOINT") {
                            enemy->SetMovementBehavior(std::make_unique<WaypointMovement>(pt.waypoints));
                        } else if (movType == "METEOR_DIVE") {
                            enemy->SetMovementBehavior(std::make_unique<MeteorDiveMovement>());
                        } else if (movType == "STRAIGHT") {
                            enemy->SetMovementBehavior(std::make_unique<StraightMovement>());
                        } else if (movType == "HORIZONTAL_SWEEP") {
                            enemy->SetMovementBehavior(std::make_unique<HorizontalSweepMovement>(1.0f));
                        }

                        if (role == EnemyRole::ASTEROID) {
                            if (layout.contains("asteroid_variant")) {
                                enemy->asteroidVariant = layout["asteroid_variant"];
                            } else {
                                enemy->asteroidVariant = GetRandomValue(1, 2);
                            }
                            enemy->canShoot = false;
                        }
                        
                        enemy->ResetEggTimer();
                        gm->AddEnemy(std::move(enemy));
                    }
                    found = true;
                }
            }
            if (found) return true;
        }
    }
    return false;
}

int WaveManager::GetTotalWaves() const {
    if (!stageData.contains("waves")) return 0;
    return stageData["waves"].size();
}

int WaveManager::GetMaxBatchForWave(int waveId) const {
    if (!stageData.contains("waves")) return 0;
    for (const auto& w : stageData["waves"]) {
        if (w["wave_id"] == waveId) {
            if (!w.contains("batches")) return 0;
            int maxBatch = 0;
            for (const auto& b : w["batches"]) {
                int bId = b["batch_id"];
                if (bId > maxBatch) maxBatch = bId;
            }
            return maxBatch;
        }
    }
    return 0;
}
