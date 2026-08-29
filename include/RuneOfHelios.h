#ifndef RUNE_OF_HELIOS_H
#define RUNE_OF_HELIOS_H
#include "IRune.h"
#include "Spaceship.h"

class RuneOfHelios : public IRune {
private:
    int level = 0;
    static constexpr float HP_BONUS[4] = { 0.0f, 0.15f, 0.30f, 0.50f };
    static constexpr int   COSTS[3]    = { 800, 1600, 3200 };

public:
    std::string GetName() const override;
    std::string GetDescription() const override; // "Max HP +XX%"
    int  GetLevel() const override               { return level; }
    int  GetCostToUpgrade() const override       { return level < 3 ? COSTS[level] : 0; }
    void ApplyToShip(Spaceship* ship) override;  // Cộng thêm % MaxHP
    void LevelUp() override                      { if (level < 3) ++level; }
    void Reset() override                        { level = 0; }
    Color GetColor() const override              { return SKYBLUE; }
};

#endif // RUNE_OF_HELIOS_H
