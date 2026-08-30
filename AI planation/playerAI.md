# Plan: AI cho Player 2 (Chế độ 1P + AI)

## Mô tả

Xây dựng hệ thống AI đơn giản nhưng ổn định cho `player2` trong chế độ `PLAYER_AND_AI`. AI tự động di chuyển, né đạn/kẻ thù, bắn kẻ thù, kích hoạt mana. Thiết kế hoàn toàn theo **OOP + Strategy + State Pattern**, tách biệt khỏi mọi logic hiện có.

---

## Kiến trúc thiết kế

### Pattern áp dụng

| Pattern | Vai trò |
|---|---|
| **Strategy Pattern** | `ISpaceshipController` định nghĩa interface; `AIController` là concrete strategy chỉ được gọi trong chế độ AI |
| **State Pattern** (nhẹ) | `AIController` có `AIState` nội bộ: `IDLE`, `EVADE`, `ATTACK` |
| **Single Responsibility** | `AIController` chỉ đọc thông tin game (read-only), không ghi vào bất kỳ entity nào khác ngoài `ship->SetPosition()` và `ship->Fire()` |

### Sơ đồ phân lớp

```
ISpaceshipController  (interface thuần, header only)
    └── AIController        (AI mới — chỉ tạo khi PLAYER_AND_AI)
            ├── AIState (enum nội bộ: IDLE, EVADE, ATTACK)
            ├── findBestTarget()
            ├── computeEvadeVector()
            ├── computeRepulsionVector()
            └── shouldActivateMana()

GameManager::player2  (shared_ptr<Spaceship>)
    ├── Chế độ TWO_PLAYERS  → đọc chuột (giữ nguyên)
    └── Chế độ PLAYER_AND_AI → gọi aiController->Update(...)
```

---

## Phân tích tình huống bug và cách phòng tránh

| Tình huống | Nguy cơ | Giải pháp |
|---|---|---|
| AI đứng phía trên gà rồi bắn | Đạn đi lên nhưng không trúng gà bên dưới | AI **chỉ bắn khi có ít nhất 1 enemy ở phía trên AI** (`enemy.y < ai.y - SAFE_OFFSET`). Không có target hợp lệ → không gọi `Fire()` |
| AI đứng trong vùng đạn địch | AI chết nhanh | `computeEvadeVector()` quét `activeBullets` không phải player-bullet, tính weighted-sum vector thoát xa |
| AI va chạm vào thân gà | Nhận sát thương liên tục | `computeRepulsionVector()` đẩy AI ra xa khi enemy quá gần |
| AI đọc chuột trong chế độ AI | Bị ghi đè bởi chuột | `AIController` tự tính `desiredPos` và dùng `SetPosition()`, **không đọc mouse** |
| AI bị stuck ở góc | Không thoát ra | Clamp `desiredPos` vào SAFE_BOUNDS mỗi frame |
| `activeEnemies` rỗng | Null access khi loop | Guard `if (enemies.empty()) return nullptr/zero` đầu mỗi hàm |
| `player2` null hoặc inactive | Crash | `AIController::Update()` check ngay đầu: `if (!ship || !ship->IsActive()) return;` |
| Mana spam mỗi frame | Mana kích hoạt lặp liên tục | Cooldown timer `manaActivationCooldown` riêng; chỉ reset sau khi kích hoạt |
| Boss Cutscene đang chạy | AI bắn trong cutscene | Param `isBossCutscene` → skip toàn bộ logic nếu true |
| AI dùng beam weapon | Phức tạp, dễ bug (cần autolocking, VFX riêng) | AI **không xử lý beam**. `ProcessBeamWeapon` lambda chỉ gọi khi `IsMouseButtonDown` → AI không trigger. AI luôn dùng non-beam weapon |
| Lerp speed quá cao → giật cục | Visual artifact | Dùng `lerp(current, desired, speed * dt)` với `speed = 8.0f` — đủ mượt |

---

## Giới hạn vùng hoạt động của AI (Critical Safety Zone)

AI **chỉ được phép tồn tại trong vùng dưới màn hình**:

```
X_MIN = MARGIN = 60.0f
X_MAX = screenWidth  - MARGIN
Y_MIN = screenHeight * Y_SAFE_RATIO   (mặc định 0.55 → 55% từ trên xuống)
Y_MAX = screenHeight - MARGIN
```

Điều này đảm bảo:
1. AI không bao giờ đứng trên gà (gà chỉ chiếm phần trên ~50%)
2. AI không va vào viền màn hình

---

## Chi tiết thuật toán AI

### 1. Chọn mục tiêu bắn (`findBestTarget`)

```
for each enemy in activeEnemies:
    if !enemy->IsActive() → skip
    if enemy->GetPosition().y >= ai.y - SAFE_OFFSET → skip  // enemy KHÔNG ở phía trên AI
    dist = distance(ai.pos, enemy.pos)
    if dist < bestDist:
        bestDist = dist
        bestTarget = enemy

if bestTarget != nullptr:
    if ship->CanFire(): ship->Fire()
```

> Đạn của `Spaceship::Fire()` luôn bay thẳng lên trên theo weapon strategy hiện có. AI chỉ quyết định `có bắn hay không`.

### 2. Né đạn địch (`computeEvadeVector`)

```
evadeVec = {0, 0}
for each bullet in activeBullets:
    if bullet->IsPlayerBullet() → skip
    dist = distance(ai.pos, bullet->GetCenter())
    if dist < DANGER_RADIUS:
        dir = normalize(ai.pos - bullet->GetCenter())
        weight = (DANGER_RADIUS - dist) / DANGER_RADIUS
        evadeVec += dir * weight
return evadeVec  // chưa normalize — độ lớn thể hiện mức nguy hiểm
```

### 3. Né thân gà gần (`computeRepulsionVector`)

```
repVec = {0, 0}
for each enemy in activeEnemies:
    if !enemy->IsActive() → skip
    dist = distance(ai.pos, enemy.pos)
    if dist < REPULSION_RADIUS:
        dir = normalize(ai.pos - enemy.pos)
        weight = (REPULSION_RADIUS - dist) / REPULSION_RADIUS
        repVec += dir * weight
return repVec
```

### 4. Logic di chuyển tổng hợp (State machine)

```
evadeVec  = computeEvadeVector(bullets, aiPos)
repVec    = computeRepulsionVector(enemies, aiPos)
threat    = length(evadeVec) + length(repVec)

if threat > 0.3f:
    state = EVADE
    desiredPos = aiPos + normalize(evadeVec + repVec) * AI_MOVE_SPEED * dt
else if bestTarget != nullptr:
    state = ATTACK
    desiredPos.x = lerp(aiPos.x, bestTarget->GetPosition().x, 0.3f * dt * 10.0f)
    desiredPos.y = aiPos.y  // giữ nguyên Y trong ATTACK
else:
    state = IDLE
    desiredPos = aiPos  // đứng yên

desiredPos = clamp(desiredPos, SAFE_BOUNDS)
newPos = lerp(aiPos, desiredPos, 8.0f * dt)
ship->SetPosition(clamp(newPos, SAFE_BOUNDS))
```

### 5. Kích hoạt Mana

```
manaActivationCooldown -= dt

if manaActivationCooldown <= 0:
    if ship->GetCurrentMana() >= ship->GetMaxMana() * MANA_THRESHOLD:
        activeEnemyCount = count(activeEnemies where IsActive())
        if activeEnemyCount >= MIN_ENEMIES_FOR_MANA:
            ship->ActivateMana()
            manaActivationCooldown = MANA_COOLDOWN
```

---

## Danh sách file cần tạo / sửa

---

### [NEW] `include/ISpaceshipController.h`

```cpp
#pragma once
#include <vector>
#include <memory>

class Spaceship;
class Bullet;
class Enemy;

class ISpaceshipController {
public:
    virtual ~ISpaceshipController() = default;
    virtual void Update(
        Spaceship* ship,
        float deltaTime,
        const std::vector<std::shared_ptr<Bullet>>& activeBullets,
        const std::vector<std::shared_ptr<Enemy>>& activeEnemies,
        bool isBossCutscene,
        int screenWidth,
        int screenHeight
    ) = 0;
};
```

---

### [NEW] `include/AIController.h`

```cpp
#pragma once
#include "ISpaceshipController.h"
#include "raylib.h"

enum class AIState { IDLE, EVADE, ATTACK };

class AIController : public ISpaceshipController {
private:
    AIState currentState = AIState::IDLE;
    float manaActivationCooldown = 0.0f;

    // Hằng số cấu hình (constexpr — không allocate runtime memory)
    static constexpr float DANGER_RADIUS        = 130.0f;
    static constexpr float REPULSION_RADIUS     = 110.0f;
    static constexpr float SAFE_OFFSET          = 10.0f;
    static constexpr float MARGIN               = 60.0f;
    static constexpr float Y_SAFE_RATIO         = 0.55f;
    static constexpr float MANA_THRESHOLD       = 0.95f;
    static constexpr float MANA_COOLDOWN        = 5.0f;
    static constexpr float AI_LERP_SPEED        = 8.0f;
    static constexpr int   MIN_ENEMIES_FOR_MANA = 3;

    Enemy*  findBestTarget(
        const std::vector<std::shared_ptr<Enemy>>& enemies, Vector2 aiPos);
    Vector2 computeEvadeVector(
        const std::vector<std::shared_ptr<Bullet>>& bullets, Vector2 aiPos);
    Vector2 computeRepulsionVector(
        const std::vector<std::shared_ptr<Enemy>>& enemies, Vector2 aiPos);

public:
    void Update(
        Spaceship* ship,
        float deltaTime,
        const std::vector<std::shared_ptr<Bullet>>& activeBullets,
        const std::vector<std::shared_ptr<Enemy>>& activeEnemies,
        bool isBossCutscene,
        int screenWidth,
        int screenHeight
    ) override;
};
```

---

### [NEW] `src/AIController.cpp`

Cài đặt đầy đủ theo các thuật toán đã mô tả ở trên.

---

### [MODIFY] `include/GameManager.h`

Thêm forward declaration và member:
```cpp
// Forward declare
class AIController;

// Trong private section:
std::unique_ptr<AIController> aiController;
```

---

### [MODIFY] `src/GameManager.cpp`

**Thay đổi DUY NHẤT:** Phần xử lý `player2` trong `Update()`:

```cpp
if (player2 && player2->IsActive()) {
    if (currentGameMode == GameMode::TWO_PLAYERS) {
        // ===== GIỮ NGUYÊN TOÀN BỘ LOGIC CHUỘT HIỆN TẠI =====
        Vector2 mousePos = GetMousePosition();
        player2->SetPosition(mousePos);
        if (!isBossCutscene) {
            ProcessBeamWeapon(player2.get(), IsMouseButtonDown(MOUSE_LEFT_BUTTON),
                              autoLockTargetPos2, beamTextTimer2, isAutoLocked2);
        }
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            player2->ActivateMana();
        }
    } else if (currentGameMode == GameMode::PLAYER_AND_AI && aiController) {
        // ===== AI CONTROLLER MỚI =====
        aiController->Update(player2.get(), deltaTime,
                             activeBullets, activeEnemies,
                             isBossCutscene, screenWidth, screenHeight);
    }
    player2->Update(deltaTime);
}
```

Và trong `StartStage()`, thêm khởi tạo AI:
```cpp
if (mode == GameMode::PLAYER_AND_AI) {
    aiController = std::make_unique<AIController>();
} else {
    aiController.reset();
}
```

---

### [MODIFY] `Makefile`

Thêm `obj/AIController.o` vào danh sách object files.

---

## Verification Plan

### Build
```bash
mingw32-make
```
Không có lỗi biên dịch, chỉ warnings hiện có.

### Test thủ công
1. Chọn `1P + AI` → vào game → quan sát AI không lên quá 55% màn hình
2. Gà bay xuống gần AI → AI phải dịch sang bên tránh
3. Gà bắn đạn thẳng vào AI → AI phải né
4. Để mana AI đầy + có 3+ gà → AI tự kích hoạt mana
5. Chạy chế độ `2P` → chuột vẫn điều khiển player2 bình thường
6. Chạy chế độ `1P` → không có player2, không crash

---

## Open Questions

> [!IMPORTANT]
> **Beam weapon cho AI:** Nếu trong tương lai player2 được trang bị beam weapon (Lightning Fryer, Plasma Rifle, Laser Cannon) thông qua shop/upgrade, AI sẽ **không bắn** vì `ProcessBeamWeapon` lambda chỉ trigger với `IsMouseButtonDown`. Plan hiện tại: AI luôn dùng Hypergun (non-beam). Bạn có muốn AI tự động fallback về Hypergun khi ở chế độ AI không?

> [!NOTE]
> **Tốc độ phản xạ AI:** `DANGER_RADIUS = 130px` và `REPULSION_RADIUS = 110px` là ước lượng ban đầu. Sau khi chạy thử có thể cần tinh chỉnh.
