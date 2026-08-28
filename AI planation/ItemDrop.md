# Item Drop System – Kế Hoạch Thiết Kế Chi Tiết

## Mô tả
Thêm cơ chế rớt item ngẫu nhiên từ quái, với 3 loại item mới có hiệu ứng đặc biệt. Item rơi từ trên cao xuống và người chơi nhặt được. Thiết kế đảm bảo OOP, Design Pattern, và **tuyệt đối không ảnh hưởng đến logic phần khác**.

---

## ⚠️ Cảnh báo thực thi
> **CHỈ** được phép thêm file mới, thêm logic trong phạm vi hệ thống Item.  
> Không được thay đổi logic của WaveManager, Enemy, Bullet, Spaceship stats gốc.  
> Mọi tích hợp vào `GameManager` phải **cực kỳ tối thiểu** và không làm xáo trộn flow hiện có.

---

## Phân tích codebase hiện tại

### Hệ thống Item hiện có
- `Item` là base class (extends `GameObject`), có `ItemType` enum và `Update()`/`Draw()` trống.
- `Meat` là subclass kế thừa `Item`, có `Draw()` và physics riêng. Kích thước render: **40×40px**.
- Item va chạm player được xử lý trong `GameManager::Update()` tại ~line 834-846.
- Item hiện tại khi rơi ra ngoài màn hình sẽ tự `isActive = false`.

### Nơi quái chết (để thêm drop logic)
- **Line 636–647**: Quái chết do Beam weapon → Meat được spawn.
- **Line 796–810**: Quái chết do Bullet → Meat được spawn.
- → Đây là 2 điểm duy nhất cần thêm **item drop check**.

### Texture loading
- Tất cả textures load trong `GameManager::Init()` (~line 317–375).
- Hiện tại có `texMeat`, cần thêm `texSwordItem`, `texShieldItem`, `texHeartItem`.

---

## Thiết kế hệ thống

### 1. Cấu trúc OOP

```
Item (Base – hiện có)
├── Meat (hiện có)
├── HeartItem    [NEW] – hồi 25% HP ngay lập tức
├── SwordItem    [NEW] – +25% DMG trong 15s
└── ShieldItem   [NEW] – giảm 30% DMG nhận vào trong 15s
```

### 2. Design Pattern sử dụng

#### 2a. Template Method Pattern – Lớp `PowerUpItem` (trung gian)
Tất cả item mới kế thừa từ `PowerUpItem`, override `OnPickup(Spaceship*)`:
```
Item (Base)
└── PowerUpItem [NEW – Abstract]
    ├── HeartItem::OnPickup() → player->Heal(...)
    ├── SwordItem::OnPickup() → player->ApplyBuff(SWORD, 15s)
    └── ShieldItem::OnPickup() → player->ApplyBuff(SHIELD, 15s)
```
`PowerUpItem::Update()` xử lý physics rơi chung (rơi thẳng, trôi nhẹ).

#### 2b. State Pattern – Buff trên player (trong `Spaceship`)
Thêm `BuffState` struct vào `Spaceship`, giữ trạng thái buff hiện tại:
```cpp
struct ActiveBuff {
    bool swordActive = false;
    float swordTimer = 0.0f;
    bool shieldActive = false;
    float shieldTimer = 0.0f;
};
```
- Buff tồn tại xuyên wave/batch, chỉ reset khi kết thúc stage (hàm `ResetSession()` của CoinManager → thêm `ClearBuffs()` tương tự vào Spaceship).
- `GetDamage()` đã là `virtual` → override trong `SpaceshipDecorator` hoặc đọc buff trong chính `Spaceship::GetDamage()`.
- `TakeDamage()` đã là `virtual` → thêm logic giảm 30% khi `shieldActive == true`.

#### 2c. Factory Pattern (mở rộng) – `ItemDropManager`
Singleton mới chịu trách nhiệm **quyết định có drop item không và drop loại nào**. Tách hoàn toàn khỏi `GameManager`.

---

## Tính toán xác suất Drop

### Mục tiêu thiết kế
> Mỗi batch nên rơi khoảng 1–2 item (trung bình ~1.3).  
> Một batch trung bình có ~10–15 quái (dựa trên data stage hiện tại).  
> **Ràng buộc**: Lúc đang có hiệu ứng buff (sword/shield) thì không rơi thêm item.

### Công thức xác suất
Mỗi khi quái chết, `ItemDropManager` tính drop theo **Dynamic Drop Rate**:

- Khi đang có **bất kỳ** buff nào active → xác suất drop = 0%.
- Khi **chưa có item nào rơi trong batch hiện tại**:
  - Stage 1: **15%**
  - Stage 2+: **10%**
- Khi **đã có item rơi trong batch hiện tại** (Soft-cap):
  - Stage 1: **3%**
  - Stage 2+: **2%**

**Giải thích tính toán Stage 2+:**
```
Giả sử 1 batch = 20 quái.
P(rơi item đầu tiên) = 10%. Kì vọng quái thứ 10 sẽ rớt item.
Số quái còn lại = 10.
P(rơi thêm item) = 2%. Kì vọng số item rơi thêm = 10 * 0.02 = 0.2 item.
=> Tổng item / batch ≈ 1.2 item. Đảm bảo phân bổ đều và giới hạn số lượng.
```

**Cơ chế Reset:**
Khi `GameManager::SpawnWaveBatch()` được gọi, `ItemDropManager::OnNewBatchStarted()` sẽ reset biến đếm số lượng item rơi về 0, khôi phục lại tỉ lệ rơi cao cho batch tiếp theo.

**Hạn chế kép (ràng buộc chất lượng)**:
- Khi đang có **bất kỳ** buff nào active → xác suất drop = 0%.
- Điều này đảm bảo không bị ngập item khi player đang mạnh.

### Phân phối loại item
Khi drop xảy ra (random uniform 3 loại):
| Item | Tỷ lệ | Lý do |
|---|---|---|
| HeartItem | **40%** | Ngay lập tức hữu ích, không conflict với buff |
| SwordItem | **30%** | Tăng công, hấp dẫn |
| ShieldItem | **30%** | Giảm thiệt hại, an toàn |

---

## Kích thước hiển thị Item

- Đùi gà hiện tại: **40×40px**.
- Item mới: **50×50px** (to hơn 25%).
- Căn chỉnh hitbox va chạm tương ứng: radius ~25px.

---

## Xử lý ảnh (xóa nền)

Ba file cần xử lý: `assets/SwordItem.png`, `assets/ShieldItem.png`, `assets/HeartItem.png`.  
Script Python sẽ xóa nền trắng/đồng nhất và export ra PNG với alpha channel trong suốt.

---

## Proposed Changes

---

### Component 1: Xử lý ảnh

#### [SCRIPT] `process_items.py` (temporary)
Dùng Pillow flood-fill từ 4 góc để xóa nền, save lại cùng path.

---

### Component 2: `PowerUpItem` (lớp trung gian mới)

#### [NEW] `include/PowerUpItem.h`
```cpp
#pragma once
#include "Item.h"

class Spaceship; // forward declaration

// Lớp trung gian trừu tượng (Template Method Pattern)
class PowerUpItem : public Item {
protected:
    float fallSpeed;      // Tốc độ rơi (pixel/s)
    float driftX;         // Trôi ngang nhẹ

public:
    PowerUpItem(Vector2 pos, ItemType type, float drift = 0.0f);

    void Update(float deltaTime) override;
    void Draw() override;

    // Template Method: subclass override để áp dụng hiệu ứng
    virtual void OnPickup(Spaceship* player) = 0;
};
```

#### [NEW] `src/PowerUpItem.cpp`
- `Update()`: Rơi thẳng với `fallSpeed = 120.0f px/s`, trôi nhẹ `driftX` (±30px/s),  tự tắt khi `y > screenHeight + 50`.
- `Draw()`: Trống (override bởi subclass).

---

### Component 3: Ba subclass item

#### [NEW] `include/HeartItem.h` / `src/HeartItem.cpp`
```cpp
class HeartItem : public PowerUpItem {
public:
    HeartItem(Vector2 pos);
    void Draw() override;
    void OnPickup(Spaceship* player) override;
    // OnPickup: player->Heal(player->GetMaxHp() * 0.25f)
};
```

#### [NEW] `include/SwordItem.h` / `src/SwordItem.cpp`
```cpp
class SwordItem : public PowerUpItem {
public:
    SwordItem(Vector2 pos);
    void Draw() override;
    void OnPickup(Spaceship* player) override;
    // OnPickup: player->ApplyBuff(BuffType::SWORD, 15.0f)
};
```

#### [NEW] `include/ShieldItem.h` / `src/ShieldItem.cpp`
```cpp
class ShieldItem : public PowerUpItem {
public:
    ShieldItem(Vector2 pos);
    void Draw() override;
    void OnPickup(Spaceship* player) override;
    // OnPickup: player->ApplyBuff(BuffType::SHIELD, 15.0f)
};
```

---

### Component 4: Buff system trong `Spaceship`

#### [MODIFY] `include/Spaceship.h`
Thêm enum và struct buff:
```cpp
enum class BuffType { SWORD, SHIELD };

struct ActiveBuff {
    bool swordActive  = false;  float swordTimer  = 0.0f;
    bool shieldActive = false;  float shieldTimer = 0.0f;
};
```
Thêm vào class:
```cpp
ActiveBuff activeBuff;
void ApplyBuff(BuffType type, float duration);
void UpdateBuffs(float deltaTime);  // gọi trong Update()
void ClearAllBuffs();               // gọi khi kết thúc stage
bool HasSwordBuff() const  { return activeBuff.swordActive; }
bool HasShieldBuff() const { return activeBuff.shieldActive; }
```
Override getters:
```cpp
virtual float GetDamage() const override;  // nếu swordActive: *1.25f
```

#### [MODIFY] `src/Spaceship.cpp`
- `ApplyBuff()`: Set timer và flag.
- `UpdateBuffs()`: Đếm ngược timer, khi hết thì tắt flag. Gọi trong `Spaceship::Update()`.
- `ClearAllBuffs()`: Reset tất cả flag.
- `GetDamage()`: Nếu `swordActive`, return `damage * 1.25f` (hoặc `+ permBonus`).
- `TakeDamage()`: Nếu `shieldActive`, `incomingDamage *= 0.7f` trước khi tính armor.

---

### Component 5: `ItemDropManager` (Singleton mới)

#### [NEW] `include/ItemDropManager.h`
```cpp
#pragma once
#include <memory>
#include "Item.h"

class Spaceship;
class Enemy;

class ItemDropManager {
private:
    static ItemDropManager* instance;
    ItemDropManager() = default;

public:
    static ItemDropManager* GetInstance();
    static void DestroyInstance();

    // Hàm chính: quyết định có drop không và drop loại nào
    // Trả về shared_ptr<Item> hoặc nullptr nếu không drop
    std::shared_ptr<Item> TryDrop(
        const Enemy* enemy,
        const Spaceship* player,
        int currentStage
    );
};
```

#### [NEW] `src/ItemDropManager.cpp`
Logic:
```
TryDrop():
  1. Nếu player có buff đang active → return nullptr
  2. float dropRate = (currentStage == 1) ? 0.18f : 0.12f
  3. int roll = GetRandomValue(0, 99)
  4. Nếu roll >= dropRate*100 → return nullptr
  5. int typeRoll = GetRandomValue(0, 99)
     if typeRoll < 40  → return HeartItem
     if typeRoll < 70  → return SwordItem
     else              → return ShieldItem
```

---

### Component 6: Textures trong `GameManager`

#### [MODIFY] `include/GameManager.h`
Thêm 3 texture vào `private`:
```cpp
Texture2D texSwordItem;
Texture2D texShieldItem;
Texture2D texHeartItem;
// Getter:
Texture2D GetTexSwordItem() const { return texSwordItem; }
Texture2D GetTexShieldItem() const { return texShieldItem; }
Texture2D GetTexHeartItem() const { return texHeartItem; }
```

#### [MODIFY] `src/GameManager.cpp` – Init
Thêm load textures:
```cpp
texSwordItem  = LoadTexture("assets/SwordItem.png");
texShieldItem = LoadTexture("assets/ShieldItem.png");
texHeartItem  = LoadTexture("assets/HeartItem.png");
```

---

### Component 7: Tích hợp Drop vào GameManager::Update

#### [MODIFY] `src/GameManager.cpp` – 2 điểm quái chết
Tại cả 2 nơi quái chết (line ~641 và ~800), thêm ngay sau `PlayExplosionSound()`:
```cpp
// Item Drop (chỉ cho enemy thường, không drop khi boss)
if (enemy->role != EnemyRole::BOSS) {
    auto drop = ItemDropManager::GetInstance()->TryDrop(
        enemy.get(), player.get(), currentStage);
    if (drop) activeItems.push_back(drop);
}
```

---

### Component 8: Tích hợp Pickup vào GameManager::Update

#### [MODIFY] `src/GameManager.cpp` – Item pickup block (~line 834)
Thêm xử lý 3 item mới bên cạnh `DRUMSTICK`:
```cpp
else if (item->GetType() == ItemType::SWORD   ||
         item->GetType() == ItemType::SHIELD  ||
         item->GetType() == ItemType::HEART_POWERUP) {
    auto* powerUp = dynamic_cast<PowerUpItem*>(item.get());
    if (powerUp) powerUp->OnPickup(player.get());
}
```

---

### Component 9: ClearBuffs khi kết thúc stage

#### [MODIFY] `src/GameManager.cpp` – GAME_OVER handling
Khi kết thúc stage (game over hoặc win):
```cpp
if (player) player->ClearAllBuffs();
```

---

### Component 10: Hiển thị buff đang active (UI nhỏ)

#### [MODIFY] `src/GameManager.cpp` – Draw() case TEST_GAMEPLAY
Thêm hiển thị icon buff bên cạnh thanh HP hiện tại (không tạo file mới):
```cpp
if (player) {
    float iconX = 230.0f;  // Ngay bên phải thanh HP
    if (player->HasSwordBuff()) {
        DrawTexturePro(texSwordItem, ..., {iconX, 75.0f, 30.0f, 30.0f}, ...);
        DrawText(TextFormat("%.0fs", swordTimer), iconX, 108, 12, ORANGE);
        iconX += 40.0f;
    }
    if (player->HasShieldBuff()) {
        DrawTexturePro(texShieldItem, ..., {iconX, 75.0f, 30.0f, 30.0f}, ...);
        DrawText(TextFormat("%.0fs", shieldTimer), iconX, 108, 12, CYAN);
    }
}
```

---

## ItemType Enum Update

#### [MODIFY] `include/Item.h`
Thêm vào enum:
```cpp
enum class ItemType {
    HEART,          // hiện có (chưa dùng)
    DRUMSTICK,      // hiện có – Meat
    BOMB,           // hiện có (chưa dùng)
    LEVEL_UP,       // hiện có (chưa dùng)
    EGG,            // hiện có
    HEART_POWERUP,  // [NEW] – hồi 25% HP
    SWORD,          // [NEW] – +25% DMG 15s
    SHIELD          // [NEW] – -30% DMG nhận 15s
};
```

---

## Verification Plan

### Build
```bash
mingw32-make
```
→ 0 lỗi compile.

### Manual Test
1. Chơi Stage 1 → giết vài quái → xác nhận item rớt ra và rơi xuống ✓
2. Nhặt HeartItem → HP tăng 25% ✓
3. Nhặt SwordItem → icon buff hiện bên cạnh HP bar → tồn tại qua wave tiếp theo ✓
4. Nhặt ShieldItem → bị đánh → sát thương giảm 30% ✓
5. Đang có buff → kiểm tra item không rơi thêm ✓
6. Kết thúc stage → buff biến mất ✓
7. Stage 2+ → xác nhận tỉ lệ giảm so với stage 1 ✓
