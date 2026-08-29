# Implementation Plan: 5 Game Features

## Background

Kế hoạch này bao gồm 5 tính năng riêng biệt, mỗi tính năng được đóng gói trong module của mình, **không ảnh hưởng chéo** sang nhau.

---

## Open Questions

> [!IMPORTANT]
> **EXP khi kill gà:** Mỗi loại gà khác nhau thì EXP nhận được có khác nhau không? Hay mặc định là 1 con = X EXP cố định?  
> → Gợi ý: gắn vào `getPointValue()` (BOSS = nhiều EXP hơn gà thường).

> [!IMPORTANT]
> **Bảng tổng kết:** Khi nhấn **BACK** giữa trận, có cần hiện bảng tổng kết không, hay chỉ hiện khi Win/Lose?
> → Plan hiện tại: **Win/Lose/Back đều hiện bảng tổng kết** trước khi về MAIN_MENU.

---

## Proposed Changes

---

### Feature 1 — EXP từ việc kill gà

**Vấn đề:** Hiện tại EXP chỉ nhận khi nhặt đùi gà (`DRUMSTICK`).  
**Giải pháp:** Thêm `GainExp()` vào đúng 2 điểm kill enemy trong game loop.

**OOP/Pattern:** Không cần class mới. Tuân thủ nguyên tắc Open/Closed — chỉ thêm, không sửa.

#### [MODIFY] `src/GameManager.cpp`

- Tại dòng ~1023 (sau `WaveManager::AddKill()`): thêm EXP cho `shooter` bằng `shooter->GainExp(expPerKill)` với `expPerKill` tính dựa theo `enemy->GetPointValue()`.
- Tại dòng ~810 (beam weapon kill path): tương tự, thêm EXP cho `player`.
- **Công thức EXP:** `float exp = enemy->GetPointValue() * 0.5f` (ví dụ: gà 100 điểm = 50 EXP, boss 1000 điểm = 500 EXP).

---

### Feature 2 — Background động ở MAIN_MENU và PLAYER_SELECT / RUNE_SELECTION

**Vấn đề:** Hiện tại background chỉ scroll khi đang trong trận (`WAVE_INTRO`, `TEST_GAMEPLAY`). Ngoài menu thì background tĩnh.  
**Giải pháp:** Cuộn `bgY` liên tục với tốc độ thấp (~30px/s) ngay cả khi ở `MAIN_MENU`, `PLAYER_SELECT`, `RUNE_SELECTION`, `GAME_OVER`, `SETTINGS`.

**OOP/Pattern:** Không cần class mới. Tách riêng logic cuộn background vào hàm `UpdateBackgroundScroll(deltaTime)` nội bộ.

#### [MODIFY] `src/GameManager.cpp`

- Trong `Update()`, thêm khối xử lý ở đầu: nếu state thuộc nhóm non-combat (`MAIN_MENU`, `PLAYER_SELECT`, `RUNE_SELECTION`, `GAME_OVER`, `SETTINGS`, `COMING_SOON`), cuộn `bgY += 30.0f * deltaTime`.
- Không thay đổi logic cuộn của `WAVE_INTRO` / `TEST_GAMEPLAY`.
- Kết quả: background trôi nhẹ ở mọi màn hình, tạo cảm giác sống động.

---

### Feature 3 — Fix BACK button trong trận → không về TEST_MENU

**Vấn đề:** Khi đang chơi thực (`TEST_GAMEPLAY` state được dùng làm gameplay thực), nhấn BACK lại về `TEST_MENU` thay vì GAME_OVER hoặc MAIN_MENU.  
**Giải pháp:** Phân biệt hai trường hợp:
- Nếu đang chơi qua `PLAYER_SELECT → RUNE_SELECTION → StartStage`: BACK → hiện bảng tổng kết (GAME_OVER).
- Nếu đang trong `TEST_GAMEPLAY` từ Test Mode: BACK → `TEST_MENU` (giữ nguyên hành vi hiện tại).

**OOP/Pattern:** Thêm 1 biến flag `bool isTestMode` trong `GameManager`. Khi `StartStage()` được gọi từ menu bình thường, `isTestMode = false`. Khi `ChangeState(GameState::TEST_GAMEPLAY)` được gọi từ WAVE_SELECTION, `isTestMode = true`.

#### [MODIFY] `include/GameManager.h`

- Thêm `bool isTestMode = false;`

#### [MODIFY] `src/GameManager.cpp`

- Trong `StartStage()`: đặt `isTestMode = false`.
- Tại `ChangeState(GameState::TEST_GAMEPLAY)` trong `WAVE_SELECTION`: đặt `isTestMode = true`.
- Nút BACK trong `TEST_GAMEPLAY` Draw:
  ```
  if (isTestMode) → currentState = TEST_MENU (hành vi cũ)
  else → tính thưởng coin → EnterSummary() (bảng tổng kết)
  ```

---

### Feature 4 — Màn hình tổng kết (Summary Screen) thay thế GAME_OVER đơn giản

**Vấn đề:**  
- Bảng tổng kết hiện tại bị lỗi layout: chữ và icon coin bị lệch do dùng hardcode `screenWidth/2 - 270` kết hợp DrawGameTextCentered sai tâm.
- Thiếu thông tin: không có điểm số, không có phân biệt thắng/thua/quit.
- Bảng chỉ hiện khi GAME_OVER, không hiện khi thắng hoặc BACK.

**Giải pháp:** Tạo class `SummaryScreen` riêng (OOP, tách UI khỏi GameManager).

#### [NEW] `include/SummaryScreen.h`

```cpp
#ifndef SUMMARYSCREEN_H
#define SUMMARYSCREEN_H

#include "raylib.h"

enum class SummaryResult { WIN, LOSE, QUIT };

class SummaryScreen {
public:
    SummaryScreen(int sw, int sh);
    void Show(SummaryResult result, int score, int sessionCoins, int stageBonus, int totalCoins);
    void Draw();
    void Update();
    bool IsBackRequested() const;
    void Reset();
private:
    int sw, sh;
    SummaryResult result;
    int score, sessionCoins, stageBonus, totalCoins;
    bool backRequested = false;
    void DrawInfoRow(float x, float y, const char* label, const char* value, Color color);
};

#endif // SUMMARYSCREEN_H
```

#### [NEW] `src/SummaryScreen.cpp`

- Vẽ background overlay mờ.
- Tiêu đề: `"STAGE COMPLETE!"` (xanh lá, WIN) / `"GAME OVER!"` (đỏ, LOSE) / `"RUN ENDED"` (vàng, QUIT).
- Các dòng thông tin căn giữa màn hình (dùng `FontManager::DrawGameTextCentered`):
  - `SCORE: xxxx`
  - Hàng coin icon + `Coins earned: xx + xx (stage bonus) = xx`
  - `Total Coins: xxxx`
- Nút `BACK TO MENU` ở giữa dưới.
- Layout hoàn toàn tính toán dựa trên `sw`/`sh`, không dùng hardcode.

#### [MODIFY] `include/GameState.h`

- Thêm `SUMMARY` vào enum (thay thế vai trò của `GAME_OVER` trong đường đi thực).

#### [MODIFY] `include/GameManager.h`

- Thêm `#include "SummaryScreen.h"`
- Thêm `SummaryScreen* summaryScreen = nullptr;`
- Thêm hàm `void EnterSummary(SummaryResult result);`

#### [MODIFY] `src/GameManager.cpp`

- `Init()`: khởi tạo `summaryScreen = new SummaryScreen(screenWidth, screenHeight)`.
- `CleanUp()`: `delete summaryScreen`.
- Hàm nội bộ `EnterSummary(SummaryResult result)`:
  ```cpp
  void GameManager::EnterSummary(SummaryResult result) {
      int totalW = WaveManager::GetInstance()->GetTotalWaves();
      bool isWin = (result == SummaryResult::WIN);
      CoinManager::GetInstance()->CalculateStageBonus(currentWave, totalW, isWin);
      CoinManager::GetInstance()->CommitSessionCoins();
      summaryScreen->Show(result, score,
          CoinManager::GetInstance()->GetSessionCoins(),
          CoinManager::GetInstance()->GetStageBonusCoins(),
          CoinManager::GetInstance()->GetTotalCoins());
      ChangeState(GameState::SUMMARY);
  }
  ```
- Trong `Update()` case `SUMMARY`: nếu `summaryScreen->IsBackRequested()` → reset game → `MAIN_MENU`.
- Trong `Draw()` case `SUMMARY`: `summaryScreen->Draw()`.

---

### Feature 5 — Hiện bảng tổng kết khi Win / Lose / Back

**Vấn đề:** Hiện tại Win đi thẳng về `MAIN_MENU`, Lose vào `GAME_OVER` (layout lỗi), Back về `TEST_MENU`.  
**Giải pháp:** Thay tất cả các điểm chuyển state thành gọi `EnterSummary()`.

#### [MODIFY] `src/GameManager.cpp`

| Điểm hiện tại | Hành vi hiện tại | Hành vi mới |
|---|---|---|
| Stage thắng (line ~936) | `ChangeState(MAIN_MENU)` | `EnterSummary(SummaryResult::WIN)` |
| Cả 2 player chết (line ~1120) | `currentState = GAME_OVER` | `EnterSummary(SummaryResult::LOSE)` |
| BACK trong gameplay không-test (line ~1771) | `currentState = TEST_MENU` | `EnterSummary(SummaryResult::QUIT)` |

- `GAME_OVER` state **giữ nguyên** (vẫn dùng cho Test Mode khi `isTestMode = true`).
- `ResetForNewStage()` và `ResetToBaseStats()` được gọi trong phần xử lý `summaryScreen->IsBackRequested()`.

---

## Verification Plan

### Automated Tests
- `mingw32-make game` sau mỗi feature, không có lỗi linker hoặc compile error.

### Manual Verification

| Feature | Cách test |
|---|---|
| EXP từ kill | Vào game, bắn gà → xem thanh EXP trên UI tăng không cần nhặt đùi |
| Background động | Mở main menu → quan sát background trôi nhẹ |
| Fix BACK | Chơi thật (từ menu) → nhấn BACK → hiện bảng tổng kết |
| Layout tổng kết | Thắng 1 stage → bảng hiện đúng, icon coin và chữ không lệch |
| Win/Lose đều hiện | Thua stage → bảng GAME OVER; Thắng → bảng STAGE COMPLETE |
