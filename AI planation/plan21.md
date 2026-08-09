# Plan 21: Stage 2 (Waves 6-9) - "Double Trouble" (chicken03)

## 1. Mục Tiêu

- Thiết kế **Wave 6 đến Wave 9** của Stage 2 (Giai đoạn cao trào trước Final Boss).
- Sử dụng đồng loạt **`chicken03.png`** làm Gà Thường cho tất cả các Batch (số lượng đông đảo).
- Tính năng đặc biệt: `chicken03` đôi lúc sẽ thả 2 quả trứng liên tiếp.
- Kích thước chuẩn 100x100.
- Tạo ra các **đội hình (formations) sáng tạo**, số lượng lớn (trên 8 con/batch).

---

## 2. Animation & Logic (Trọng tâm)

- Đã sinh ra `chicken03_anim.png` kích thước 2400x100.
- Cứ mỗi lần thả trứng, có 30% tỷ lệ kích hoạt cơ chế `pendingDoubleShots` (thả thêm 1 quả trứng ngay sau quả đầu tiên).
- Sử dụng 100% `chicken03` thay vì mix với `chicken02` để tạo sự đồng nhất.

---

## 3. Lịch Sử Debug & Cập Nhật Engine

Trong quá trình phát triển Wave 6, một số lỗi Engine đã được phát hiện và khắc phục triệt để:
- **Mở khóa giới hạn Wave**: Ở `GameManager.cpp`, Stage 2 trước đây bị khóa cứng giới hạn ở Wave 5 (gây ra hiện tượng đơ nút khi chọn Wave 6). Đã nâng giới hạn lên 10.
- **Vá lỗi Missing Movement**: Bổ sung cơ chế `HORIZONTAL_SWEEP` vào trình phân tích JSON (`WaveManager::SpawnBatch`). Trước đó game không parse được cơ chế này, khiến gà sinh ra nhưng kẹt vĩnh viễn ở ngoài màn hình (lỗi kết thúc Batch sớm).
- **Tinh chỉnh khoảng cách (Spacing)**: Khoảng cách dọc (spacing_y) của đội hình chữ V được nâng lên 110px để gà không bị dính vào nhau do sprite có kích thước 100x100.

---

## 4. Kịch Bản Chi Tiết (Waves 6-9)

### Wave 6: "The Swarm" (Đã hoàn thiện & Test ổn định)
- **Batch 1:** Hình chữ V (V-shape) gồm **11 con** gà `chicken03` giăng ngang, nảy hai bên màn hình. Đã giãn cách y=110px để không đè lên nhau.
- **Batch 2:** Đội hình lưới (Grid) 2 hàng 5 cột, tổng cộng **10 con** `chicken03` bay từ trên xuống và nảy nhẹ (HORIZONTAL_BOUNCE drift = 0).
- **Batch 3:** Đội hình ngang hàng ngang **8 con** `chicken03` quét ngang ở dưới đáy màn hình (HORIZONTAL_SWEEP start_y=700).

### Wave 7: "The Spiral Ambush" (Đã hoàn thiện)
- **Batch 1:** Đội hình chữ V lớn (9 con) rơi từ trên xuống theo chuyển động zíc-zắc đứng (`VERTICAL_ZIGZAG`).
- **Batch 2:** Đội hình lưới (Grid 2x5) bay lượn lờ chao đảo dưới dạng lốc xoắn ốc (`SPIRAL`). Đã tích hợp thuật toán **Wrap-around** giúp gà vòng lên trên đỉnh khi rớt xuống đáy màn hình, lặp lại vô tận cho tới khi bị diệt.

### Wave 8: "The Ring" (Đã hoàn thiện)
- **Batch 1:** Khởi tạo thuật toán sinh trận pháp hoàn toàn mới (OOP): `BuildRing`. 12 con gà xuất phát từ ngoài màn hình lao vào bao vây thành một vòng tròn quanh trung tâm (`WAYPOINT`).
- **Batch 2:** Đội hình chữ V lớn (11 con) sử dụng `HORIZONTAL_BOUNCE` với gia tốc biên (drift) bằng 0. Gà bay thẳng xuống lơ lửng giữa màn hình để xả đạn.

### Wave 9: "Chaos Matrix" (Đã hoàn thiện)
- **Batch 1:** Mưa trứng (`RANDOM_RAIN`) gồm 15 con gà rơi tự do, kết hợp thuật toán **Wrap-around** trong `StraightMovement` giúp mưa lặp lại vô tận.
- **Batch 2:** Đội hình chữ X giao nhau (`INTERSECTING_V`). Tốc độ nén xuống mượt mà (Speed = 80) để gà bay qua bay lại nhẹ nhàng, không bị giật.

---

## 5. Báo Cáo Kiểm Tra Tương Thích (Regression Test)
Đã kiểm tra kỹ lưỡng các sửa đổi đối với hệ thống Movement (`StraightMovement`, `SpiralMovement`, v.v.). **Kết quả:** Không có bất kỳ Wave nào trong Stage 1 bị ảnh hưởng, vì `STRAIGHT` và `SPIRAL` chỉ được gọi ra ở Stage 2 (Waves 7 và 9). Các tính năng cũ của Wave Manager hoàn toàn nguyên vẹn và độc lập!


> [!IMPORTANT]
> LƯU Ý KHI THIẾT KẾ CÁC WAVE VÀ STAGE MỚI: Tuyệt đối không được sửa đổi, tái cấu trúc (refactor), hay làm ảnh hưởng đến logic của các wave và stage cũ đã được duyệt. Phải luôn đảm bảo tính toàn vẹn của code cũ.
