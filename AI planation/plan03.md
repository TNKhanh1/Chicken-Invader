# Phase 3 Implementation Plan (Basic Progression & UI)

Dựa trên tiến độ hiện tại (Phase 2 đã hoàn thành Minimal Core: di chuyển, bắn đạn, quái spawn và va chạm cơ bản), Phase 3 sẽ tập trung vào việc bổ sung các phản hồi hình ảnh cơ bản nhất để vòng lặp game hoàn thiện hơn.

Chúng ta sẽ thực hiện 2 task chính sau:

---

## 1. Hệ thống Điểm số cơ bản (Score System)
*   **Mục tiêu**: Ghi nhận thành tích của người chơi khi tiêu diệt quái vật.
*   **Chi tiết triển khai**:
    *   Thêm thuộc tính `score` vào `Spaceship` hoặc trực tiếp trong `GameManager`.
    *   Trong logic xử lý va chạm (Collision Detection), nếu viên đạn tiêu diệt được một `Enemy` (làm máu quái <= 0), cộng thêm một lượng điểm (VD: +10).
    *   Hiển thị điểm số ở góc trên bên phải màn hình bằng `DrawText`.

---

## 2. Giao diện Thanh máu (Health Bar)
*   **Mục tiêu**: Cải thiện UI, thay thế dòng chữ hiển thị máu đơn điệu bằng thanh trạng thái trực quan.
*   **Chi tiết triển khai**:
    *   Trong hàm `GameManager::Draw()`, thay vì chỉ dùng `DrawText` cho HP, tính toán tỷ lệ phần trăm máu: `ratio = (Spaceship->GetHp() / Spaceship->GetMaxHp())`.
    *   Sử dụng hàm `DrawRectangle()` của Raylib để vẽ 2 hình chữ nhật lồng nhau: 
        *   Hình nền (màu xám/đen) thể hiện thanh máu trống.
        *   Hình màu đỏ/xanh lá đè lên trên, có chiều dài tỷ lệ thuận với lượng máu còn lại.
    *   Đặt thanh máu ở góc trên bên trái màn hình.

---


