#include "../include/ProgressManager.h"
#include <fstream>
#include <iostream>
#include "raylib.h"

ProgressManager* ProgressManager::instance = nullptr;

ProgressManager::ProgressManager() : highestUnlockedStage(1), dataFilePath("data/progress.dat") {
    // Đảm bảo thư mục data tồn tại nếu cần (bỏ qua nếu đã có sẵn trong cấu trúc dự án)
    LoadProgress();
}

ProgressManager* ProgressManager::GetInstance() {
    if (instance == nullptr) {
        instance = new ProgressManager();
    }
    return instance;
}

void ProgressManager::LoadProgress() {
    std::ifstream inFile(dataFilePath);
    if (inFile.is_open()) {
        inFile >> highestUnlockedStage;
        inFile.close();
        if (highestUnlockedStage < 1) highestUnlockedStage = 1;
        if (highestUnlockedStage > 7) highestUnlockedStage = 7;
        std::cout << "[PROGRESS] Loaded highest unlocked stage: " << highestUnlockedStage << std::endl;
    } else {
        std::cout << "[PROGRESS] No progress file found. Starting fresh at Stage 1." << std::endl;
        highestUnlockedStage = 1;
        SaveProgress();
    }
}

void ProgressManager::SaveProgress() {
    std::ofstream outFile(dataFilePath);
    if (outFile.is_open()) {
        outFile << highestUnlockedStage;
        outFile.close();
        std::cout << "[PROGRESS] Saved highest unlocked stage: " << highestUnlockedStage << std::endl;
    } else {
        std::cerr << "[PROGRESS] Failed to save progress file." << std::endl;
    }
}

void ProgressManager::ResetAllProgress() {
    highestUnlockedStage = 1;
    SaveProgress();
}

int ProgressManager::GetHighestUnlockedStage() const {
    return highestUnlockedStage;
}

void ProgressManager::UnlockStage(int stage) {
    if (stage > highestUnlockedStage && stage <= 7) {
        highestUnlockedStage = stage;
        SaveProgress();
        std::cout << "[PROGRESS] Stage " << stage << " unlocked!" << std::endl;
    }
}

bool ProgressManager::IsStageUnlocked(int stage) const {
    return stage <= highestUnlockedStage;
}
