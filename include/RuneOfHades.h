#ifndef RUNE_OF_HADES_H
#define RUNE_OF_HADES_H
#include "IRune.h"
#include "Spaceship.h"

class RuneOfHades : public IRune {
private:
    int  level  = 0;
    bool p1Used = false; // P1 đã dùng lần hồi sinh chưa
    bool p2Used = false; // P2 đã dùng lần hồi sinh chưa

    static constexpr float REVIVE_HP[4] = { 0.0f, 0.25f, 0.50f, 0.75f };
    static constexpr int   COSTS[3]     = { 1200, 2400, 4800 };

public:
    std::string GetName() const override;
    std::string GetDescription() const override; // "Revive with XX% HP"
    int  GetLevel() const override               { return level; }
    int  GetCostToUpgrade() const override       { return level < 3 ? COSTS[level] : 0; }
    void ApplyToShip(Spaceship* /*ship*/) override {} // Không apply ngay, trigger khi chết

    // Thử hồi sinh – trả về true nếu thành công, false nếu level=0 hoặc đã dùng rồi
    bool TryRevive(Spaceship* ship, bool isPlayer2);
    bool CanRevive(bool isPlayer2) const;
    float GetReviveHpPercent() const { return REVIVE_HP[level]; }

    void LevelUp() override { if (level < 3) ++level; }

    // QUAN TRỌNG: Reset cả p1Used và p2Used cùng với level
    void Reset() override { level = 0; p1Used = false; p2Used = false; }

    Color GetColor() const override { return PURPLE; }
};

#endif // RUNE_OF_HADES_H
