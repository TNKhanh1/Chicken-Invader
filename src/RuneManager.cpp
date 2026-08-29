#include "RuneManager.h"
#include "RuneOfAres.h"
#include "RuneOfHelios.h"
#include "RuneOfGaia.h"
#include "RuneOfHades.h"
#include "CoinManager.h"
#include "Spaceship.h"

RuneManager* RuneManager::instance = nullptr;

RuneManager::RuneManager() {
    runes.push_back(std::make_unique<RuneOfAres>());
    runes.push_back(std::make_unique<RuneOfHelios>());
    runes.push_back(std::make_unique<RuneOfGaia>());
    runes.push_back(std::make_unique<RuneOfHades>());
}

RuneManager* RuneManager::GetInstance() {
    if (instance == nullptr) {
        instance = new RuneManager();
    }
    return instance;
}

void RuneManager::DestroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

IRune* RuneManager::GetRune(int idx) {
    if (idx >= 0 && idx < (int)runes.size()) {
        return runes[idx].get();
    }
    return nullptr;
}

bool RuneManager::UpgradeRune(int idx) {
    if (idx < 0 || idx >= (int)runes.size()) return false;
    IRune* r = runes[idx].get();
    if (r->IsMaxLevel()) return false;
    int cost = r->GetCostToUpgrade();
    if (!CoinManager::GetInstance()->SpendCoins(cost)) return false;
    r->LevelUp();
    totalSpent += cost;
    return true;
}

void RuneManager::ResetAllAndRefund() {
    CoinManager::GetInstance()->AddCoins(totalSpent);
    for (auto& r : runes) r->Reset();
    totalSpent = 0;
}

void RuneManager::ApplyAll(Spaceship* ship1, Spaceship* ship2) {
    for (auto& r : runes) {
        if (ship1) r->ApplyToShip(ship1);
        if (ship2) r->ApplyToShip(ship2);
    }
}

void RuneManager::UpdateAll(Spaceship* ship1, Spaceship* ship2, float deltaTime) {
    for (auto& r : runes) {
        if (ship1) r->UpdateEffect(ship1, deltaTime);
        if (ship2) r->UpdateEffect(ship2, deltaTime);
    }

    if (p1ReviveEffect.active) {
        p1ReviveEffect.timer += deltaTime;
        if (p1ReviveEffect.timer >= p1ReviveEffect.duration) {
            p1ReviveEffect.active = false;
        }
    }
    if (p2ReviveEffect.active) {
        p2ReviveEffect.timer += deltaTime;
        if (p2ReviveEffect.timer >= p2ReviveEffect.duration) {
            p2ReviveEffect.active = false;
        }
    }
}

bool RuneManager::TryRevive(Spaceship* ship, bool isPlayer2) {
    if (runes.size() > 3) {
        RuneOfHades* hades = dynamic_cast<RuneOfHades*>(runes[3].get());
        if (hades) {
            if (hades->TryRevive(ship, isPlayer2)) {
                if (isPlayer2) {
                    p2ReviveEffect.active = true;
                    p2ReviveEffect.timer = 0.0f;
                    p2ReviveEffect.forP2 = true;
                } else {
                    p1ReviveEffect.active = true;
                    p1ReviveEffect.timer = 0.0f;
                    p1ReviveEffect.forP2 = false;
                }
                return true;
            }
        }
    }
    return false;
}

bool RuneManager::IsReviving(bool isPlayer2) const {
    return isPlayer2 ? p2ReviveEffect.active : p1ReviveEffect.active;
}

float RuneManager::GetReviveBlinkAlpha(bool isPlayer2) const {
    const ReviveEffect& e = isPlayer2 ? p2ReviveEffect : p1ReviveEffect;
    if (!e.active) return 1.0f;
    // Nhấp nháy 5 lần / giây, alpha từ 0.3 đến 1.0
    float blink = sinf(e.timer * 10.0f * PI);
    return 0.3f + 0.7f * (0.5f + 0.5f * blink);
}

void RuneManager::ResetForNewStage() {
    // Reset TOÀN BỘ: level về 0 + usage flags
    for (auto& r : runes) {
        r->Reset(); // Mỗi rune tự reset level và flags của nó
    }
    totalSpent = 0;
    // Reset blink effects
    p1ReviveEffect = ReviveEffect{};
    p2ReviveEffect = ReviveEffect{};
}
