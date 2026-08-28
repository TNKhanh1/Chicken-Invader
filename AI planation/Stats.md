# Stats Panel – Tab-Hold Overlay

## Mô tả
Khi người chơi **giữ phím Tab** trong lúc chiến đấu, một bảng chỉ số (Stats Panel) bán trong suốt sẽ trượt vào từ cạnh trái màn hình, hiển thị toàn bộ chỉ số cơ bản và các Lõi (Arguments) đang sở hữu. **Thả Tab thì bảng mờ dần và biến mất.** Game **không dừng lại** trong suốt quá trình đó.

---

## ⚠️ Cảnh báo thực thi
> **TUYỆT ĐỐI không đụng đến logic gameplay hiện có.**
> Chỉ được phép: thêm file mới, thêm hàm render vào `GameManager::Draw()`, và thêm kiểm tra phím Tab.
> Không được thay đổi: `Update()`, `WaveManager`, `Enemy`, `Bullet`, `Spaceship stats`, hay bất kỳ logic nào khác.

---

## Thiết kế hiển thị

### Layout tổng quan
```
┌──────────────────────────────┐
│       ⚡ PLAYER STATS        │  ← Header (căn giữa)
├──────────────────────────────┤
│  HP        : 850 / 1000      │
│  Damage    : 45  (+12 bonus) │  ← +X là permanent bonus
│  Armor     : 8               │
│  Fire Rate : 0.35s           │
│  Crit Chance: 30%            │
│  Crit Damage: 150%           │
│  Mana      : 60 / 100        │
│  Level     : 5               │
│  EXP       : 800 / 1200      │
├──────────────────────────────┤
│  📌 ARGUMENTS (2 owned)      │  ← Section header
│  ► Blood Fury                │
│     +2 DMG per kill          │
│  ► Round Recovery            │
│     30% HP at wave start     │
└──────────────────────────────┘
```

### Vị trí & kích thước
- **Vị trí**: Cạnh trái màn hình, căn giữa theo chiều dọc
- **Kích thước**: Rộng 340px, cao tự động (tối đa ~580px)
- **Nền**: Đen bán trong suốt `{0, 0, 0, 210}`
- **Viền**: Đường viền xanh lam 1px `{80, 180, 255, 200}`

### Animation slide
- **Hiện** (Tab đang giữ): Lerp `slideX` từ `-345` → `10` (ease-out nhanh ~0.15s)
- **Ẩn** (Tab thả ra): Lerp `slideX` từ `10` → `-345` (ease-in nhanh ~0.12s)

---

## Design Pattern sử dụng

### 1. Facade Pattern – Class `StatsPanel`
Tạo class **`StatsPanel`** mới, độc lập, đóng gói toàn bộ logic render.  
`GameManager::Draw()` chỉ cần gọi một dòng duy nhất:
```cpp
statsPanel.Draw(player.get(), screenHeight);
```
Không cần biết bên trong vẽ gì, tính toán gì.

### 2. Strategy Pattern (tận dụng sẵn)
`StatsPanel::Draw()` nhận `const Spaceship* player` qua tham số — không lưu trữ con trỏ player bên trong class, tránh coupling và memory ownership issues.

### 3. Single Responsibility Principle (SRP)
`StatsPanel` chỉ chịu trách nhiệm duy nhất: **render bảng thống kê**.  
Animation state (`slideX`) cũng được đóng gói trong nội bộ class này.

---

## Proposed Changes

---

### Component: StatsPanel (Mới)

#### [NEW] `include/StatsPanel.h`
```cpp
#pragma once
#include "raylib.h"

class Spaceship; // forward declaration

class StatsPanel {
private:
    float slideX;
    static constexpr float PANEL_W      = 340.0f;
    static constexpr float PANEL_X_SHOW =  10.0f;
    static constexpr float PANEL_X_HIDE = -PANEL_W - 5.0f;
    static constexpr float LERP_IN_SPD  =  12.0f;
    static constexpr float LERP_OUT_SPD =  14.0f;

    void DrawRow(float x, float& y, const char* label,
                 const char* value, Color valueColor) const;

public:
    StatsPanel();
    void Update(float deltaTime, bool tabHeld);
    void Draw(const Spaceship* player, int screenHeight) const;
};
```

#### [NEW] `src/StatsPanel.cpp`
Implement đầy đủ 3 hàm:

- **`Update(deltaTime, tabHeld)`**:
  - Lerp `slideX` về `PANEL_X_SHOW` nếu `tabHeld == true`
  - Lerp `slideX` về `PANEL_X_HIDE` nếu `tabHeld == false`
  - Dùng công thức lerp: `slideX = slideX + (target - slideX) * speed * deltaTime`

- **`DrawRow()`**: Helper vẽ 1 dòng label – value theo layout 2 cột.

- **`Draw(player, screenHeight)`**:
  - Bỏ qua nếu `slideX <= PANEL_X_HIDE + 2` (hoàn toàn ngoài màn hình)
  - Tính `panelY` để căn giữa panel theo chiều dọc
  - Vẽ nền + viền
  - Vẽ Header "PLAYER STATS"
  - Vẽ 9 chỉ số cơ bản (xem bảng bên dưới)
  - Vẽ đường phân cách
  - Vẽ "ARGUMENTS" header
  - Duyệt `player->activeArguments` → vẽ tên và mô tả ngắn của từng Lõi

  Tên + mô tả ngắn Arguments được khai báo là `static const` trong `StatsPanel.cpp` (Option A – không đụng GameManager.cpp).

---

### Component: GameManager (Chỉnh sửa tối thiểu)

#### [MODIFY] `include/GameManager.h`
Thêm vào `private`:
```cpp
#include "StatsPanel.h"
// ...
StatsPanel statsPanel;
```

#### [MODIFY] `src/GameManager.cpp`
Chỉ thêm **3 dòng duy nhất** vào cuối `case TEST_GAMEPLAY:` trong `Draw()`, trước `break`:
```cpp
// --- Stats Panel (Tab overlay) ---
statsPanel.Update(GetFrameTime(), IsKeyDown(KEY_TAB));
if (player) statsPanel.Draw(player.get(), screenHeight);
```

**Không đụng vào `Update()` hay bất kỳ logic nào khác.**

---

## Bảng chỉ số hiển thị

| Nhãn hiển thị | Getter | Ghi chú |
|---|---|---|
| HP | `GetHp()` / `GetMaxHp()` | Format: `850 / 1000` |
| Damage | `GetDamage()` | Nếu `GetPermanentDamageBonus() > 0` thì thêm `(+X)` |
| Armor | `GetArmor()` | |
| Fire Rate | `GetAttackSpeed()` | Format: `0.35s` |
| Crit Chance | `GetCritChance()` | Format: `30%` |
| Crit Damage | `GetCritDamage()` | Format: `150%` |
| Mana | `GetCurrentMana()` / `GetMaxMana()` | Format: `60 / 100` |
| Level | `GetLevel()` | |
| EXP | `GetCurrentExp()` / `GetMaxExp()` | Format: `800 / 1200` |

---

## Verification Plan

### Automated
```bash
mingw32-make
```
→ Phải compile thành công, 0 lỗi.

### Manual
1. Vào **Test Gameplay** → giữ Tab → Panel trượt vào từ trái ✓
2. Thả Tab → Panel trượt ra khỏi màn hình ✓
3. Đảm bảo **game không pause**: gà vẫn bay, đạn vẫn bắn khi panel hiện ✓
4. Giết gà với Argument **Blood Fury** → kiểm tra số Damage trong panel tăng ✓
5. Panel vừa khít trong màn hình, không tràn ✓
