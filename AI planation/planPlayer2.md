# Kế hoạch thêm Người Chơi 2 (Player 2)

## 1. Tổng quan

Thêm hỗ trợ chế độ 2 người chơi vào game, trong đó:
- **Player 1**: Điều khiển bằng bàn phím (WASD / phím mũi tên) như hiện tại.
- **Player 2**: Điều khiển bằng chuột — phi thuyền di chuyển đến đúng vị trí con trỏ chuột, chuột trái để bắn, chuột phải để kích hoạt Mana.
- **AI**: Chưa triển khai (placeholder để trống cho tương lai).

Asset phi thuyền Player 2: `assets/spaceship/spaceship2.png` (**đã có sẵn**).

---

## 2. Màn hình chọn chế độ chơi (Player Select Screen)

### 2.1 Luồng điều hướng mới

```
MainMenu → [Click Stage] → PlayerSelectScreen → [Chọn chế độ] → Game
```

Trước đây khi click một Stage trong MenuManager sẽ trả về ID stage ngay lập tức. Sau khi sửa:
- Click Stage → trả về ID stage → `GameManager` chuyển sang `GameState::PLAYER_SELECT` (state mới).
- Màn hình Player Select hiện 3 lựa chọn rồi mới bắt đầu game.

### 2.2 Giao diện Player Select Screen

```
┌──────────────────────────────────────────────┐
│              STAGE 3 - CHỌN CHẾ ĐỘ           │
│                                              │
│  ┌────────────┐  ┌────────────┐  ┌──────────┐│
│  │ 1 PLAYER   │  │ 2 PLAYERS  │  │ 1P + AI  ││
│  │            │  │            │  │          ││
│  │  [ship1]   │  │ [s1]  [s2] │  │ [s1][AI] ││
│  └────────────┘  └────────────┘  └──────────┘│
│                                              │
│                 [BACK]                       │
└──────────────────────────────────────────────┘
```

---

## 3. Kiến trúc OOP / Design Decisions

### 3.1 Phân tích hệ thống hiện tại

| Thành phần | Mô tả | Ghi chú |
|---|---|---| 
| `Spaceship` | Class phi thuyền, kế thừa `Character`, `ISubject` | Tái sử dụng cho Player 2 |
| `Spaceship::Fire()` | Bắn dựa vào `shootingBehavior` | Giữ nguyên |
| `Spaceship::ActivateMana()` | Kích hoạt Mana | Giữ nguyên |
| `GameManager::player` | `shared_ptr<Spaceship>` — Player 1 | Thêm `player2` song song |
| `GameManager::Update()` | Xử lý input Player 1 | Thêm nhánh input Player 2 |
| `GameManager::Draw()` | Vẽ Player 1 | Thêm vẽ Player 2 |

### 3.2 Enum chế độ chơi (mới)

```cpp
// include/GameMode.h (FILE MỚI)
enum class GameMode {
    SINGLE_PLAYER,   // 1 người
    TWO_PLAYERS,     // 2 người
    PLAYER_AND_AI    // 1 người + 1 AI (chưa triển khai)
};
```

### 3.3 Cập nhật GameState

```cpp
// include/GameState.h — Thêm PLAYER_SELECT
enum class GameState {
    // ... các state cũ ...
    PLAYER_SELECT   // Màn hình chọn 1P / 2P / AI
};
```

### 3.4 Biến mới trong `GameManager`

```cpp
// include/GameManager.h
GameMode currentGameMode = GameMode::SINGLE_PLAYER;
std::shared_ptr<Spaceship> player2;           // nullptr nếu 1P
Texture2D texSpaceship2;                      // spaceship2.png
int pendingStageFromMenu = -1;                // Stage đang chờ xác nhận chế độ
```

---

## 4. Hệ thống Input cho Player 2 (Mouse Control)

### 4.1 Cơ chế điều khiển chuột

| Hành động | Input |
|---|---|
| Di chuyển phi thuyền | Phi thuyền vẽ tại vị trí con trỏ chuột (`GetMousePosition()`) |
| Bắn | `IsMouseButtonDown(MOUSE_LEFT_BUTTON)` → `player2->Fire()` |
| Kích hoạt Mana | `IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)` → `player2->ActivateMana()` |

> **Không có "moveSpeed" hay "lerp"** — phi thuyền 2 được set thẳng vị trí đến vị trí con trỏ mỗi frame để đảm bảo phản hồi tức thì, tránh cảm giác lag.

### 4.2 Code logic Input Player 2 trong `GameManager::Update()`

```cpp
// Trong GameManager::Update() — nhánh case GameState::PLAYING
if (player2 && currentGameMode == GameMode::TWO_PLAYERS) {
    // Di chuyển: set thẳng vị trí tới con trỏ chuột
    Vector2 mousePos = GetMousePosition();
    player2->SetPosition(mousePos);

    // Bắn bằng chuột trái
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        player2->Fire();
    }

    // Kích hoạt Mana bằng chuột phải
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        player2->ActivateMana();
    }

    player2->Update(deltaTime);
}
```

---

## 5. Game Over Logic cho 2 người chơi

- **2 Players**: Game Over khi **cả 2** phi thuyền đều chết (HP = 0).
- Nếu Player 2 chết trước, Player 1 vẫn tiếp tục chơi một mình (và ngược lại).
- Điều kiện kiểm tra:
```cpp
bool p1Dead = !player || player->GetCurrentHp() <= 0;
bool p2Dead = !player2 || player2->GetCurrentHp() <= 0;
if (p1Dead && p2Dead) ChangeState(GameState::GAME_OVER);
else if (p1Dead && !p2Dead)  { /* chỉ P2 còn sống, tiếp tục */ }
else if (!p1Dead && p2Dead)  { /* chỉ P1 còn sống, tiếp tục */ }
```

---

## 6. Vấn đề Bullet Ownership (đạn của ai?)

Hiện tại tất cả đạn của player đều thuộc `Bullet::isPlayerBullet = true`.  
Với 2 người chơi, đạn của cả 2 phi thuyền đều là "đạn người chơi" → **không cần phân biệt owner**, chỉ cần flag `isPlayerBullet = true` là đủ để gây sát thương cho kẻ địch.

Không cần thay đổi hệ thống Bullet.

---

## 7. Màn hình Player Select — Chi tiết UI

### 7.1 Luồng State

```
MAIN_MENU → click stage N → pendingStageFromMenu = N → ChangeState(PLAYER_SELECT)
PLAYER_SELECT → click "1 Player"   → currentGameMode = SINGLE_PLAYER → StartStage()
PLAYER_SELECT → click "2 Players"  → currentGameMode = TWO_PLAYERS   → StartStage()
PLAYER_SELECT → click "1P + AI"    → currentGameMode = PLAYER_AND_AI → StartStage() [AI stub]
PLAYER_SELECT → click "Back"       → ChangeState(MAIN_MENU)
```

### 7.2 Hàm mới `StartStage(int stageId, GameMode mode)` trong GameManager

Gộp toàn bộ logic khởi tạo game vào một hàm riêng (refactor từ code hiện tại trong `Draw()` case `MAIN_MENU`):

```cpp
void GameManager::StartStage(int stageId, GameMode mode) {
    currentStage = stageId;
    currentWave = 1;
    currentBatch = 1;
    currentGameMode = mode;
    score = 0;
    activeItems.clear();
    CoinManager::GetInstance()->ResetSession();

    // Khởi tạo Player 1
    if (!player) {
        player = SpaceshipFactory::CreateSpaceship("Hypergun", 1, {screenWidth/2.0f, screenHeight - 100.0f});
    } else {
        player->SetPosition({screenWidth/2.0f, screenHeight - 100.0f});
        player->Heal(player->GetMaxHp());
    }
    player->SetWeapon(ShopManager::GetInstance()->GetSelectedWeapon());

    // Khởi tạo Player 2 (nếu chế độ 2 người)
    if (mode == GameMode::TWO_PLAYERS || mode == GameMode::PLAYER_AND_AI) {
        player2 = SpaceshipFactory::CreateSpaceship("Hypergun", 1, {screenWidth/2.0f + 80, screenHeight - 100.0f});
        // Player 2 dùng weapon mặc định (Hypergun) hoặc có thể thêm UI chọn sau
    } else {
        player2 = nullptr;
    }

    pendingEggSkinLoad = true;
    // Tải Stage và bắt đầu Wave 1...
}
```

---

## 8. Các file cần tạo mới / chỉnh sửa

### [MỚI] `include/GameMode.h`

```cpp
#ifndef GAMEMODE_H
#define GAMEMODE_H

enum class GameMode {
    SINGLE_PLAYER,
    TWO_PLAYERS,
    PLAYER_AND_AI   // Stub — chưa triển khai
};

#endif
```

---

### [CHỈNH SỬA] `include/GameState.h`
- Thêm `PLAYER_SELECT` vào enum `GameState`.

---

### [CHỈNH SỬA] `include/GameManager.h`
- Include `GameMode.h`.
- Thêm field `GameMode currentGameMode`.
- Thêm field `std::shared_ptr<Spaceship> player2`.
- Thêm field `Texture2D texSpaceship2`.
- Thêm field `int pendingStageFromMenu`.
- Thêm khai báo `void StartStage(int stageId, GameMode mode)`.
- Thêm getter `std::shared_ptr<Spaceship> GetPlayer2() const { return player2; }`.

---

### [CHỈNH SỬA] `src/GameManager.cpp`

#### `Init()`
- Load `texSpaceship2` từ `assets/spaceship/spaceship2.png`.

#### `CleanUp()`
- `UnloadTexture(texSpaceship2)`.
- `player2.reset()`.

#### `Update()` — case `PLAYING`
- Thêm xử lý input chuột cho `player2` (xem mục 4.2).
- Thêm kiểm tra Game Over cho 2 người (mục 5).

#### `Draw()` — case `PLAYING`
- Thêm `if (player2) player2->Draw()`.

#### `Draw()` — case `PLAYER_SELECT` (mới)
- Vẽ màn hình chọn chế độ với 3 nút.

#### `Draw()` — case `MAIN_MENU`
- Thay logic khởi tạo game trực tiếp bằng chuyển sang `PLAYER_SELECT`.
- Refactor thành gọi `StartStage()`.

---

### [CHỈNH SỬA] `src/Spaceship.cpp`
- `Draw()`: Cần phân biệt texture khi vẽ Player 1 vs Player 2.
  - Cách tiếp cận: Thêm method `SetTextureOverride(Texture2D tex)` cho `Spaceship` để Player 2 dùng `texSpaceship2`.

---

## 9. Thứ tự triển khai (Implementation Order)

```
[1] Tạo include/GameMode.h
[2] Thêm PLAYER_SELECT vào GameState.h
[3] Cập nhật GameManager.h (fields + getter mới)
[4] Load/Unload texSpaceship2 trong Init()/CleanUp()
[5] Thêm method Spaceship::SetTextureOverride()
[6] Viết hàm GameManager::StartStage() — refactor code khởi tạo cũ
[7] Viết logic Draw() case PLAYER_SELECT (UI 3 nút)
[8] Cập nhật MAIN_MENU flow: click stage → PLAYER_SELECT thay vì khởi tạo ngay
[9] Thêm Update() input chuột cho player2 trong case PLAYING
[10] Thêm Draw() player2 trong case PLAYING
[11] Cập nhật logic Game Over kiểm tra cả 2 player
[12] Test toàn bộ: 1P, 2P, back button, die P1/die P2 riêng lẻ
```

---

## 10. AI Stub (Chưa triển khai)

Khi người dùng chọn "1P + AI":
- Tạo `player2` như bình thường.
- Trong `Update()`, thay vì đọc input chuột, hiển thị placeholder:
  ```cpp
  // TODO: Implement AI behavior
  // player2 đứng yên hoặc di chuyển theo pattern đơn giản
  ```
- Sẽ được thiết kế chi tiết ở plan riêng `planAI.md`.

---

## 11. Verification Plan

| Kiểm tra | Kết quả mong đợi |
|---|---|
| Click Stage → Màn hình chọn chế độ xuất hiện | ✓ |
| Click Back → Quay về MainMenu | ✓ |
| Chọn "1 Player" → game bình thường, không có Player 2 | ✓ |
| Chọn "2 Players" → 2 phi thuyền xuất hiện, P2 bám theo chuột | ✓ |
| Chuột trái → Player 2 bắn | ✓ |
| Chuột phải → Player 2 kích hoạt Mana | ✓ |
| P2 bị giết → P1 vẫn chơi tiếp, Game Over khi P1 cũng chết | ✓ |
| P1 bị giết → P2 vẫn chơi tiếp | ✓ |
| Chọn "1P + AI" → Không crash, P2 xuất hiện dưới dạng stub | ✓ |
