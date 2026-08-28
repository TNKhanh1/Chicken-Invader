#include "CoinManager.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

CoinManager* CoinManager::instance = nullptr;

CoinManager::CoinManager() : totalCoins(0), sessionCoins(0), stageBonusCoins(0) {
    killStrategy = std::make_unique<DefaultCoinStrategy>();
}

CoinManager::~CoinManager() {
    Save();
}

CoinManager* CoinManager::GetInstance() {
    if (!instance) {
        instance = new CoinManager();
    }
    return instance;
}

void CoinManager::DestroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

void CoinManager::Load() {
    std::ifstream file("data/coins.dat");
    if (file.is_open()) {
        std::string line;
        if (std::getline(file, line)) {
            size_t pos = line.find("=");
            if (pos != std::string::npos) {
                try {
                    totalCoins = std::stoi(line.substr(pos + 1));
                } catch (...) {
                    totalCoins = 0;
                }
            }
        }
        file.close();
    } else {
        totalCoins = 0;
    }
}

void CoinManager::Save() const {
    std::ofstream file("data/coins.dat");
    if (file.is_open()) {
        file << "totalCoins=" << totalCoins << "\n";
        file.close();
    }
}

void CoinManager::OnNotify(EventType event, const std::string& data) {
    if (event == EventType::ENEMY_DIED) {
        try {
            int coins = std::stoi(data);
            sessionCoins += coins;
        } catch (...) {
            // Error parsing coin amount, do nothing
        }
    }
}

void CoinManager::CalculateStageBonus(int currentWave, int totalWaves, bool isWin) {
    if (isWin) {
        stageBonusCoins = totalWaves * 30;
    } else {
        if (currentWave <= 1) {
            stageBonusCoins = 0;
        } else {
            stageBonusCoins = (currentWave - 1) * 15;
        }
    }
}

void CoinManager::CommitSessionCoins() {
    totalCoins += sessionCoins + stageBonusCoins;
    Save();
}

void CoinManager::ResetSession() {
    sessionCoins = 0;
    stageBonusCoins = 0;
}

bool CoinManager::SpendCoins(int amount) {
    if (amount < 0 || amount > totalCoins) {
        return false;
    }
    totalCoins -= amount;
    Save();
    return true;
}
