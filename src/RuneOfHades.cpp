#include "RuneOfHades.h"

constexpr float RuneOfHades::REVIVE_HP[4];
constexpr int RuneOfHades::COSTS[3];

std::string RuneOfHades::GetName() const {
    return "RUNE OF HADES";
}

std::string RuneOfHades::GetDescription() const {
    int displayLevel = level < 3 ? level + 1 : level;
    int percent = (int)(REVIVE_HP[displayLevel] * 100);
    return "Auto-resurrect once when dead.\nEffect: Revive with " + std::to_string(percent) + "% HP";
}

bool RuneOfHades::CanRevive(bool isPlayer2) const {
    if (level == 0) return false;
    return isPlayer2 ? !p2Used : !p1Used;
}

bool RuneOfHades::TryRevive(Spaceship* ship, bool isPlayer2) {
    if (!ship || level == 0) return false;
    bool& used = isPlayer2 ? p2Used : p1Used;
    if (used) return false;
    used = true;
    ship->SetHp(ship->GetMaxHp() * REVIVE_HP[level]);
    ship->SetActive(true);
    return true; // RuneManager sẽ bật blink effect sau lệnh này
}
