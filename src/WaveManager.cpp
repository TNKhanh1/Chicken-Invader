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
#include "SpiralMovement.h"
#include "SineZigzagMovement.h"
#include "VerticalZigzagMovement.h"
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

void WaveManager::AddKill() {
    if (isContinuousStream) {
        currentKills++;
    }
}

void WaveManager::Update(float deltaTime) {
    if (isContinuousStream) {
        if (currentKills >= targetKills) {
            isContinuousStream = false;
        } else {
            if (currentKills >= 100) {
                spawnInterval = originalSpawnInterval * 0.75f; // Tăng tần suất ra quái thêm 33% (giảm delay 25%)
                
                if (currentKills - lastAsteroidKillCount >= 10) {
                    lastAsteroidKillCount += 10;
                    
                    int numAsteroids = GetRandomValue(1, 2);
                    for (int i = 0; i < numAsteroids; i++) {
                        float startX = GameManager::GetInstance()->GetScreenWidth() / 2.0f;
                        if (GameManager::GetInstance()->GetPlayer()) {
                            startX = GameManager::GetInstance()->GetPlayer()->GetPosition().x;
                        }
                        Vector2 startPos = { startX, -100.0f - i * 150.0f };
                        int visualId = GetRandomValue(4, 5); // visualId 4 hoặc 5 là thiên thạch
                        
                        EnemyStats astStats;
                        astStats.hp = 1500.0f;
                        astStats.damage = 30.0f;
                        // Giảm 20-30% tốc độ cho stage 3 (VD: 250 - 350 thay vì 350 - 500)
                        astStats.speed = (float)GetRandomValue(250, 350);
                        astStats.score = 50;

                        auto enemy = EnemyFactory::CreateEnemy(visualId, EnemyRole::ASTEROID, astStats, startPos);
                        enemy->SetMovementBehavior(std::make_unique<MeteorDiveMovement>());
                        enemy->ResetEggTimer();
                        GameManager::GetInstance()->AddEnemy(std::move(enemy));
                    }
                }
            }

            spawnTimer -= deltaTime;
            if (spawnTimer <= 0.0f) {
                spawnTimer = spawnInterval;
                if (GameManager::GetInstance()->GetActiveEnemies().size() < 25) {
                    float hpMult = 1.0f;
                    float speedMult = 1.0f;
                    for (const auto& ph : powerPhases) {
                        if (spawnedCount >= ph.spawnCountThreshold) {
                            hpMult = ph.hpMultiplier;
                            speedMult = ph.speedMultiplier;
                        }
                    }

                    int laneCount = 7;
                    int laneWidth = GameManager::GetInstance()->GetScreenWidth() / laneCount;
                    int lane = GetRandomValue(0, laneCount - 1);
                    if (lane == lastSpawnLane) {
                        lane = (lane + 1) % laneCount;
                    }
                    lastSpawnLane = lane;
                    
                    Vector2 startPos = { (float)(lane * laneWidth + laneWidth / 2), -100.0f };
                    int visualId = GetRandomValue(1, 3);
                    EnemyRole role = EnemyRole::NORMAL;
                    
                    auto b = continuousBatchData;
                    EnemyStats stats;
                    stats.hp = b["stats"]["hp"].get<float>() * hpMult;
                    stats.damage = b["stats"]["damage"].get<float>();
                    stats.speed = b["stats"]["speed"].get<float>() * speedMult;
                    stats.score = b["stats"]["score"];
                    stats.scale = b["stats"].value("scale", 1.0f);

                    auto enemy = EnemyFactory::CreateEnemy(visualId, role, stats, startPos);
                    enemy->SetMovementBehavior(std::make_unique<StraightMovement>(false));
                    enemy->ResetEggTimer();
                    GameManager::GetInstance()->AddEnemy(std::move(enemy));
                    
                    spawnedCount++;
                }
            }
        }
    }

    auto gm = GameManager::GetInstance();
    
    for (auto it = delayedSpawns.begin(); it != delayedSpawns.end();) {
        it->delayTimer -= deltaTime;
        if (it->delayTimer <= 0.0f) {
            auto b = it->batchData;
            auto layout = b["layout"];
            std::string layoutType = layout["type"];
            int visualId = b.value("visual_id", 1);
            
            std::string roleStr = b["role"];
            EnemyRole role = EnemyRole::NORMAL;
            if (roleStr == "SWARM") role = EnemyRole::SWARM;
            else if (roleStr == "TANK") role = EnemyRole::TANK;
            else if (roleStr == "BOSS") role = EnemyRole::BOSS;
            else if (roleStr == "ASTEROID") role = EnemyRole::ASTEROID;

            EnemyStats stats;
            auto s = b["stats"];
            stats.hp = s.value("hp", 100.0f);
            stats.damage = s.value("damage", 20.0f);
            stats.armor = s.value("armor", 0.0f);
            stats.speed = s.value("speed", 100.0f);
            stats.eggRate = s.value("egg_rate", 3.0f);
            stats.score = s.value("score", 10);
            stats.scale = s.value("scale", 1.0f);
            
            std::vector<SpawnData> spawnPoints;
            float sw = gm->GetScreenWidth();
            
            if (layoutType == "TARGETED_PLAYER") {
                float startY = layout["start_y"];
                float px = sw / 2.0f; // fallback
                if (gm->GetPlayer()) {
                    px = gm->GetPlayer()->GetPosition().x;
                }
                spawnPoints = FormationBuilder::BuildTargetedPlayer(startY, px);
            } else if (layoutType == "METEOR_SHOWER") {
                int count = b.value("count", 5);
                for (int i = 0; i < count; i++) {
                    float px = (float)GetRandomValue(50, (int)sw - 50);
                    float py = -100.0f - (float)GetRandomValue(0, 500);
                    spawnPoints.push_back({ {px, py}, 0.0f });
                }
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
                    stats.hp = s.value("hp", 100.0f);
                    stats.damage = s.value("damage", 20.0f);
                    stats.armor = s.value("armor", 0.0f);
                    stats.speed = s.value("speed", 100.0f);
                    stats.eggRate = s.value("egg_rate", 3.0f);
                    stats.score = s.value("score", 10);
                    stats.scale = s.value("scale", 1.0f);
                    auto layout = b["layout"];
                    isContinuousStream = false;
                    if (layout["type"] == "CONTINUOUS_STREAM") {
                        isContinuousStream = true;
                        targetKills = layout.value("target_kills", 200);
                        spawnInterval = layout.value("spawn_interval", 1.0f);
                        originalSpawnInterval = spawnInterval;
                        currentKills = 0;
                        spawnedCount = 0;
                        spawnTimer = 0.0f;
                        lastSpawnLane = -1;
                        lastAsteroidKillCount = 100;
                        continuousBatchData = b;
                        
                        powerPhases.clear();
                        if (layout.contains("phases")) {
                            for (auto& ph : layout["phases"]) {
                                PowerPhase pp;
                                pp.spawnCountThreshold = ph.value("spawn_count_threshold", 0);
                                pp.hpMultiplier = ph.value("hp_multiplier", 1.0f);
                                pp.speedMultiplier = ph.value("speed_multiplier", 1.0f);
                                powerPhases.push_back(pp);
                            }
                        } else {
                            powerPhases.push_back({0, 1.0f, 1.0f});
                        }
                        return true;
                    }

                    std::string layoutType = layout["type"];
                    
                    // Handle delayed spawning
                    if (layout.contains("spawn_delay") && layout["spawn_delay"] > 0.0f) {
                        float delay = layout["spawn_delay"];
                        delayedSpawns.push_back({delay, b, waveId});
                        found = true;
                        continue; // Do not spawn now
                    }

                    std::vector<SpawnData> spawnPoints;
                    
                    if (layoutType == "V_SHAPE") {
                        spawnPoints = FormationBuilder::BuildVShape(b["count"], layout["spacing_x"], layout["spacing_y"], layout["start_y"], layout["target_base_y"], layout["layers"], layout["layer_spacing"], sw);
                    } else if (layoutType == "GRID") {
                        spawnPoints = FormationBuilder::BuildGrid(layout["rows"], layout["cols"], layout["spacing_x"], layout["spacing_y"], layout["start_y"], layout["target_base_y"], sw);
                    } else if (layoutType == "SWEEP_TO_GRID") {
                        spawnPoints = FormationBuilder::BuildSweepToGrid(layout["rows"], layout["cols_per_side"], layout["spacing_x"], layout["spacing_y"], layout["target_base_y"], sw);
                    } else if (layoutType == "INTERSECTING_V") {
                        spawnPoints = FormationBuilder::BuildIntersectingV(b["count"], layout["spacing_x"], layout["spacing_y"], layout["target_base_y"], sw, sh);
                    } else if (layoutType == "RANDOM_RAIN") {
                        spawnPoints = FormationBuilder::BuildRandomRain(b["count"], layout["duration"], layout["start_y"], stats.speed, sw);
                    } else if (layoutType == "TARGETED_PLAYER") {
                        float px = sw / 2.0f;
                        if (gm->GetPlayer()) px = gm->GetPlayer()->GetPosition().x;
                        spawnPoints = FormationBuilder::BuildTargetedPlayer(layout["start_y"], px);
                    } else if (layoutType == "RING") {
                        Vector2 center = {sw / 2.0f, layout["center_y"]};
                        spawnPoints = FormationBuilder::BuildRing(b["count"], layout["radius"], center);
                    }
                    
                    auto mov = b["movement"];
                    std::string movType = mov["type"];

                    bool wrapAround = true;
                    if (mov.contains("wrap_around")) {
                        wrapAround = mov["wrap_around"];
                    }

                    for (const auto& pt : spawnPoints) {
                        auto enemy = EnemyFactory::CreateEnemy(visualId, role, stats, pt.startPos);
                        
                        if (movType == "HORIZONTAL_BOUNCE") {
                            float drift = mov["drift"];
                            enemy->SetMovementBehavior(std::make_unique<HorizontalBounceMovement>(pt.targetPos.y, drift, 1.0f));
                        } else if (movType == "WAYPOINT") {
                            std::vector<Vector2> wp = pt.waypoints;
                            if (mov.contains("waypoints")) {
                                wp.clear();
                                for (auto& w : mov["waypoints"]) {
                                    wp.push_back({w["x"].get<float>(), w["y"].get<float>()});
                                }
                            }
                            enemy->SetMovementBehavior(std::make_unique<WaypointMovement>(wp));
                        } else if (movType == "METEOR_DIVE") {
                            enemy->SetMovementBehavior(std::make_unique<MeteorDiveMovement>());
                        } else if (movType == "STRAIGHT") {
                            bool straightWrap = mov.contains("wrap_around") ? wrapAround : false;
                            enemy->SetMovementBehavior(std::make_unique<StraightMovement>(straightWrap));
                        } else if (movType == "HORIZONTAL_SWEEP") {
                            enemy->SetMovementBehavior(std::make_unique<HorizontalSweepMovement>(1.0f));
                        } else if (movType == "SPIRAL") {
                            enemy->SetMovementBehavior(std::make_unique<SpiralMovement>(wrapAround));
                        } else if (movType == "SINE_ZIGZAG") {
                            enemy->SetMovementBehavior(std::make_unique<SineZigzagMovement>(wrapAround));
                        } else if (movType == "VERTICAL_ZIGZAG") {
                            float dMult = mov.value("downward_speed_mult", 0.3f);
                            float hMult = mov.value("horizontal_speed_mult", 0.5f);
                            enemy->SetMovementBehavior(std::make_unique<VerticalZigzagMovement>(wrapAround, dMult, hMult));
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
