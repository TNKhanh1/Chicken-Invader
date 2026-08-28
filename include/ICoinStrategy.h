#ifndef ICOINSTRATEGY_H
#define ICOINSTRATEGY_H

#include "Enemy.h"
#include <algorithm>

// Strategy pattern interface cho việc tính số coin nhận được
class ICoinStrategy {
public:
    virtual ~ICoinStrategy() = default;
    
    // Tính số coin dựa trên thông số của Enemy bị tiêu diệt
    virtual int CalculateKillCoin(const Enemy* enemy) const = 0;
};

// Chiến lược mặc định: thưởng coin dựa trên (score / 2), tối thiểu 1 coin
class DefaultCoinStrategy : public ICoinStrategy {
public:
    int CalculateKillCoin(const Enemy* enemy) const override {
        return std::max(1, enemy->stats.score / 2);
    }
};

#endif // ICOINSTRATEGY_H
