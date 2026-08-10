#ifndef WAVEMANAGER_H
#define WAVEMANAGER_H

#include "json.hpp"
#include <string>
#include <vector>
#include <memory>
#include "Enemy.h"

using json = nlohmann::json;

struct DelayedSpawn {
    float delayTimer;
    json batchData; // To store what to spawn when timer hits 0
    int wave;
};

struct PowerPhase {
    int spawnCountThreshold;
    float hpMultiplier;
    float speedMultiplier;
};

class WaveManager {
private:
    static WaveManager* instance;
    json stageData;
    std::vector<DelayedSpawn> delayedSpawns;

    // Continuous Stream State
    bool isContinuousStream = false;
    int targetKills = 0;
    int currentKills = 0;
    float spawnInterval = 1.0f;
    float originalSpawnInterval = 1.0f;
    float spawnTimer = 0.0f;
    int spawnedCount = 0;
    int lastSpawnLane = -1;
    int lastAsteroidKillCount = 100;
    json continuousBatchData;
    std::vector<PowerPhase> powerPhases;

    WaveManager();

public:
    static WaveManager* GetInstance();
    
    bool LoadStage(const std::string& filepath);
    bool SpawnBatch(int waveId, int batchId);
    
    int GetTotalWaves() const;
    int GetMaxBatchForWave(int waveId) const;
    
    void Update(float deltaTime);

    void AddKill();
    int GetCurrentKills() const { return currentKills; }
    int GetTargetKills() const { return targetKills; }
    bool IsContinuousStream() const { return isContinuousStream; }
};

#endif
