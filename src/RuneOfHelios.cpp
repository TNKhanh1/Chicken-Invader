#include "RuneOfHelios.h"

constexpr float RuneOfHelios::HP_BONUS[4];
constexpr int RuneOfHelios::COSTS[3];

std::string RuneOfHelios::GetName() const {
    return "RUNE OF HELIOS";
}

std::string RuneOfHelios::GetDescription() const {
    int displayLevel = level < 3 ? level + 1 : level;
    int percent = (int)(HP_BONUS[displayLevel] * 100);
    return "Increase base health.\nEffect: Max HP +" + std::to_string(percent) + "%";
}

void RuneOfHelios::ApplyToShip(Spaceship* ship) {
    if (!ship || level == 0) return;
    float baseMaxHp = ship->GetMaxHp();
    float bonus = baseMaxHp * HP_BONUS[level];
    ship->AddPermanentMaxHp(bonus);
}
