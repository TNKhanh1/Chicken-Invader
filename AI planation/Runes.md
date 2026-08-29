# Runes.md – Kế hoạch chi tiết hệ thống Ngọc (Runes)

---

## Tổng quan kiến trúc

Hệ thống Runes được thiết kế hoàn toàn độc lập, tuân thủ **OOP + Design Pattern** và **không ảnh hưởng bất kỳ logic hiện có** nào. Nó được nhúng vào flow game tại một điểm duy nhất: sau `PLAYER_SELECT` và trước khi `StartStage()` được gọi.

```
MAIN_MENU → PLAYER_SELECT → [NEW] RUNE_SELECTION → StartStage() → WAVE_INTRO → PLAYING → ...
                                                                                ↓
                                                                    (Win / Lose / Thoát giữa chừng)
                                                                                ↓
                                                                    ResetForNewStage() → MAIN_MENU
```

> **Nguyên tắc quan trọng:** Runes chỉ có hiệu lực **trong 1 stage**. Mỗi khi rời khỏi stage dù bằng cách nào (thắng, thua, thoát giữa chừng từ menu PAUSED), toàn bộ level rune đều bị reset về 0. Tiền đã chi không hoàn lại khi thoát giữa chừng — chỉ hoàn lại khi nhấn nút RESET trên màn hình chọn rune.

---

## 1. Thiết kế chỉ số 4 Runes

### Rune 1: RUNE OF ARES (Ngọc Đỏ 🔴 – Hình thoi đỏ)
> Tăng sát thương cơ bản theo % damage gốc

| Level | Hiệu ứng (cộng dồn) | Giá nâng cấp | Tổng chi phí tích lũy |
|-------|---------------------|-------------|----------------------|
| 0 → 1 | Dmg **+10%** | 1,000 coin | 1,000 |
| 1 → 2 | Dmg **+25%** | 2,000 coin | 3,000 |
| 2 → 3 | Dmg **+45%** | 4,000 coin | 7,000 |

> ℹ️ Các % là **tổng tích lũy** (không phải per-level). Lv3 = tổng Dmg +45% so với gốc.
> **Cách apply:** `ship->AddPermanentDamage(ship->GetDamage() * DMG_BONUS[level])`

---

### Rune 2: RUNE OF HELIOS (Ngọc Xanh Dương 🔵 – Hình thoi xanh)
> Tăng máu tối đa theo % MaxHP gốc

| Level | Hiệu ứng (cộng dồn) | Giá nâng cấp | Tổng chi phí tích lũy |
|-------|---------------------|-------------|----------------------|
| 0 → 1 | Max HP **+15%** | 800 coin | 800 |
| 1 → 2 | Max HP **+30%** | 1,600 coin | 2,400 |
| 2 → 3 | Max HP **+50%** | 3,200 coin | 5,600 |

> ℹ️ Các % là **tổng tích lũy**. Lv3 = MaxHP +50% so với MaxHP gốc.
> **Cách apply:** `ship->AddPermanentMaxHp(baseMaxHp * HP_BONUS[level])`

---

### Rune 3: RUNE OF GAIA (Ngọc Xanh Lá 🟢 – Hình thoi xanh lá)
> Hồi máu passively mỗi giây theo % MaxHP hiện tại

| Level | Hiệu ứng | Giá nâng cấp | Tổng chi phí tích lũy |
|-------|----------|-------------|----------------------|
| 0 → 1 | Regen **0.4%** HP/s | 600 coin | 600 |
| 1 → 2 | Regen **0.9%** HP/s | 1,200 coin | 1,800 |
| 2 → 3 | Regen **1.5%** HP/s | 2,400 coin | 4,200 |

> ℹ️ Regen hoạt động **per-frame** qua `UpdateEffect()`, không apply 1 lần. Tự động dừng khi HP full.
> **Cách update:** `ship->Heal(ship->GetMaxHp() * REGEN_RATE[level] * deltaTime)`

---

### Rune 4: RUNE OF HADES (Ngọc Tím 🟣 – Hình thoi tím)
> Tự động hồi sinh 1 lần khi chết, với hiệu ứng nhấp nháy

| Level | % HP hồi sinh | Giá nâng cấp | Tổng chi phí tích lũy |
|-------|--------------|-------------|----------------------|
| 0 → 1 | **25%** HP | 1,200 coin | 1,200 |
| 1 → 2 | **50%** HP | 2,400 coin | 3,600 |
| 2 → 3 | **75%** HP | 4,800 coin | 8,400 |

> ℹ️ **2 Player:** P1 và P2 mỗi người có **1 lần hồi sinh riêng** và độc lập nhau.
> ℹ️ Sau khi hồi sinh, tàu nhấp nháy **2 giây** (alpha dao động theo sin) để báo hiệu invulnerability ngắn.
> ℹ️ Tiền đã mua rune này **không hoàn lại** khi thoát stage giữa chừng.

---

### Tổng chi phí tối đa (nếu mua hết tất cả)
| Rune | Max cost |
|------|---------|
| Ares | 7,000 |
| Helios | 5,600 |
| Gaia | 4,200 |
| Hades | 8,400 |
| **Tổng** | **25,200 coin** |

---

## 2. Files cần tạo mới (NEW)

### [NEW] `include/IRune.h` – Interface Rune (Strategy Pattern)
```cpp
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
```

---

### [NEW] `include/RuneOfAres.h`
```cpp
#ifndef RUNE_OF_ARES_H
#define RUNE_OF_ARES_H
#include "IRune.h"

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
```

---

### [NEW] `include/RuneOfHelios.h`
```cpp
#ifndef RUNE_OF_HELIOS_H
#define RUNE_OF_HELIOS_H
#include "IRune.h"

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
```

---

### [NEW] `include/RuneOfGaia.h`
```cpp
#ifndef RUNE_OF_GAIA_H
#define RUNE_OF_GAIA_H
#include "IRune.h"

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
```

---

### [NEW] `include/RuneOfHades.h`
```cpp
#ifndef RUNE_OF_HADES_H
#define RUNE_OF_HADES_H
#include "IRune.h"

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
```

---

### [NEW] `include/RuneManager.h` – Singleton + Facade Pattern
```cpp
#ifndef RUNEMANAGER_H
#define RUNEMANAGER_H

#include "IRune.h"
#include <vector>
#include <memory>

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
```

---

### [NEW] `include/RuneSelectionUI.h`
```cpp
#ifndef RUNE_SELECTION_UI_H
#define RUNE_SELECTION_UI_H

#include "raylib.h"

class RuneSelectionUI {
private:
    int   screenW, screenH;
    bool  startRequested = false;
    float animTimer      = 0.0f; // Cho animation hiện ra

    // Helpers vẽ UI
    void DrawPanel(float x, float y, float w, float h, Color bg, Color border);
    void DrawDiamond(Vector2 center, float halfSize, Color fill, Color outline);
    void DrawLevelBadge(int level, bool isMax, float cx, float cy, float size);
    void DrawRuneRow(int idx, float rowY, float rowH, bool isHovered);

public:
    RuneSelectionUI(int w, int h);

    void Update(float deltaTime);
    void Draw();

    bool IsStartRequested() const { return startRequested; }
    void ResetStartFlag() { startRequested = false; }
};

#endif // RUNE_SELECTION_UI_H
```

---

### Source files cần tạo
- `src/RuneOfAres.cpp`
- `src/RuneOfHelios.cpp`
- `src/RuneOfGaia.cpp`
- `src/RuneOfHades.cpp`
- `src/RuneManager.cpp`
- `src/RuneSelectionUI.cpp`

---

## 3. Files cần chỉnh sửa (MODIFY)

### [MODIFY] `include/GameState.h`
Thêm **1 dòng duy nhất**:
```diff
     PLAYER_SELECT         // Màn hình chọn chế độ 1P/2P/AI
+    RUNE_SELECTION,       // Màn hình mua Runes trước mỗi stage
```

---

### [MODIFY] `include/CoinManager.h`
```diff
     bool SpendCoins(int amount);
+    void AddCoins(int amount); // Dùng khi hoàn tiền nút RESET trên rune screen
```

---

### [MODIFY] `src/CoinManager.cpp`
```cpp
void CoinManager::AddCoins(int amount) {
    totalCoins += amount;
    Save();
}
```

---

### [MODIFY] `include/GameManager.h`
```diff
+class RuneSelectionUI; // Forward declare
...
private:
+    RuneSelectionUI* runeUI = nullptr;
```

---

### [MODIFY] `src/GameManager.cpp` — 9 điểm tích hợp

**① Thêm include:**
```diff
+#include "RuneManager.h"
+#include "RuneSelectionUI.h"
```

**② Init() – Khởi tạo UI:**
```diff
+runeUI = new RuneSelectionUI(screenWidth, screenHeight);
// RuneManager::GetInstance() tự khởi tạo lazily
```

**③ CleanUp() – Giải phóng:**
```diff
+delete runeUI; runeUI = nullptr;
+RuneManager::DestroyInstance();
```

**④ PLAYER_SELECT – Chuyển sang RUNE_SELECTION thay vì StartStage trực tiếp:**
```diff
 if (DrawButton(singleBtn, "1 PLAYER")) {
-    StartStage(pendingStageFromMenu, GameMode::SINGLE_PLAYER);
+    currentGameMode = GameMode::SINGLE_PLAYER;
+    RuneManager::GetInstance()->ResetForNewStage(); // Đảm bảo sạch
+    ChangeState(GameState::RUNE_SELECTION);
 }
 // Tương tự cho TWO_PLAYERS và PLAYER_AND_AI
```

**⑤ Update() – Thêm case RUNE_SELECTION:**
```cpp
case GameState::RUNE_SELECTION:
{
    runeUI->Update(deltaTime);
    if (runeUI->IsStartRequested() || IsKeyPressed(KEY_SPACE)) {
        runeUI->ResetStartFlag();
        StartStage(pendingStageFromMenu, currentGameMode);
    }
    break;
}
```

**⑥ Draw() – Thêm case RUNE_SELECTION:**
```cpp
case GameState::RUNE_SELECTION:
{
    runeUI->Draw();
    break;
}
```

**⑦ StartStage() – Apply runes sau khi ship đã được init:**
```diff
 // Sau khi player và player2 đã được khởi tạo/reset đầy đủ...
+RuneManager::GetInstance()->ApplyAll(player.get(),
+                                      player2 ? player2.get() : nullptr);
```

**⑧ Update PLAYING – Regen Gaia + Blink Hades:**
```diff
 // Đầu block update PLAYING/TEST_GAMEPLAY, trước khi update ship
+if (currentState == GameState::TEST_GAMEPLAY || currentState == GameState::PLAYING) {
+    RuneManager::GetInstance()->UpdateAll(player.get(),
+                                          player2 ? player2.get() : nullptr,
+                                          deltaTime);
+}
```

**⑨ Draw PLAYING – Hiệu ứng nhấp nháy Hades:**
```diff
 // Khi vẽ player ship (sau khi vẽ bình thường)
+if (RuneManager::GetInstance()->IsReviving(false) && player && player->IsActive()) {
+    float alpha = RuneManager::GetInstance()->GetReviveBlinkAlpha(false);
+    // Vẽ overlay trắng với alpha lên ship để tạo hiệu ứng blink
+}
+// Tương tự cho player2 với IsReviving(true)
```

**⑩ Khi player chết – TryRevive Hades trước khi mark dead:**
```diff
-bool p1Dead = !player || !player->IsActive();
+bool p1Dead = false;
+if (!player || !player->IsActive()) {
+    if (!RuneManager::GetInstance()->TryRevive(player.get(), false)) {
+        p1Dead = true;
+    }
+    // Nếu TryRevive thành công → ship được hồi sinh, p1Dead vẫn false
+}
// Tương tự cho p2Dead
```

**⑪ Khi kết thúc stage (Win / Game Over / Thoát giữa chừng) – Reset runes:**

Thêm `RuneManager::GetInstance()->ResetForNewStage();` vào **3 nơi**:

```diff
// WIN:
+RuneManager::GetInstance()->ResetForNewStage();
 ChangeState(GameState::MAIN_MENU);

// GAME OVER – "BACK TO MENU" button:
+RuneManager::GetInstance()->ResetForNewStage();
 currentState = GameState::MAIN_MENU;

// PAUSED – "QUIT TO MENU" (nếu có nút thoát trong pause):
+RuneManager::GetInstance()->ResetForNewStage();
 ChangeState(GameState::MAIN_MENU);
```

> ⚠️ Nếu hiện chưa có nút "Quit" trong PAUSED, cần kiểm tra và thêm vào nếu bạn muốn cho phép thoát giữa chừng.

---

## 4. Chi tiết Implementation quan trọng

### `RuneManager::ResetForNewStage()`
```cpp
void RuneManager::ResetForNewStage() {
    // Reset TOÀN BỘ: level về 0 + usage flags
    for (auto& r : runes) {
        r->Reset(); // Mỗi rune tự reset level và flags của nó
    }
    totalSpent = 0;
    // Reset blink effects
    p1ReviveEffect = ReviveEffect{};
    p2ReviveEffect = ReviveEffect{};
}
```

### `RuneManager::ResetAllAndRefund()` (nút RESET trên UI)
```cpp
void RuneManager::ResetAllAndRefund() {
    CoinManager::GetInstance()->AddCoins(totalSpent);
    for (auto& r : runes) r->Reset();
    totalSpent = 0;
}
```

### `RuneManager::UpgradeRune()`
```cpp
bool RuneManager::UpgradeRune(int idx) {
    if (idx < 0 || idx >= (int)runes.size()) return false;
    IRune* r = runes[idx].get();
    if (r->IsMaxLevel()) return false;
    int cost = r->GetCostToUpgrade();
    if (!CoinManager::GetInstance()->SpendCoins(cost)) return false;
    r->LevelUp();
    totalSpent += cost;
    return true;
}
```

### `RuneOfAres::ApplyToShip()` – Flat damage bonus
```cpp
void RuneOfAres::ApplyToShip(Spaceship* ship) {
    if (!ship || level == 0) return;
    // Tính bonus dựa trên damage GỐC (trước khi apply Rune)
    float bonus = ship->GetDamage() * DMG_BONUS[level];
    ship->AddPermanentDamage(bonus);
}
```

### `RuneOfHelios::ApplyToShip()` – MaxHP bonus
```cpp
void RuneOfHelios::ApplyToShip(Spaceship* ship) {
    if (!ship || level == 0) return;
    float baseMaxHp = ship->GetMaxHp(); // MaxHP sau khi đã reset về gốc
    float bonus = baseMaxHp * HP_BONUS[level];
    ship->AddPermanentMaxHp(bonus);
}
```

### `RuneOfGaia::UpdateEffect()` – Per-frame regen
```cpp
void RuneOfGaia::UpdateEffect(Spaceship* ship, float deltaTime) {
    if (!ship || !ship->IsActive() || level == 0) return;
    if (ship->GetHp() >= ship->GetMaxHp()) return; // Không hồi khi full máu
    float regenAmt = ship->GetMaxHp() * REGEN_RATE[level] * deltaTime;
    ship->Heal(regenAmt);
}
```

### `RuneOfHades::TryRevive()`
```cpp
bool RuneOfHades::TryRevive(Spaceship* ship, bool isPlayer2) {
    if (!ship || level == 0) return false;
    bool& used = isPlayer2 ? p2Used : p1Used;
    if (used) return false;
    used = true;
    ship->SetHp(ship->GetMaxHp() * REVIVE_HP[level]);
    ship->SetActive(true);
    return true; // RuneManager sẽ bật blink effect sau lệnh này
}
```

### `RuneManager::GetReviveBlinkAlpha()`
```cpp
float RuneManager::GetReviveBlinkAlpha(bool isPlayer2) const {
    const ReviveEffect& e = isPlayer2 ? p2ReviveEffect : p1ReviveEffect;
    if (!e.active) return 1.0f;
    // Nhấp nháy 5 lần / giây, alpha từ 0.3 đến 1.0
    float blink = sinf(e.timer * 10.0f * PI);
    return 0.3f + 0.7f * (0.5f + 0.5f * blink);
}
```

---

## 5. Thiết kế UI (RuneSelectionUI)

### Layout tổng thể (bám sát ảnh tham khảo)
```
Screen: 1600 x 900
┌──── Panel 900x600 centered ─────────────────────────────────────┐
│  RUNES                                    🪙  [total coins]      │
├─────────────────────────────────────────────────────────────────┤
│ [◆Đỏ]  RUNE OF ARES            [LV badge]  [🪙 cost / MAX]      │  H=115px
│         Increase base damage.                                     │
│         Effect: Dmg +XX%                                         │
├─────────────────────────────────────────────────────────────────┤
│ [◆Xanh] RUNE OF HELIOS          [LV badge]  [🪙 cost / MAX]      │  H=115px
│         Increase base health.                                     │
│         Effect: Max HP +XX%                                      │
├─────────────────────────────────────────────────────────────────┤
│ [◆Lá]   RUNE OF GAIA            [LV badge]  [🪙 cost / MAX]      │  H=115px
│         Cast health regeneration.                                 │
│         Effect: Regen X.X% HP / sec                              │
├─────────────────────────────────────────────────────────────────┤
│ [◆Tím]  RUNE OF HADES           [LV badge]  [🪙 cost / MAX]      │  H=115px
│         Auto-resurrect once when dead.                            │
│         Effect: Revive with XX% HP                               │
├─────────────────────────────────────────────────────────────────┤
│  [🔄 RESET]  (hoàn tiền tất cả)      [▶ START!  Press SPACE]    │
└─────────────────────────────────────────────────────────────────┘
```

### Màu sắc 4 hình thoi ngọc
| Rune | Fill | Outline |
|------|------|---------|
| Ares | `RED` | `MAROON` |
| Helios | `SKYBLUE` | `BLUE` |
| Gaia | `LIME` | `DARKGREEN` |
| Hades | `VIOLET` | `PURPLE` |

### Badge Level
| State | Màu nền | Nội dung |
|-------|---------|---------|
| Level 0 | GRAY | `"0"` |
| Level 1 | GREEN | `"1"` |
| Level 2 | PURPLE | `"2"` |
| Level 3 | GOLD | `"MAX"` |

### Nút mua (cột phải)
- Chưa max: Hiện icon coin + số tiền → click để mua
- Đã max: Hiện `"MAX"` màu GOLD, không click được
- Không đủ tiền: Hiện màu xám mờ, không click được

### Hover Effect
- Row được hover sẽ highlight nền nhẹ

---

## 6. Design Patterns sử dụng

| Pattern | Áp dụng |
|---------|--------|
| **Strategy** | `IRune` – mỗi Rune là 1 chiến lược khác nhau (`ApplyToShip`, `UpdateEffect`) |
| **Singleton** | `RuneManager` – 1 instance duy nhất quản lý toàn bộ |
| **Facade** | `RuneManager` gom toàn bộ `UpgradeRune`, `ApplyAll`, `TryRevive`, `UpdateAll` thành API đơn giản cho GameManager |
| **Template Method** | `IRune` cung cấp `IsMaxLevel()` dùng chung; các subclass chỉ override phần cần |

---

## 7. Đảm bảo cô lập – Không ảnh hưởng logic cũ

| Điểm tích hợp | Cách cô lập |
|--------------|------------|
| `PLAYER_SELECT` → `StartStage` | Chỉ chèn state `RUNE_SELECTION` ở giữa, không xóa gì |
| `StartStage()` | Chỉ thêm `ApplyAll()` sau khi ship init xong |
| Update loop | Chỉ thêm `UpdateAll()` đầu block PLAYING |
| Player chết | Kiểm tra `TryRevive()` trước khi mark dead |
| Win/Lose/Quit | Thêm `ResetForNewStage()` vào 3 điểm exit |
| `Spaceship` / `Character` | **Không thay đổi gì** |
| `Bullet` / `Boss` / `Enemy` / `Wave` | **Không thay đổi gì** |
| Argument/Stat Selection | **Không thay đổi gì** |

---

## 8. Thứ tự thực hiện

| Bước | Task |
|------|------|
| 1 | Thêm `AddCoins()` vào `CoinManager` (.h + .cpp) |
| 2 | Tạo `include/IRune.h` |
| 3 | Tạo 4 Rune header + cpp: Ares, Helios, Gaia, Hades |
| 4 | Tạo `RuneManager.h` + `RuneManager.cpp` |
| 5 | Thêm `RUNE_SELECTION` vào `GameState.h` |
| 6 | Tạo `RuneSelectionUI.h` + `RuneSelectionUI.cpp` |
| 7 | Tích hợp vào `GameManager.h` và `GameManager.cpp` (11 điểm) |
| 8 | Cập nhật `Makefile` thêm các `.o` mới |
| 9 | Build + kiểm tra logic từng rune |

---

## 9. Files thay đổi tổng kết

| File | Loại | Ghi chú |
|------|------|---------|
| `include/IRune.h` | **NEW** | Interface Strategy |
| `include/RuneOfAres.h` | **NEW** | Rune tăng damage |
| `include/RuneOfHelios.h` | **NEW** | Rune tăng MaxHP |
| `include/RuneOfGaia.h` | **NEW** | Rune hồi máu regen |
| `include/RuneOfHades.h` | **NEW** | Rune hồi sinh |
| `src/RuneOfAres.cpp` | **NEW** | Implementation |
| `src/RuneOfHelios.cpp` | **NEW** | Implementation |
| `src/RuneOfGaia.cpp` | **NEW** | Implementation |
| `src/RuneOfHades.cpp` | **NEW** | Implementation |
| `include/RuneManager.h` | **NEW** | Facade + Singleton |
| `src/RuneManager.cpp` | **NEW** | Implementation |
| `include/RuneSelectionUI.h` | **NEW** | UI declaration |
| `src/RuneSelectionUI.cpp` | **NEW** | UI implementation |
| `include/GameState.h` | MODIFY | +`RUNE_SELECTION` |
| `include/GameManager.h` | MODIFY | +`runeUI*` |
| `src/GameManager.cpp` | MODIFY | 11 điểm tích hợp |
| `include/CoinManager.h` | MODIFY | +`AddCoins()` |
| `src/CoinManager.cpp` | MODIFY | Impl `AddCoins()` |
| `Makefile` | MODIFY | Thêm `.o` mới |

---

> **Cam kết chất lượng:** Toàn bộ logic Rune nằm trong module riêng. GameManager chỉ biết 4 hàm: `ApplyAll`, `UpdateAll`, `TryRevive`, `ResetForNewStage`. Nếu xóa hệ thống Rune đi, chỉ cần xóa các dòng include + gọi đó, game vẫn chạy bình thường.
