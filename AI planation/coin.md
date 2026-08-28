# Coin System – Implementation Plan

## Tổng quan

Thêm cơ chế **Coin (Vàng)** vào game:
- Coin kiếm được khi **giết quái**
- Tổng kết Coin sau khi **kết thúc stage** (thắng hoặc thua) theo **tiến độ wave**
- Coin **không hiển thị trong màn chơi**, chỉ dùng ở Menu (Runes / Shop, sẽ phát triển sau)
- Toàn bộ dữ liệu Coin được **lưu persistent** qua các lần chơi

---

## Phân tích Codebase Hiện Tại

### Các điểm gắn vào logic kill quái
| File | Dòng | Mô tả |
|---|---|---|
| `GameManager.cpp:617` | Beam weapon kill | `AddScore(enemy->GetPointValue())` |
| `GameManager.cpp:768` | Bullet kill | `AddScore(enemy->GetPointValue())` |
| `GameManager.cpp:809` | Item pickup (meat) | `AddScore(50)` |

### Các điểm gắn vào kết thúc stage/wave
| File | Dòng | Mô tả |
|---|---|---|
| `GameManager.cpp:667` | Stage 7 clear | `GAME_OVER` (thắng) |
| `GameManager.cpp:681` | Stage thường clear | `ChangeState(WAVE_SELECTION)` |
| `GameManager.cpp:819` | Player chết | `GAME_OVER` |

### Pattern hiện có
- **Observer Pattern**: đã có `IObserver` / `ISubject` và `EventType::ENEMY_DIED` trong `Observer.h` — **chưa được dùng**. Ta sẽ tận dụng luôn.
- **Singleton**: `GameManager` đã là Singleton.
- **Score**: đã có `int score` và `AddScore()` trong `GameManager`.

---

## Design Patterns Áp Dụng

### 1. Singleton — `CoinManager`
- Quản lý toàn bộ logic coin: tích lũy trong session, lưu persistent (file).
- Dễ access từ mọi nơi: `CoinManager::GetInstance()`.

### 2. Observer — Notify khi quái chết
- `GameManager` đã là `ISubject` (chỉ cần implement thêm).
- `CoinManager` implement `IObserver`, lắng nghe `EventType::ENEMY_DIED`.
- Khi quái chết → `GameManager::Notify(ENEMY_DIED, coinData)` → `CoinManager::OnNotify()`.
- **Lợi ích:** Tách biệt logic coin khỏi GameManager, dễ mở rộng cho Runes/Shop sau.

### 3. Strategy — `ICoinRewardStrategy` (mở rộng sau)
- Interface tính thưởng coin theo loại quái / boss / event.
- Hiện tại: `DefaultCoinStrategy` (đơn giản).
- Sau này: `BossStageBonus`, `WaveCompletionBonus`, v.v.

---

## Cấu trúc Files Mới / Sửa

```
include/
├── CoinManager.h       [NEW]  Singleton quản lý coin
├── ICoinStrategy.h     [NEW]  Strategy interface cho phần thưởng
├── Observer.h          [MODIFY] Thêm EventType::COIN_EARNED
├── GameManager.h       [MODIFY] Implement ISubject, thêm Notify, texCoin, GetSessionCoins()

src/
├── CoinManager.cpp     [NEW]
├── GameManager.cpp     [MODIFY] Gắn Notify khi kill quái, gọi CoinManager khi end stage

data/
├── coins.dat           [NEW]  File lưu tổng coin persistent (text/binary đơn giản)
```

---

## Chi Tiết Từng File

---

### [NEW] `include/ICoinStrategy.h`

```cpp
#pragma once

class Enemy;

class ICoinStrategy {
public:
    virtual ~ICoinStrategy() = default;
    // Trả về số coin nhận được khi giết enemy này
    virtual int CalculateKillCoin(const Enemy* enemy) const = 0;
};

// Chiến lược mặc định: coin = score / 2, tối thiểu 1
class DefaultCoinStrategy : public ICoinStrategy {
public:
    int CalculateKillCoin(const Enemy* enemy) const override;
};
```

---

### [NEW] `include/CoinManager.h`

```cpp
#pragma once
#include "Observer.h"
#include "ICoinStrategy.h"
#include <memory>
#include <string>

class CoinManager : public IObserver {
private:
    static CoinManager* instance;
    CoinManager();

    int totalCoins       = 0;   // Tổng coin tích lũy (persistent, save/load)
    int sessionCoins     = 0;   // Coin kiếm được trong session hiện tại (từ kill)
    int stageBonusCoins  = 0;   // Coin thưởng cuối stage (theo tiến độ)

    std::unique_ptr<ICoinStrategy> killStrategy;

    void Save() const;  // Ghi coins.dat
    void Load();        // Đọc coins.dat

public:
    static CoinManager* GetInstance();
    static void DestroyInstance();

    // Observer callback
    void OnNotify(EventType event, const std::string& data) override;

    // Được gọi khi kết thúc stage để tính thưởng tiến độ
    void CalculateStageBonus(int currentWave, int totalWaves, bool isWin);

    // Commit: cộng session + bonus vào total và save
    void CommitSessionCoins();

    // Reset session (khi bắt đầu stage mới)
    void ResetSession();

    // Getters
    int GetTotalCoins()      const { return totalCoins; }
    int GetSessionCoins()    const { return sessionCoins; }
    int GetStageBonusCoins() const { return stageBonusCoins; }

    // Spend coin (dùng cho Shop/Runes sau)
    bool SpendCoins(int amount);
};
```

---

### [NEW] `src/CoinManager.cpp`

**Logic chính:**

```cpp
// OnNotify: chỉ xử lý ENEMY_DIED
void CoinManager::OnNotify(EventType event, const std::string& data) {
    if (event == EventType::ENEMY_DIED) {
        // data = số coin từ enemy (serialize đơn giản: "15")
        int coins = std::stoi(data);
        sessionCoins += coins;
    }
}

// CalculateStageBonus:
// - Win toàn bộ stage:       bonus = totalWaves * 30
// - Đến wave X (thua/thoát): bonus = completedWaves * 15
void CoinManager::CalculateStageBonus(int currentWave, int totalWaves, bool isWin) {
    if (isWin)
        stageBonusCoins = totalWaves * 30;
    else
        stageBonusCoins = (currentWave - 1) * 15;
}

// DefaultCoinStrategy:
// coin = max(1, enemy->stats.score / 2)
int DefaultCoinStrategy::CalculateKillCoin(const Enemy* enemy) const {
    return std::max(1, enemy->stats.score / 2);
}
```

**Save/Load** dùng `std::fstream` với file text đơn giản `data/coins.dat`:
```
totalCoins=1250
```

---

### [MODIFY] `include/Observer.h`

Thêm `ENEMY_DIED` (đã có) + `COIN_EARNED` (optional, dùng sau):

```cpp
enum class EventType {
    PLAYER_TOOK_DAMAGE,
    PLAYER_HEALED,
    PLAYER_MANA_CHANGED,
    PLAYER_EXP_GAINED,
    PLAYER_LEVEL_UP,
    ENEMY_DIED,        // ← đã có, giờ sẽ được dùng thực sự
    BOSS_SPAWNED,
    COIN_EARNED        // [NEW] cho Shop/Runes sau
};
```

---

### [MODIFY] `include/GameManager.h`

```cpp
// Thêm ISubject implementation
class GameManager : public ISubject { ... }

// Thêm thành viên private:
std::vector<IObserver*> observers;
Texture2D texCoin;   // coin.png (load trong Init)

// Thêm public methods:
void AddObserver(IObserver* observer) override;
void RemoveObserver(IObserver* observer) override;
void Notify(EventType event, const std::string& data) override;

// Getter session info cho Summary Screen
int GetCurrentWave() const { return currentWave; }
```

---

### [MODIFY] `src/GameManager.cpp`

#### 1. `Init()` — Load coin texture & đăng ký CoinManager

```cpp
texCoin = LoadTexture("assets/coin.png");
CoinManager::GetInstance()->Load();         // Đọc total coins từ file
AddObserver(CoinManager::GetInstance());    // Đăng ký observer
CoinManager::GetInstance()->ResetSession(); // Reset session coin khi init
```

#### 2. Hai điểm kill quái (dòng 615-624 và 766-778)

Thêm một dòng `Notify` sau mỗi `AddScore`:

```cpp
if (!enemy->IsActive()) {
    WaveManager::GetInstance()->AddKill();
    AddScore(enemy->GetPointValue());
    // [NEW] Tính và notify coin
    int coin = CoinManager::GetInstance()->GetKillCoin(enemy.get());
    Notify(EventType::ENEMY_DIED, std::to_string(coin));
    // ... phần còn lại giữ nguyên
}
```

#### 3. Khi kết thúc stage — gọi `CalculateStageBonus` + `CommitSessionCoins`

**Trường hợp win stage thường** (dòng 681):
```cpp
// Trước ChangeState(WAVE_SELECTION)
int total = WaveManager::GetInstance()->GetTotalWaves();
CoinManager::GetInstance()->CalculateStageBonus(currentWave, total, true);
CoinManager::GetInstance()->CommitSessionCoins();
ChangeState(GameState::STAGE_SUMMARY);  // NEW state, xem bên dưới
```

**Trường hợp GAME_OVER** (dòng 819):
```cpp
int total = WaveManager::GetInstance()->GetTotalWaves();
CoinManager::GetInstance()->CalculateStageBonus(currentWave, total, false);
CoinManager::GetInstance()->CommitSessionCoins();
// GAME_OVER state vẫn dùng như cũ, nhưng Draw() của GAME_OVER sẽ hiện summary
```

#### 4. `Draw()` — GAME_OVER và WIN screen: hiển thị coin summary

Trong `case GameState::GAME_OVER:` (dòng 1029), thêm:

```cpp
// Coin Summary (tạm thời đặt ở đây, sau này chuyển sang MainMenu screen)
DrawText(TextFormat("COINS EARNED THIS RUN: %d + %d (stage bonus) = %d",
    CoinManager::GetInstance()->GetSessionCoins(),
    CoinManager::GetInstance()->GetStageBonusCoins(),
    CoinManager::GetInstance()->GetSessionCoins() + CoinManager::GetInstance()->GetStageBonusCoins()),
    screenWidth/2 - 200, 320, 20, GOLD);
DrawText(TextFormat("TOTAL COINS: %d", CoinManager::GetInstance()->GetTotalCoins()),
    screenWidth/2 - 100, 360, 22, YELLOW);
// texCoin icon nhỏ bên cạnh
DrawTexture(texCoin, screenWidth/2 - 230, 316, WHITE);
```

#### 5. `CleanUp()` — Unload coin texture
```cpp
UnloadTexture(texCoin);
```

---

## Bảng Coin Rewards

### Per-kill coin
| Loại quái | Score (EnemyStats) | Coin nhận (score / 2) |
|---|---|---|
| SWARM | 5–10 | 3–5 |
| NORMAL | 10–20 | 5–10 |
| TANK | 30–50 | 15–25 |
| BOSS | 100–500 | 50–250 |
| ASTEROID | 50 | 25 |

### Stage completion bonus
| Tiến độ | Bonus |
|---|---|
| Win hoàn toàn | `totalWaves × 30` coin |
| Đến wave X (thua) | `(X - 1) × 15` coin |
| Wave 1 (chưa đến wave 2) | 0 bonus |

*Ví dụ: Stage có 5 wave, win → +150 bonus; đến wave 3 rồi thua → +30 bonus.*

---

## Nơi lưu tạm (Vì Menu chưa có)

- Coin **tổng cộng** sẽ được **save vào `data/coins.dat`** ngay khi commit.
- Màn hình `GAME_OVER` (hiện tại đã có) sẽ hiển thị **summary coin của run vừa xong** → đây là nơi **tạm thời** hợp lý nhất.
- Khi làm Menu sau, chỉ cần:
  1. Thêm `DrawText(TextFormat("Coins: %d", CoinManager::GetInstance()->GetTotalCoins()), ...)` + icon `texCoin` vào `case GameState::MAIN_MENU`.
  2. `CoinManager::GetInstance()` đã có sẵn, không cần refactor gì thêm.

---

## Thứ Tự Thực Hiện (Step-by-step)

- `[ ]` **Bước 1** – Thêm ảnh `assets/coin.png`
- `[ ]` **Bước 2** – Tạo `include/ICoinStrategy.h` + `src/ICoinStrategy.cpp` (DefaultCoinStrategy)
- `[ ]` **Bước 3** – Tạo `include/CoinManager.h` + `src/CoinManager.cpp` (Singleton, Observer, Save/Load)
- `[ ]` **Bước 4** – Sửa `include/Observer.h` (thêm `COIN_EARNED`)
- `[ ]` **Bước 5** – Sửa `include/GameManager.h` (implement ISubject, thêm texCoin, thêm GetCurrentWave)
- `[ ]` **Bước 6** – Sửa `src/GameManager.cpp`:
  - `Init()`: load texture, đăng ký observer, reset session
  - 2 điểm kill quái: gọi `Notify(ENEMY_DIED, coin)`
  - End-stage / game-over: `CalculateStageBonus` + `CommitSessionCoins`
  - `Draw()` GAME_OVER: hiện coin summary
  - `CleanUp()`: unload texture
- `[ ]` **Bước 7** – Tạo `data/coins.dat` (rỗng, sẽ được tạo tự động khi chạy)
- `[ ]` **Bước 8** – Cập nhật `Makefile` thêm `CoinManager.o` + `ICoinStrategy.o`
- `[ ]` **Bước 9** – Compile & test

---

## Open Questions

> [!IMPORTANT]
> **Tỉ lệ coin per-kill**: Hiện tại dùng `score / 2`. Bạn muốn điều chỉnh tỉ lệ không?

> [!IMPORTANT]
> **Stage completion bonus**: `totalWaves × 30` và `(wave-1) × 15` — số này có hợp lý chưa?

> [!NOTE]
> **Persistence**: Dùng file text `data/coins.dat` đơn giản. Nếu sau này muốn lưu nhiều thứ hơn (unlock, settings...) thì sẽ migrate sang JSON chung.

> [!NOTE]
> **coin.png**: Bạn cần cung cấp ảnh `coin.png` (hoặc để tôi generate). Kích thước khuyến nghị: 32×32 hoặc 64×64, nền trong suốt.
