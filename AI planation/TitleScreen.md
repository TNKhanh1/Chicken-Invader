# Implementation Plan: Title Screen (TITLE_SCREEN State)

## Background

Hiện tại khi khởi động game, `currentState` được set thẳng thành `GameState::MAIN_MENU` (màn hình chọn stage). Yêu cầu mới là thêm một màn hình trung gian **TITLE_SCREEN** hiển thị trước đó, có 2 nút:
- **CONTINUE** → vào `MAIN_MENU` với toàn bộ tiến trình, coin được giữ nguyên
- **NEW GAME** → reset tất cả (coin, tiến trình stage, shop) rồi mới vào `MAIN_MENU`

---

## Nguyên tắc thiết kế

- **Không ảnh hưởng đến bất kỳ phần nào khác**: `MAIN_MENU`, gameplay, Summary, Test Mode, Shop... không thay đổi một dòng.
- **Tách biệt class**: Logic và giao diện Title Screen đặt vào class riêng `TitleScreen` (giống pattern `ShopUI`, `SummaryScreen`, `RuneSelectionUI`).
- **Single Responsibility**: `TitleScreen` chỉ vẽ UI và trả về action; `GameManager` xử lý action đó.
- **Không đụng vào `MAIN_MENU`**: `MAIN_MENU` vẫn hoạt động y chang hiện tại (từ Back trong game, từ Summary, v.v.). Chỉ thay điểm khởi động ban đầu.

---

## Proposed Changes

---

### Step 1 — Thêm `TITLE_SCREEN` vào `GameState.h`

#### [MODIFY] `include/GameState.h`

Thêm `TITLE_SCREEN` vào đầu enum (trước `MAIN_MENU`):

```cpp
enum class GameState {
    TITLE_SCREEN, // ← THÊM MỚI
    MAIN_MENU,
    PLAYING,
    ...
};
```

> **Không đổi bất kỳ giá trị/case nào khác.** Đây chỉ là thêm 1 entry.

---

### Step 2 — Thêm `ResetAllProgress()` vào các Manager

**New Game** cần reset coin, stage, và trạng thái shop weapon/skins. Tuân thủ OOP: mỗi Manager tự biết cách reset chính nó.

#### [MODIFY] `include/CoinManager.h`

```cpp
void ResetAllProgress(); // Xóa toàn bộ coin, lưu file
```

#### [MODIFY] `src/CoinManager.cpp`

```cpp
void CoinManager::ResetAllProgress() {
    totalCoins = 0;
    sessionCoins = 0;
    stageBonusCoins = 0;
    Save(); // Ghi 0 vào coins.dat
}
```

#### [MODIFY] `include/ProgressManager.h`

```cpp
void ResetAllProgress(); // Đặt lại về stage 1, lưu file
```

#### [MODIFY] `src/ProgressManager.cpp`

```cpp
void ProgressManager::ResetAllProgress() {
    highestUnlockedStage = 1;
    SaveProgress(); // Ghi 1 vào progress.dat
}
```

---

### Step 3 — Tạo class `TitleScreen` (OOP)

#### [NEW] `include/TitleScreen.h`

```cpp
#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include "raylib.h"

enum class TitleAction {
    NONE,
    CONTINUE,
    NEW_GAME
};

class TitleScreen {
public:
    TitleScreen(int sw, int sh);
    TitleAction UpdateAndDraw(float deltaTime);
    void Reset();

private:
    int sw, sh;
    float titleScale;     // Hiệu ứng scale nẩy cho title
    float alphaTimer;     // Fade-in khi lần đầu hiện

    // Tọa độ nút, tính từ sw/sh (không hardcode)
    void DrawTitle();
    void DrawButtons(TitleAction& outAction);
};

#endif // TITLESCREEN_H
```

**Giải thích**: `TitleScreen` trả về `TitleAction` mỗi frame. `GameManager` check value đó để quyết định làm gì. Không lộ state machine ra ngoài class.

#### [NEW] `src/TitleScreen.cpp`

```cpp
#include "TitleScreen.h"
#include "FontManager.h"
#include <cmath>

TitleScreen::TitleScreen(int sw, int sh)
    : sw(sw), sh(sh), titleScale(1.0f), alphaTimer(0.0f) {}

TitleAction TitleScreen::UpdateAndDraw(float deltaTime) {
    // Fade-in effect
    alphaTimer += deltaTime * 1.5f;
    if (alphaTimer > 1.0f) alphaTimer = 1.0f;
    unsigned char alpha = (unsigned char)(255 * alphaTimer);

    // Hiệu ứng title scale nẩy nhẹ
    titleScale = 1.0f + 0.015f * sinf(GetTime() * 2.0f);

    TitleAction action = TitleAction::NONE;

    DrawTitle();
    DrawButtons(action);

    return action;
}

void TitleScreen::DrawTitle() {
    // Vẽ "CHICKEN INVADER" căn giữa màn hình phía trên
    const char* title = "CHICKEN INVADER";
    int fontSize = (int)(80 * titleScale);
    FontManager::GetInstance()->DrawGameTextCentered(
        title,
        sw / 2, sh / 2 - 120,
        fontSize,
        YELLOW, // Màu vàng nổi bật, hoặc gradient nếu dùng shader
        "Retro"
    );
}

void TitleScreen::DrawButtons(TitleAction& outAction) {
    float btnW = 280, btnH = 60;
    float btnX = sw / 2.0f - btnW / 2.0f;
    float continueY = (float)sh / 2 + 20;
    float newGameY = (float)sh / 2 + 100;

    // Nút CONTINUE
    Rectangle contBtn = { btnX, continueY, btnW, btnH };
    bool contHover = CheckCollisionPointRec(GetMousePosition(), contBtn);
    DrawRectangleRounded(contBtn, 0.3f, 8, contHover ? GOLD : DARKGRAY);
    DrawRectangleRoundedLinesEx(contBtn, 0.3f, 8, 2, contHover ? WHITE : GRAY);
    FontManager::GetInstance()->DrawGameTextCentered(
        "CONTINUE", (int)(btnX + btnW/2), (int)(continueY + 15), 28,
        contHover ? BLACK : WHITE, "Modern"
    );
    if (contHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        outAction = TitleAction::CONTINUE;

    // Nút NEW GAME
    Rectangle ngBtn = { btnX, newGameY, btnW, btnH };
    bool ngHover = CheckCollisionPointRec(GetMousePosition(), ngBtn);
    DrawRectangleRounded(ngBtn, 0.3f, 8, ngHover ? RED : DARKGRAY);
    DrawRectangleRoundedLinesEx(ngBtn, 0.3f, 8, 2, ngHover ? WHITE : GRAY);
    FontManager::GetInstance()->DrawGameTextCentered(
        "NEW GAME", (int)(btnX + btnW/2), (int)(newGameY + 15), 28,
        ngHover ? BLACK : WHITE, "Modern"
    );
    if (ngHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        outAction = TitleAction::NEW_GAME;
}

void TitleScreen::Reset() {
    alphaTimer = 0.0f;
    titleScale = 1.0f;
}
```

---

### Step 4 — Tích hợp vào `GameManager`

**Điều quan trọng nhất**: Chỉ chạm vào đúng 4 điểm sau trong `GameManager`, không đụng gì khác.

#### [MODIFY] `include/GameManager.h`

- Thêm `#include "TitleScreen.h"`
- Thêm member: `TitleScreen* titleScreen = nullptr;`

#### [MODIFY] `src/GameManager.cpp`

**Điểm 1** — Trong `Init()`: Đổi initial state và khởi tạo `titleScreen`:

```cpp
// Thay:
currentState = GameState::MAIN_MENU;
// Thành:
titleScreen = new TitleScreen(screenWidth, screenHeight);
currentState = GameState::TITLE_SCREEN;
```

**Điểm 2** — Trong `CleanUp()`: Giải phóng bộ nhớ:

```cpp
if (titleScreen) { delete titleScreen; titleScreen = nullptr; }
```

**Điểm 3** — Trong `Draw()`, thêm case mới (KHÔNG thay đổi case MAIN_MENU):

```cpp
case GameState::TITLE_SCREEN:
{
    if (titleScreen) {
        TitleAction action = titleScreen->UpdateAndDraw(GetFrameTime());
        if (action == TitleAction::CONTINUE) {
            // Giữ nguyên toàn bộ progress, vào thẳng menu
            titleScreen->Reset();
            ChangeState(GameState::MAIN_MENU);
        } else if (action == TitleAction::NEW_GAME) {
            // Reset tất cả tiến trình
            CoinManager::GetInstance()->ResetAllProgress();
            ProgressManager::GetInstance()->ResetAllProgress();
            if (mainMenuUI) mainMenuUI->UpdateStageStatus();
            titleScreen->Reset();
            ChangeState(GameState::MAIN_MENU);
        }
    }
    break;
}
```

**Điểm 4** — Trong phần background dynamic scroll, thêm `TITLE_SCREEN` vào danh sách state được cuộn:

```cpp
if (currentState != GameState::TEST_GAMEPLAY &&
    currentState != GameState::TEST_ENEMY &&
    // ... các state cũ...
    ) {
    bgY += 30.0f * deltaTime;
    // ...
}
```

`TITLE_SCREEN` **mặc định** sẽ được scroll vì nó không nằm trong danh sách loại trừ. Không cần sửa gì thêm.

---

## Luồng hoạt động sau khi implement

```
Khởi động game
    → TITLE_SCREEN
        ├── Click CONTINUE  → MAIN_MENU (progress giữ nguyên)
        └── Click NEW GAME  → Reset coin + stage → MAIN_MENU
              
MAIN_MENU → [chơi game bình thường...]
    ↓ khi thua/thắng/back
    → SUMMARY → MAIN_MENU (không về TITLE_SCREEN)
```

> [!NOTE]
> Khi chơi xong hoặc thua thì game sẽ về `MAIN_MENU` (như cũ), **không** về `TITLE_SCREEN`. Đây là behavior đúng vì `TITLE_SCREEN` chỉ là màn hình đón đầu khi mở game lần đầu.

---

## Verification Plan

### Automated
```
mingw32-make game
```
Không có lỗi compile hay linker.

### Manual Verification

| Test case | Expected |
|---|---|
| Khởi động game | Thấy màn `CHICKEN INVADER` với 2 nút |
| Click CONTINUE | Vào menu chọn stage, coin và tiến trình giữ nguyên |
| Click NEW GAME | Vào menu chọn stage, chỉ Stage 1 được mở, coin = 0 |
| Chơi xong / thua | Về `MAIN_MENU`, không về Title Screen |
| Mọi luồng khác (Back, Settings...) | Hoạt động y chang trước đây |
