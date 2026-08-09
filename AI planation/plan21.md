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

### Wave 7: "The Spiral Ambush" (Sẽ triển khai tiếp)
- **Batch 1 & 2:** Hai nhóm `chicken03` bay theo quỹ đạo hình xoắn ốc (`SpiralMovement`).

### Wave 8: "The Ring" (Sẽ triển khai tiếp)
- **Batch 1:** Sử dụng thuật toán vòng tròn bao vây.

### Wave 9: "Chaos Matrix" (Sẽ triển khai tiếp)
- Gà xuất hiện ngẫu nhiên liên tục và xả đạn đôi.
