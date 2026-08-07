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

class WaveManager {
private:
    static WaveManager* instance;
    json stageData;
    std::vector<DelayedSpawn> delayedSpawns;

    WaveManager();

public:
    static WaveManager* GetInstance();
    
    bool LoadStage(const std::string& filepath);
    bool SpawnBatch(int waveId, int batchId);
    
    int GetTotalWaves() const;
    int GetMaxBatchForWave(int waveId) const;
    
    void Update(float deltaTime);
};

#endif
