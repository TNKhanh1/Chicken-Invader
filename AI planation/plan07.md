# Plan 07: Audio System, Item Collection, and Additional Behaviors

Dựa trên cấu trúc đã hoàn thiện ở Plan 06 (Wave/Batch Architecture, Test Gameplay, v.v.), Plan 07 sẽ tập trung vào các tính năng cơ bản tiếp theo nhằm nâng cao trải nghiệm (gamefeel) và hoàn thiện vòng lặp gameplay cốt lõi.

## 1. Tích hợp Hệ thống Âm thanh Cơ bản (Audio System)
**Mục tiêu:** Thêm phản hồi âm thanh (audio feedback) cho các hành động cơ bản của game để trò chơi trở nên sinh động hơn.
**Chi tiết triển khai:**
- **Khởi tạo & Hủy (Core):** Thêm hàm `InitAudioDevice()` vào quá trình khởi tạo của `GameManager` và `CloseAudioDevice()` khi kết thúc game để Raylib quản lý thiết bị âm thanh.
- **Quản lý Asset:** Khai báo và load một số file âm thanh cơ bản như: tiếng bắn súng (`shoot.wav`), tiếng nổ (`explosion.wav`), tiếng nhặt đồ (`pickup.wav`), và nhạc nền.
- **Thực thi:** 
  - Gọi hàm `PlaySound()` phát tiếng bắn khi `Spaceship` tạo ra đạn mới.
  - Gọi hàm `PlaySound()` phát tiếng nổ khi `Enemy` bị triệt tiêu máu (HP <= 0).
  - Cập nhật luồng nhạc nền `UpdateMusicStream()` trong vòng lặp chính.

## 2. Logic Thu thập Vật phẩm & Thanh Kinh Nghiệm (Item Collection & EXP UI)
**Mục tiêu:** Hoàn thiện cơ chế vật phẩm (ở Plan 05 đã làm hiệu ứng đùi gà rơi) bằng cách cho phép phi thuyền tương tác, thu thập và ghi nhận thành quả.
**Chi tiết triển khai:**
- **Xử lý Va chạm (Collision):** Trong vòng lặp `Update` của `GameManager`, bổ sung hàm kiểm tra va chạm (`CheckCollisionRecs`) giữa `Spaceship` hitbox và danh sách các `Item` (đùi gà) đang rơi.
- **Chỉ số Phi thuyền:** Bổ sung các biến `exp` và `maxExp` vào lớp `Spaceship`. Khi phát hiện va chạm với vật phẩm, xóa vật phẩm đó khỏi màn hình, tăng `exp` cho phi thuyền và phát âm thanh `pickup.wav`.
- **Giao diện (UI):** Sử dụng `DrawRectangle()` để vẽ thêm một thanh Kinh nghiệm (EXP Bar) nhỏ ngay bên dưới thanh Máu (HP Bar) của phi thuyền. Chiều dài của thanh tỉ lệ thuận với `exp / maxExp`.

## 3. Thêm Chiến lược Di chuyển Mới cho Quái (Zigzag Movement)
**Mục tiêu:** Tận dụng triệt để `Strategy Pattern` (`IMovementBehavior`) để làm phong phú thêm quỹ đạo bay của quái vật, tạo độ khó cơ bản cho các đợt Wave tiếp theo.
**Chi tiết triển khai:**
- **Class mới:** Tạo class `ZigzagMovement` kế thừa từ interface `IMovementBehavior`.
- **Thuật toán Di chuyển:** 
  - Trục Y đi xuống với vận tốc không đổi (`y += speed * deltaTime`).
  - Trục X dao động qua lại quanh một trục cố định bằng hàm lượng giác (`x = startX + sin(timer * frequency) * amplitude`).
- **Áp dụng:** Cài đặt thử nghiệm cho một loại gà hoặc một batch gà nhỏ để quan sát hệ thống chuyển động hoạt động trên thực tế.
