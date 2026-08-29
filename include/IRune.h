#ifndef IRUNE_H
#define IRUNE_H

#include "raylib.h"
#include <string>
class Spaceship;

class IRune {
public:
    virtual ~IRune() = default;

    virtual std::string GetName() const = 0;
    virtual std::string GetDescription() const = 0; // Mô tả hiệu ứng hiện tại (dựa vào level)
    virtual int  GetLevel() const = 0;
    virtual int  GetMaxLevel() const { return 3; }
    virtual int  GetCostToUpgrade() const = 0;      // Giá lên level tiếp (0 nếu max)
    virtual bool IsMaxLevel() const { return GetLevel() >= GetMaxLevel(); }

    // Apply hiệu ứng 1 lần khi bắt đầu stage (Ares, Helios)
    virtual void ApplyToShip(Spaceship* ship) = 0;

    // Apply hiệu ứng per-frame (chỉ Gaia dùng)
    virtual void UpdateEffect(Spaceship* ship, float deltaTime) {}

    virtual void LevelUp() = 0;

    // Reset level VÀ usage flags về 0 (gọi khi thoát stage bất kỳ lý do gì)
    virtual void Reset() = 0;

    // Màu hình thoi đại diện
    virtual Color GetColor() const = 0;
};

#endif // IRUNE_H
