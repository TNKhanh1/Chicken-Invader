#include "RuneOfGaia.h"
#include <sstream>
#include <iomanip>

constexpr float RuneOfGaia::REGEN_RATE[4];
constexpr int RuneOfGaia::COSTS[3];

std::string RuneOfGaia::GetName() const {
    return "RUNE OF GAIA";
}

std::string RuneOfGaia::GetDescription() const {
    int displayLevel = level < 3 ? level + 1 : level;
    float percent = REGEN_RATE[displayLevel] * 100.0f;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << percent;
    return "Cast health regeneration.\nEffect: Regen " + ss.str() + "% HP / sec";
}

void RuneOfGaia::UpdateEffect(Spaceship* ship, float deltaTime) {
    if (!ship || !ship->IsActive() || level == 0) return;
    if (ship->GetHp() >= ship->GetMaxHp()) return; // Không hồi khi full máu
    float regenAmt = ship->GetMaxHp() * REGEN_RATE[level] * deltaTime;
    ship->Heal(regenAmt);
}
