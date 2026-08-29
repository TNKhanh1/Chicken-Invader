#ifndef RUNEMANAGER_H
#define RUNEMANAGER_H

#include "IRune.h"
#include <vector>
#include <memory>
#include <cmath>
#ifndef PI
#define PI 3.14159265358979323846f
#endif

class Spaceship;

class RuneManager {
private:
    static RuneManager* instance;
    RuneManager(); // Khởi tạo 4 rune vào vector

    std::vector<std::unique_ptr<IRune>> runes; // Index: 0=Ares,1=Helios,2=Gaia,3=Hades
    int totalSpent = 0; // Tổng coin đã chi trong phiên rune hiện tại

    // Hiệu ứng nhấp nháy sau hồi sinh
    struct ReviveEffect {
        bool  active   = false;
        float timer    = 0.0f;
        float duration = 2.0f; // 2 giây nhấp nháy
        bool  forP2    = false;
    };
    ReviveEffect p1ReviveEffect;
    ReviveEffect p2ReviveEffect;

public:
    static RuneManager* GetInstance();
    static void DestroyInstance();

    IRune* GetRune(int idx);
    int GetRuneCount() const { return (int)runes.size(); }

    // Mua rune: trừ coin, tăng level. Trả false nếu không đủ tiền hoặc max level.
    bool UpgradeRune(int idx);

    // Nút RESET trên UI: hoàn lại toàn bộ tiền đã chi, reset level về 0
    void ResetAllAndRefund();

    int GetTotalSpent() const { return totalSpent; }

    // Gọi trong StartStage() – Apply Ares+Helios lên ship, Gaia+Hades passive
    void ApplyAll(Spaceship* ship1, Spaceship* ship2);

    // Gọi mỗi frame trong PLAYING – Gaia regen + update blink timer
    void UpdateAll(Spaceship* ship1, Spaceship* ship2, float deltaTime);

    // Gọi khi player chết – thử trigger Hades revive
    // Trả true nếu hồi sinh thành công (GameManager sẽ không đánh dấu player chết)
    bool TryRevive(Spaceship* ship, bool isPlayer2);

    // Kiểm tra ship có đang trong thời gian nhấp nháy hồi sinh không
    bool IsReviving(bool isPlayer2) const;
    float GetReviveBlinkAlpha(bool isPlayer2) const; // 0.0 ~ 1.0 theo sin

    // Gọi khi thoát stage bất kỳ lý do (win/lose/pause quit)
    // Chỉ reset runtime flags (Hades used), KHÔNG reset level (level giữ nguyên cho lần chơi)
    // NHƯNG theo yêu cầu: reset TOÀN BỘ kể cả level
    void ResetForNewStage();
};

#endif // RUNEMANAGER_H
