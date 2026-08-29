#ifndef COINMANAGER_H
#define COINMANAGER_H

#include "Observer.h"
#include "ICoinStrategy.h"
#include <memory>
#include <string>

// Singleton quản lý tiền tệ (Coin) của người chơi
class CoinManager : public IObserver {
private:
    static CoinManager* instance;
    CoinManager();

    int totalCoins;       // Tổng coin tích lũy (được lưu persistent)
    int sessionCoins;     // Coin thu thập được trong lần chơi hiện tại
    int stageBonusCoins;  // Coin thưởng thêm dựa trên tiến độ (cuối màn)

    std::unique_ptr<ICoinStrategy> killStrategy;

    void Save() const;
public:
    static CoinManager* GetInstance();
    static void DestroyInstance();

    ~CoinManager();

    void Load();

    // IObserver implementation
    void OnNotify(EventType event, const std::string& data) override;

    // Tính toán thưởng tiến độ khi kết thúc màn chơi
    void CalculateStageBonus(int currentWave, int totalWaves, bool isWin);

    // Xác nhận session và lưu coin vào tổng
    void CommitSessionCoins();

    // Reset lại số coin của session khi bắt đầu ván mới
    void ResetSession();

    // Getters
    int GetTotalCoins() const { return totalCoins; }
    int GetSessionCoins() const { return sessionCoins; }
    int GetStageBonusCoins() const { return stageBonusCoins; }

    // Helpers
    int GetKillCoin(const Enemy* enemy) const {
        if (killStrategy) return killStrategy->CalculateKillCoin(enemy);
        return 0;
    }

    // Tiêu thụ coin (cho cửa hàng/runes sau này)
    bool SpendCoins(int amount);

    // Dùng khi hoàn tiền nút RESET trên rune screen
    void AddCoins(int amount);
};

#endif // COINMANAGER_H
