#ifndef RUNE_OF_ARES_H
#define RUNE_OF_ARES_H
#include "IRune.h"
#include "Spaceship.h"

class RuneOfAres : public IRune {
private:
    int level = 0;
    // Tổng % damage bonus tích lũy theo level
    static constexpr float DMG_BONUS[4] = { 0.0f, 0.10f, 0.25f, 0.45f };
    static constexpr int   COSTS[3]     = { 1000, 2000, 4000 };

public:
    std::string GetName() const override;
    std::string GetDescription() const override; // "Dmg +XX%"
    int  GetLevel() const override               { return level; }
    int  GetCostToUpgrade() const override       { return level < 3 ? COSTS[level] : 0; }
    void ApplyToShip(Spaceship* ship) override;  // Cộng flat damage bonus vào ship
    void LevelUp() override                      { if (level < 3) ++level; }
    void Reset() override                        { level = 0; }
    Color GetColor() const override              { return RED; }
};

#endif // RUNE_OF_ARES_H
