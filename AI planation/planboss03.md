# Plan Boss 03: Eskimo Chicken Boss — Stage 4, Wave 10

## 1. Mục Tiêu

Thêm **Boss Eskimo (chicken08)** vào Stage 4, Wave 10. Boss này sẽ:
- Sử dụng hình ảnh `chicken08.png` (Gà mặc áo khoác lông Eskimo).
- Có **2 mạng (2 phases)** với cơ chế chuyển dạng (Transition) giống Boss 01 và Boss 02.
- **Sử dụng 2 loại đạn hoàn toàn mới:**
  - **Đạn tròn xanh dương:** Bắn nhanh, tỏa ra nhiều hướng.
  - **Đạn cầu tuyết (Snowball):** Bắn chậm nhưng **kích thước to dần** theo thời gian bay.
- Quá trình làm Animation (warping) sẽ được xử lý **cực kỳ cẩn thận** bằng phương pháp khoanh vùng (Bounding Box) do màu lông ở cánh/đùi trùng với màu thân áo.

> [!IMPORTANT]
> - Yêu cầu người dùng review và phê duyệt thiết kế đạn, chu kỳ tấn công và hướng giải quyết Animation trước khi tiến hành code.

---

## 2. Giải Pháp Xử Lý Animation (chicken08.png)

Vì cánh, đùi và thân của gà 08 đều dùng chung dải màu nâu (lông áo khoác), phương pháp Floodfill tự động (loang màu) ở các script trước sẽ bị "lem" từ cánh vào thân.

**Giải pháp:**
- Viết một Python Script mới (`patch_boss03.py`) chuyên biệt cho `chicken08.png`.
- Thay vì chỉ dùng màu, script sẽ **định nghĩa trước các hình chữ nhật (Bounding Box)** giới hạn khu vực của:
  - Cánh trái (Chứa cả phần lông nâu và đầu cánh cyan).
  - Cánh phải.
  - Chân trái, chân phải.
- Áp dụng các thuật toán cắt, xoay (Rotate) và co giãn tĩnh trên các mảnh đã cắt này thay vì dùng lưới Warp phức tạp, đảm bảo giữ nguyên hình dạng khối lông áo mà vẫn tạo được chuyển động đập cánh, đạp chân rõ rệt.

---

## 3. Thiết Kế Chi Tiết Cơ Chế Đạn & Kỹ Năng

### 3.1 Hệ Thống Đạn Mới

**1. Đạn Xanh Dương (Blue Round Bullet)**
- **Visual:** Vẽ bằng Code (Procedural) sử dụng `DrawCircleGradient`. Lõi màu trắng/cyan sáng, viền ngoài màu xanh dương đậm (DARKBLUE), có vệt sáng mờ (glow).
- **Tính chất:** Tốc độ bay nhanh (300px/s), kích thước nhỏ (Hitbox r=12).

**2. Đạn Cầu Tuyết (Snowball Bullet)**
- **Visual:** Vẽ vòng tròn gradient Trắng -> Xám nhạt, có các hạt "bụi tuyết" (sparkles) li ti bay lả tả quanh quả cầu.
- **Tính chất đặc biệt:** Kích thước **TO DẦN** trong lúc bay.
  - Lúc mới bắn: Bán kính `r = 10`.
  - Giãn nở tối đa: Bán kính `r = 45` sau 2 giây.
  - **Code:** Yêu cầu phải thêm hàm `SetRadius(float)` vào lớp `Bullet` gốc (trong `Bullet.h`) để cập nhật Hitbox vật lý linh động theo hình ảnh hiển thị.

### 3.2 Chu Kỳ Tấn Công (Attack Cycle)

**Phase 1 (Giai đoạn đầu):**
- Bắn **Đạn Xanh Dương (x3 đợt liên tục):** Mỗi đợt bắn ra 5 viên chùm ngang hình quạt.
- Bắn **Kỹ năng Cầu Tuyết:** Phóng 1 quả Cầu Tuyết khổng lồ thẳng xuống vị trí người chơi.
- (Lặp lại)

**Phase 2 (Nổi điên):**
- Bắn **Đạn Xanh Dương (x4 đợt liên tục):** Bắn 7-9 viên tỏa vòng cung.
- Bắn **Kỹ năng Cầu Tuyết:** Phóng **3 quả Cầu Tuyết** tỏa ra 3 hướng (trái, giữa, phải). Chúng sẽ đồng loạt phình to ra choán hết khoảng không.

---

## 4. Cấu Trúc Lớp (OOP & Design Pattern)

Tương tự hệ thống Boss hiện có, chúng ta sẽ mở rộng `Bosses.h/cpp`:

1. Cập nhật **Base Class `Bullet`**: Thêm phương thức `void SetRadius(float r);`.
2. Tạo **Class `BlueRoundBullet : public Bullet`**: Tự quản lý việc vẽ gradient xanh.
3. Tạo **Class `SnowballBullet : public Bullet`**: 
   - Có biến `age` đếm thời gian.
   - Ghi đè `Update(deltaTime)`: Gọi `SetRadius()` để nội suy (lerp) kích thước từ 10 -> 45. Ghi đè `Draw()` để vẽ cầu tuyết bự.
4. Tạo **Class `EskimoBoss : public Boss`**:
   - Chứa state logic quản lý 2 phases.
   - Thêm hiệu ứng hạt tuyết (Snow flakes particles) rụng lả tả thay vì tia lửa.

---

## 5. Kế Hoạch Cập Nhật Files

1. **[NEW SCRIPT]** `scratch/patch_boss03.py`: Script sinh sprite sheet 24 frames cho `chicken08_anim.png` dùng bounding box cropping.
2. **[MODIFY]** `include/Bullet.h` & `src/Bullet.cpp`: Bổ sung cơ chế resize Hitbox (`SetRadius`).
3. **[MODIFY]** `include/Bosses.h` & `src/Bosses.cpp`: Cài đặt toàn bộ logic `EskimoBoss`, `BlueRoundBullet`, `SnowballBullet`.
4. **[MODIFY]** `src/EnemyFactory.cpp`: Thêm case `if (role == BOSS && visualId == 8)` tạo `EskimoBoss`.
5. **[MODIFY]** `data/stage4.json`: Cấu hình Wave 10 spawn Boss 08.

---

## 6. Open Questions (Cần Feedback)

1. Cầu Tuyết khi phình to ra tối đa sẽ bằng khoảng bao nhiêu % kích thước phi thuyền người chơi? (Mặc định tôi đang set `r=45`, phi thuyền người chơi khoảng `r=25`).
2. Tốc độ bay của Cầu Tuyết nên nhanh hay chậm? (Đề xuất: Chậm, khoảng `150px/s` để tăng cảm giác áp lực khi nó cứ to dần mà người chơi không vội né được).
