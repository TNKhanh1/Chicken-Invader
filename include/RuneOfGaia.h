#ifndef RUNE_OF_GAIA_H
#define RUNE_OF_GAIA_H
#include "IRune.h"
#include "Spaceship.h"

class RuneOfGaia : public IRune {
private:
    int level = 0;
    static constexpr float REGEN_RATE[4] = { 0.0f, 0.004f, 0.009f, 0.015f }; // % MaxHP / giây
    static constexpr int   COSTS[3]      = { 600, 1200, 2400 };

public:
    std::string GetName() const override;
    std::string GetDescription() const override; // "Regen X.X% HP / sec"
    int  GetLevel() const override               { return level; }
    int  GetCostToUpgrade() const override       { return level < 3 ? COSTS[level] : 0; }
    void ApplyToShip(Spaceship* /*ship*/) override {} // Không apply 1 lần – chỉ per-frame
    void UpdateEffect(Spaceship* ship, float deltaTime) override;
    float GetRegenRate() const { return REGEN_RATE[level]; }
    void LevelUp() override                      { if (level < 3) ++level; }
    void Reset() override                        { level = 0; }
    Color GetColor() const override              { return LIME; }
};

#endif // RUNE_OF_GAIA_H
