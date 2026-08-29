# Implementation Plan – Sửa 5 Lỗi Hệ Thống Lớn

---

## Tổng quan vấn đề

Sau khi đọc kỹ từng dòng code, phát hiện **5 lỗi hệ thống** cần được xử lý theo thứ tự ưu tiên. Mỗi vấn đề đều được phân tích nguyên nhân gốc rễ và đưa ra giải pháp chi tiết ở mức code.

---

## BUG 1: Thua game vẫn unlock stage mới
**Mức độ: 🔴 CRITICAL**

### Nguyên nhân gốc rễ
`GameManager.cpp:1070` — Khi cả P1 và P2 chết (`p1Dead && p2Dead`), code đang gọi:
```cpp
ProgressManager::GetInstance()->UnlockStage(currentStage + 1); // ← SAI
currentState = GameState::GAME_OVER;
```
Đây là **lỗi logic rõ ràng**: Game Over không nên unlock stage mới, chỉ nên unlock khi **thắng**.

### Giải pháp
Xóa lệnh `UnlockStage` khỏi block Game Over. Lệnh unlock stage ở khối **thắng** (`GameManager.cpp:888`) là đúng và không cần sửa.

### Thay đổi
**[MODIFY] `src/GameManager.cpp` ~line 1064–1073**
```diff
 if (p1Dead && p2Dead) {
     int totalW = WaveManager::GetInstance()->GetTotalWaves();
     CoinManager::GetInstance()->CalculateStageBonus(currentWave, totalW, false);
     CoinManager::GetInstance()->CommitSessionCoins();
     if (player) player->ClearAllBuffs();
     if (player2) player2->ClearAllBuffs();
-    ProgressManager::GetInstance()->UnlockStage(currentStage + 1);  // BUG: xóa dòng này
-    if (mainMenuUI) mainMenuUI->UpdateStageStatus();
     currentState = GameState::GAME_OVER;
 }
```

---

## BUG 2: Không reset chỉ số và lõi khi thua/thắng/back
**Mức độ: 🔴 CRITICAL**

### Nguyên nhân gốc rễ
Khi người chơi thắng hoặc thua và quay về menu:

**Trường hợp THẮNG (line 884–894):** Chỉ gọi `player->Heal()` và `ClearAllBuffs()`, nhưng **không reset**:
- `permanentDamageBonus`
- `activeArguments` (danh sách Lõi)
- Tất cả chỉ số tăng vĩnh viễn (MaxHP, Armor, CritChance, CritDamage, FireRate) đã tích lũy

**Trường hợp THUA (line 1329–1338):** Chỉ tạo `player` mới thô bằng `SpaceshipFactory::CreateSpaceship(...)` nhưng **không reset player2**. Quan trọng hơn, đây là xử lý ở tầng **Draw()**, không phải Update() — tức là nó phụ thuộc vào click nút, không đảm bảo được gọi đúng thời điểm.

**Trường hợp BACK (chưa có code xử lý):** Không có nơi nào dọn dẹp khi người chơi bấm "Thoát giữa chừng" từ menu Settings hay ESC.

### Phân tích kỹ: Tại sao chỉ tạo player mới là chưa đủ?
Vì `player` là `shared_ptr<Spaceship>` nhưng thực chất nó trỏ tới một `SpaceshipDecorator` bọc bên ngoài `Spaceship` thật. Các chỉ số vĩnh viễn (`permanentDamageBonus`, `activeArguments`, `maxHp` đã cộng thêm...) đều nằm trong `Spaceship` lõi bên trong. Khi `StartStage()` được gọi lần 2 (chơi lại), nhánh `else` sẽ được kích hoạt:
```cpp
} else {
    player->SetPosition(...);
    player->Heal(player->GetMaxHp()); // ← Hồi máu MAX hiện tại (đã bị tăng)
}
```
Điều này có nghĩa là nếu người chơi đã tăng `+30 MaxHP` ở round trước và thua, khi chơi lại họ vẫn giữ `+30 MaxHP` và các Lõi cũ.

### Giải pháp
**Bước 1:** Thêm hàm `ResetToBaseStats()` vào `Spaceship`.
Hàm này gọi `ReloadStatsFromCSV()` (đã có) để khôi phục toàn bộ chỉ số về gốc, sau đó clear thêm phần tăng thêm:
```cpp
void Spaceship::ResetToBaseStats() {
    // Đặt lại level về 1, tải lại chỉ số gốc từ CSV
    level = 1;
    ReloadStatsFromCSV();
    // Xóa toàn bộ chỉ số tích lũy
    permanentDamageBonus = 0.0f;
    activeArguments.clear();
    ClearAllBuffs();
}
```

**Bước 2:** Thêm override vào `SpaceshipDecorator`:
```cpp
void SpaceshipDecorator::ResetToBaseStats() { coreShip->ResetToBaseStats(); }
```

**Bước 3:** Gọi `ResetToBaseStats()` ở mọi điểm thoát khỏi game:
- **Thắng stage:** Sau dòng `ChangeState(GameState::MAIN_MENU)` 
- **Thua game:** Thay vì tạo player mới (`SpaceshipFactory::CreateSpaceship()`), gọi `player->ResetToBaseStats()` trực tiếp (tiết kiệm hơn, không phải cấp phát bộ nhớ mới)
- **Back từ Pause/Settings:** Thêm nút Abandon Stage, khi nhấn gọi Reset trước khi về menu

**Thay đổi files:**

**[MODIFY] `include/Spaceship.h`** — Thêm khai báo:
```cpp
virtual void ResetToBaseStats();
```

**[MODIFY] `src/Spaceship.cpp`** — Thêm implementation:
```cpp
void Spaceship::ResetToBaseStats() {
    level = 1;
    ReloadStatsFromCSV(); // Tải lại HP, Dmg, Armor... về gốc Level 1
    permanentDamageBonus = 0.0f;
    activeArguments.clear();
    ClearAllBuffs();
}
```

**[MODIFY] `include/SpaceshipDecorator.h`** — Thêm override:
```cpp
virtual void ResetToBaseStats() override;
```

**[MODIFY] `src/SpaceshipDecorator.cpp`** — Thêm pass-through:
```cpp
void SpaceshipDecorator::ResetToBaseStats() { coreShip->ResetToBaseStats(); }
```

**[MODIFY] `src/GameManager.cpp` — Khối thắng stage (line ~884):**
```diff
 ProgressManager::GetInstance()->UnlockStage(currentStage + 1);
 if (mainMenuUI) mainMenuUI->UpdateStageStatus();
+if (player) player->ResetToBaseStats();
+if (player2) player2->ResetToBaseStats();
 ChangeState(GameState::MAIN_MENU);
-if (player) {
-    player->Heal(player->GetMaxHp());  // Không dùng nữa, đã reset toàn bộ
-    player->ClearAllBuffs();
-}
```

**[MODIFY] `src/GameManager.cpp` — Khối GAME_OVER Draw button "BACK TO MENU" (line ~1329):**
```diff
 if (DrawButton({...}, "BACK TO MENU")) {
     currentState = GameState::MAIN_MENU;
     score = 0;
     activeEnemies.clear();
     activeBullets.clear();
     activeItems.clear();
-    player = SpaceshipFactory::CreateSpaceship(...); // Bỏ dòng tạo mới
-    player->SetShootingBehavior(...);
+    if (player) player->ResetToBaseStats();
+    if (player2) player2->ResetToBaseStats();
 }
```

> **Lưu ý quan trọng:** `StartStage()` cũng cần gọi `ResetToBaseStats()` thay vì chỉ `Heal()` để đảm bảo player luôn sạch khi bắt đầu stage mới:
```diff
 } else {
     player->SetPosition({...});
-    player->Heal(player->GetMaxHp());
+    player->ResetToBaseStats();
+    player->SetPosition({...}); // Sau reset, set lại vị trí
 }
```

---

## BUG 3: Đạn player 2 dùng chỉ số của player 1
**Mức độ: 🟡 HIGH**

### Nguyên nhân gốc rễ
Trong toàn bộ collision loop (`GameManager.cpp:920–1000`), logic xử lý sát thương đạn của **player bullet** luôn tham chiếu đến `player` (Player 1):
```cpp
bool isCrit = (GetRandomValue(0, 100) < player->GetCritChance()); // Luôn là P1
float finalDamage = bullet->GetDamage() + player->GetPermanentDamageBonus(); // Luôn là P1
if (player->HasArgument(5)) player->AddPermanentDamage(2.0f); // Luôn cộng vào P1
if (player->HasArgument(6)) player->Heal(player->GetMaxHp() * 0.05f); // Luôn hồi P1
```
Không có cách nào để biết viên đạn đó do P1 hay P2 bắn ra.

### Giải pháp
**Bước 1:** Thêm field `Spaceship* shooter` vào class `Bullet`. 

**[MODIFY] `include/Bullet.h`** — Thêm field và getter/setter:
```cpp
private:
    Spaceship* shooter = nullptr; // Con trỏ thô (không sở hữu)
public:
    void SetShooter(Spaceship* s) { shooter = s; }
    Spaceship* GetShooter() const { return shooter; }
```

**Bước 2:** Khi player hoặc player2 bắn đạn, gán `shooter`:
**[MODIFY] `src/GameManager.cpp`** — Sau mỗi đoạn `player->Fire()` / `player2->Fire()`:
- Lấy batch đạn mới nhất từ `pendingBullets` và set shooter cho chúng.
- Cách đơn giản hơn: tạo helper `FireForPlayer(Spaceship* p)` nhận con trỏ người chơi và set shooter ngay sau khi Fire().

> **Thiết kế thay thế (sạch hơn):** Thêm vào `IShootingBehavior::Fire()` tham số `Spaceship* shooter` và truyền xuống để Bullet tự set. Tuy nhiên cách này đụng đến nhiều file hơn, có thể để P2 scope sau.

**Bước 3:** Trong collision loop, dùng `bullet->GetShooter()` thay cho `player`:
```diff
 if (hit) {
-    bool isCrit = (GetRandomValue(0, 100) < player->GetCritChance());
-    float finalDamage = bullet->GetDamage() + player->GetPermanentDamageBonus();
+    Spaceship* shooter = bullet->GetShooter() ? bullet->GetShooter() : player.get();
+    bool isCrit = (GetRandomValue(0, 100) < shooter->GetCritChance());
+    float finalDamage = bullet->GetDamage() + shooter->GetPermanentDamageBonus();
     if (isCrit) finalDamage *= (shooter->GetCritDamage() / 100.0f);
     if (shooter->HasArgument(3) && enemy->role == EnemyRole::BOSS) finalDamage *= 1.8f;
     ...
     if (!enemy->IsActive()) {
-        if (player->HasArgument(5)) player->AddPermanentDamage(2.0f);
-        if (player->HasArgument(6)) player->Heal(player->GetMaxHp() * 0.05f);
+        if (shooter->HasArgument(5)) shooter->AddPermanentDamage(2.0f);
+        if (shooter->HasArgument(6)) shooter->Heal(shooter->GetMaxHp() * 0.05f);
     }
 }
```
Tương tự cho beam weapon damage loop (~line 748–790).

---

## BUG 4: Màn hình chọn Chỉ Số và Lõi chỉ có Player 1
**Mức độ: 🟡 HIGH**

### Nguyên nhân gốc rễ
`STAT_SELECTION` và `ARGUMENT_SELECTION` hiện tại chỉ apply cho `player`:
```cpp
if (player) { player->AddPermanentMaxHp(30.0f); }
// player2 không được nhận gì
```

### Giải pháp (Đơn giản, tối ưu cho Co-op)
Áp dụng **cùng một lựa chọn cho cả 2 player** khi ở chế độ TWO_PLAYERS. Điều này công bằng và không làm phức tạp UI:

**[MODIFY] `src/GameManager.cpp` — STAT_SELECTION handler:**
```diff
 switch (statIdx) {
     case 0:
         player->AddPermanentMaxHp(30.0f);
+        if (player2) player2->AddPermanentMaxHp(30.0f);
         break;
     // ... tương tự cho các case còn lại
 }
```

**[MODIFY] `src/GameManager.cpp` — ARGUMENT_SELECTION handler:**
```diff
 if (player) {
     player->AddArgument(argId);
+    if (player2) player2->AddArgument(argId);
     if (argId == 9) { // Fast Track
         player->LevelUp(); player->LevelUp(); player->LevelUp();
+        if (player2) { player2->LevelUp(); player2->LevelUp(); player2->LevelUp(); }
     }
 }
```

---

## BUG 5: Boss AI chỉ nhắm Player 1
**Mức độ: 🟢 MEDIUM**

### Nguyên nhân gốc rễ
`Bosses.cpp` gọi `gm->GetPlayer()` để lấy target, không quan tâm đến Player 2.

### Giải pháp
Thêm helper `GetNearestPlayer(Vector2 from)` vào `GameManager`:

**[MODIFY] `include/GameManager.h`** — Thêm khai báo:
```cpp
Spaceship* GetNearestPlayer(Vector2 from) const;
```

**[MODIFY] `src/GameManager.cpp`** — Thêm implementation:
```cpp
Spaceship* GameManager::GetNearestPlayer(Vector2 from) const {
    Spaceship* target = nullptr;
    float minDist = FLT_MAX;
    if (player && player->IsActive()) {
        Vector2 d = {player->GetPosition().x - from.x, player->GetPosition().y - from.y};
        float dist = d.x*d.x + d.y*d.y;
        if (dist < minDist) { minDist = dist; target = player.get(); }
    }
    if (player2 && player2->IsActive()) {
        Vector2 d = {player2->GetPosition().x - from.x, player2->GetPosition().y - from.y};
        float dist = d.x*d.x + d.y*d.y;
        if (dist < minDist) { minDist = dist; target = player2.get(); }
    }
    return target;
}
```

**[MODIFY] `src/Bosses.cpp`** — Thay thế `gm->GetPlayer()` bằng `gm->GetNearestPlayer(position)` tại tất cả điểm Boss cần target.

---

## Thứ tự thực hiện

| Thứ tự | Bug | Mức độ | Lý do ưu tiên |
|--------|-----|--------|---------------|
| 1 | BUG 1: Unlock stage khi thua | 🔴 CRITICAL | 1 dòng fix, không rủi ro |
| 2 | BUG 2: Reset stats khi thua/thắng/back | 🔴 CRITICAL | Ảnh hưởng trực tiếp game balance |
| 3 | BUG 4: Stat selection cho P2 | 🟡 HIGH | Phụ thuộc vào BUG 2 |
| 4 | BUG 3: Bullet owner P2 | 🟡 HIGH | Ảnh hưởng gameplay P2 |
| 5 | BUG 5: Boss AI target | 🟢 MEDIUM | Ít ảnh hưởng nhất |

---

## Files thay đổi

| File | Loại | Ghi chú |
|------|------|---------|
| `src/GameManager.cpp` | MODIFY | Fix BUG 1, 2, 3, 4 |
| `include/Spaceship.h` | MODIFY | Thêm `ResetToBaseStats()` |
| `src/Spaceship.cpp` | MODIFY | Implement `ResetToBaseStats()` |
| `include/SpaceshipDecorator.h` | MODIFY | Thêm override |
| `src/SpaceshipDecorator.cpp` | MODIFY | Pass-through |
| `include/Bullet.h` | MODIFY | Thêm `shooter` field |
| `src/Bullet.cpp` | MODIFY | Init shooter = nullptr |
| `include/GameManager.h` | MODIFY | Khai báo `GetNearestPlayer()` |
| `src/Bosses.cpp` | MODIFY | Thay thế target logic |

---

> **Cam kết:** Mọi thay đổi đều được cô lập, không phá vỡ logic hiện tại. Sau khi sửa xong sẽ chạy `mingw32-make` và kiểm tra 0 lỗi biên dịch.
