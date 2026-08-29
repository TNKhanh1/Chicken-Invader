#include "RuneOfAres.h"

constexpr float RuneOfAres::DMG_BONUS[4];
constexpr int RuneOfAres::COSTS[3];

std::string RuneOfAres::GetName() const {
    return "RUNE OF ARES";
}

std::string RuneOfAres::GetDescription() const {
    int displayLevel = level < 3 ? level + 1 : level;
    int percent = (int)(DMG_BONUS[displayLevel] * 100);
    return "Increase base damage.\nEffect: Dmg +" + std::to_string(percent) + "%";
}

void RuneOfAres::ApplyToShip(Spaceship* ship) {
    if (!ship || level == 0) return;
    float bonus = ship->GetDamage() * DMG_BONUS[level];
    ship->AddPermanentDamage(bonus);
}
