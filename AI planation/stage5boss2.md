# Plan: Boss Wave 15 Stage 5 — Void Chicken (chicken12.png)

## 1. Phân tích nguyên nhân lỗi trước đó

### Vấn đề 1: Sai visualId mapping → Boss hiển thị sai hình
Hệ thống hiện tại load texture theo công thức:
```
Boss::Draw() → gm->GetTexEnemyAnim(visualId - 1)
GetTexEnemyAnim(index) → if (index >= 0 && index < 13) return texEnemyAnims[index]
```

**Bản đồ mapping hiện tại:**

| Index | File                                | visualId | Đang dùng cho         |
|-------|-------------------------------------|----------|-----------------------|
| 0–10  | chicken01_anim.png → chicken11_anim.png | 1–11 | Enemy thường + Boss cũ |
| 11    | Militarychicken_anim.png            | 12       | Stage 7 Boss (Military) |
| 12    | CI4_SuperChick_Summer_anim.png      | 13       | Stage 7 Boss (SuperChick) |
| 13    | chicken12_anim.png                  | 14 (tôi đặt) | VoidChickenBoss ← **BỊ CHẶN!** |

> **LỖI NGHIÊM TRỌNG:** `GetTexEnemyAnim()` giới hạn cứng `index < 13`, nên `texEnemyAnims[13]` (chicken12_anim.png) **KHÔNG BAO GIỜ được trả về** → Boss fallback hiển thị `texEnemyAnims[0]` = chicken01.

**Giải pháp:** Sửa giới hạn `GetTexEnemyAnim()` thành `< 20` (bằng kích thước mảng).

### Vấn đề 2: Animation sprite sheet sai format
- Các enemy/boss chuẩn dùng **24 frames vuông** (WxH = 24*H x H).
- `chicken12_anim.png` hiện có: **2520×241** (10 frames, mỗi frame 252×241, KHÔNG vuông).
- `Boss::Draw()` và `Enemy::Draw()` đều dùng `frameSize = tex.height` và `currentAnimFrame % 24`, nên 10 frames sẽ bị lặp/cắt sai.

**Giải pháp:** Tạo lại `chicken12_anim.png` đúng chuẩn **24 frames vuông** bằng `batch_warp_v3.py`.

---

## 2. Phân tích ảnh gốc chicken12.png

- **Kích thước gốc:** 212×201 px, RGBA
- **Mô tả:** Con gà mặc giáp cơ khí kim loại màu xám đen, cánh trải rộng hai bên (cánh lông trắng tím), đầu tròn (mũ bảo hiểm trong suốt), chân đi giày boot cơ khí.
- **Đặc điểm quan trọng cho animation:**
  - Cánh trải rộng đối xứng hai bên, cần vỗ cánh mạnh
  - Thân bọc giáp trung tâm, cần body bob
  - Chân boot hai bên, cần leg spread

### Pivot tính toán cho chicken12.png (212px wide → resize về 100px)

Theo tỉ lệ 100/212:
- **Left wing pivot:** ~30 (cánh bắt đầu từ ~63px gốc → 63*100/212 ≈ 30)
- **Right wing pivot:** ~70 (cánh phải bắt đầu từ ~147px gốc → 147*100/212 ≈ 69)

→ **Pivot mặc định `left=30, right=70` của `batch_warp_v3.py` phù hợp hoàn toàn!**

---

## 3. Kế hoạch thực hiện

### Bước 1: Tạo lại animation sprite sheet chuẩn

Dùng `batch_warp_v3.py` để gen `chicken12_anim.png` với **24 frames vuông 100×100** → file output `2400×100`:

```bash
python scratch/batch_warp_v3.py assets/enemy/chicken12.png assets/enemy/chicken12_anim.png
```

### Bước 2: Sửa giới hạn `GetTexEnemyAnim()`

**[MODIFY] GameManager.h (dòng 184)**
```diff
-    if (variantIndex >= 0 && variantIndex < 13) return texEnemyAnims[variantIndex];
+    if (variantIndex >= 0 && variantIndex < 20) return texEnemyAnims[variantIndex];
```

### Bước 3: Sửa giới hạn UnloadTexture

**[MODIFY] GameManager.cpp (dòng 1589)**
```diff
-    for (int i = 0; i < 13; i++) UnloadTexture(texEnemyAnims[i]);
+    for (int i = 0; i < 20; i++) UnloadTexture(texEnemyAnims[i]);
```

### Bước 4: Verify visualId mapping
- Texture `chicken12_anim.png` load vào `texEnemyAnims[13]` (dòng 294)
- `visualId = 14` → `visualId - 1 = 13` → trả đúng `texEnemyAnims[13]`
- EnemyFactory: `visualId == 14` → `VoidChickenBoss` ✅
- stage5.json Wave 15: `visual_id: 14` ✅

---

## 4. Thiết kế hệ thống tấn công Boss

### 4.1 Bắn thường — Đạn tròn đỏ (bulletType = 3)
- **Mô tả:** Bắn ra nhiều viên đạn tròn đỏ hướng xuống dưới, spread nhẹ.
- **Rendering:** bulletType 3 đã có sẵn (đạn gradient đỏ tròn)
- **Logic:** Mỗi 0.2s bắn 1 lượt 3 viên (spread -20deg, 0deg, +20deg), tổng 8 lượt.
- **Dùng:** SpreadTrajectory đã có sẵn.

### 4.2 Kỹ năng 1 — Tia Laser đỏ (bulletType = 5)
- **Mô tả:** Bắn vài tia laser đỏ hướng về phía phi thuyền. Tia laser là **đoạn thẳng** (có chiều dài cố định, không phải đường thẳng vô hạn).
- **Cải thiện rendering:** Thêm 3 lớp glow cho đẹp:
  - Lớp ngoài: DrawLineEx, radius*3, {255, 50, 50, 100} — hào quang đỏ mờ
  - Lớp giữa: DrawLineEx, radius*2, {255, 80, 80, 200} — thân laser đỏ
  - Lớp trong: DrawLineEx, radius*0.8, {255, 220, 220, 255} — lõi trắng hồng sáng
- **Logic:** Bắn 4 tia laser cách nhau 0.4s, mỗi tia hướng thẳng về vị trí player tại thời điểm bắn.
- **Dùng:** SpreadTrajectory với góc tính từ atan2(player - boss).

### 4.3 Kỹ năng 2 — Đạn tam giác homing (bulletType = 4)
- **Mô tả:** Bắn ra 12 viên đạn tam giác **đều, không quá to** xung quanh boss (360deg), đạn có quỹ đạo homing nhẹ.
- **Chỉnh kích thước tam giác:** Giảm tip từ radius*2.0 xuống radius*1.5, base từ radius xuống radius*0.8 → tam giác đều nhỏ gọn hơn.
- **Logic:**
  - 12 viên phân bố đều 360deg (mỗi viên cách 30deg)
  - Speed: 200px/s (chậm hơn đạn thường)
  - Turn speed: 50deg/s (≈ 0.87 rad/s) — nhẹ nhàng, phi thuyền di chuyển nhanh hoàn toàn né được
- **Dùng:** HomingTrajectory đã có sẵn.

---

## 5. Danh sách thay đổi file

### [MODIFY] GameManager.h
- Sửa `GetTexEnemyAnim()` giới hạn từ `< 13` sang `< 20`

### [MODIFY] GameManager.cpp
- Sửa `UnloadTexture` loop từ `< 13` sang `< 20`

### [MODIFY] Bullet.cpp
- Cải thiện rendering laser (bulletType 5): thêm 3 lớp glow
- Chỉnh kích thước tam giác (bulletType 4): nhỏ hơn, đều hơn

### [REGENERATE] chicken12_anim.png
- Tạo lại bằng `batch_warp_v3.py` → 24 frames vuông 100×100 → file 2400×100

### Các file KHÔNG thay đổi (đã đúng):
- `Bosses.h` — class VoidChickenBoss kế thừa Boss đúng
- `Bosses.cpp` — logic Update (State Machine) đã đúng, bulletType 3/4/5 đúng
- `EnemyFactory.cpp` — visualId == 14 → VoidChickenBoss đúng
- `BulletTrajectory.h/cpp` — HomingTrajectory đúng
- `data/stage5.json` — Wave 15 visual_id=14 đúng

---

## 6. Kiểm tra ảnh hưởng đến wave/stage đã hoàn thiện

| Thay đổi | Ảnh hưởng? | Giải thích |
|---|---|---|
| `GetTexEnemyAnim` giới hạn `< 13 → < 20` | Không | Index 0–12 vẫn hoạt động y nguyên, chỉ mở thêm index 13–19 |
| `UnloadTexture` loop `< 13 → < 20` | Không | Index 13–19 đều `{0}` (chưa load) → UnloadTexture bỏ qua |
| Tạo lại `chicken12_anim.png` | Không | File mới thay file cũ, chỉ boss mới dùng |
| Sửa rendering bulletType 4, 5 | Chỉ ảnh hưởng VoidChickenBoss | Không boss nào khác dùng bulletType 4 hoặc 5 |
| `EnemyFactory` thêm visualId 14 | Không | Chỉ kích hoạt khi wave data chứa visualId=14 |

---

## 7. Verification Plan

1. Chạy `batch_warp_v3.py` → verify output `2400×100`, 24 frames
2. `mingw32-make game` → compile thành công
3. F1 → Stage 5 Wave 15 → verify:
   - Boss hiển thị đúng hình chicken12 (gà giáp cơ khí)
   - Animation vỗ cánh mượt 24 frames
   - Đạn tròn đỏ bắn chùm
   - Tia laser đỏ có glow 3 lớp hướng về player
   - Đạn tam giác nhỏ homing nhẹ nhàng
4. F1 → Stage 1~5 các wave cũ → verify không bị ảnh hưởng
