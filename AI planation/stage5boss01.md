# Kế Hoạch Boss Phụ Stage 5: Bomber Chicken (Gà Oanh Tạc 07) — Wave 10

## 1. Mục Tiêu

Thêm **Mini-Boss Gà Oanh Tạc (chicken07)** vào Stage 5, Wave 10.
- Hình ảnh `chicken07.png`.
- **Có 1 Phase (1 mạng)**, HP tạm thời để thấp (`1500 HP`) để dễ test.
- Không có kỹ năng đặc biệt rườm rà (ví dụ Bom chùm). Chỉ tập trung vào việc luân phiên các hình thái **Đánh thường đa dạng (Bullet Hell)**.

## 2. Giải Pháp Xử Lý Animation (Warping Bounding Box)

- **Nhận diện:** Dùng kênh Alpha dò mép cánh trái, mép cánh phải, và vùng thân của `chicken07.png`.
- **Thiết lập Pivot:** Do cánh của Gà 07 trải rất dài ngang hai bên, `left_pivot` và `right_pivot` sẽ được đặt sát vào vùng ngực.
- **Thuật toán Warp:** Giữ cố định vùng thân giữa. Phần bên ngoài 2 pivot sẽ bị uốn cong (warp) theo sóng Sine, đẩy lên/xuống.
- File đầu ra: `chicken07_anim.png`.

## 3. Thiết Kế Cơ Chế Tấn Công (Đa Dạng Hóa Đánh Thường)

Vì là Mini-boss, Boss này lơ lửng tĩnh (rất ít di chuyển) và xả đạn liên tục theo chu kỳ:

**Chu Kỳ Tấn Công:** Luân phiên 3 kiểu bắn đạn đỏ:
1. **Kiểu 1 (V-Shape Liên Hoàn):** Bắn 3 đợt đạn, mỗi đợt 2 viên đạn V-Shape (Góc 75, 105). Mỗi đợt cách nhau 0.3s.
2. **Kiểu 2 (3-Way Trực Diện):** Bắn 1 đợt 3 viên đạn (Góc 60, 90, 120).
3. **Kiểu 3 (7-Way Rẽ Quạt):** Bắn 1 đợt 7 viên đạn tỏa rộng bao trùm màn hình (Góc: 45, 60, 75, 90, 105, 120, 135). Rất khó né.

- Sau mỗi kiểu bắn, Boss nghỉ khoảng 1.5 - 2 giây.

## 4. Cấu Trúc Lớp (OOP)

- **Thêm Class `BomberBoss : public Boss`**:
  - Chứa tracking variables: `attackTimer`, `attackType` (luân phiên 0, 1, 2), `subAttackTimer`, `subAttackCount`.
  - Có các hàm: `FireVShape()`, `Fire3Way()`, `Fire7Way()`.
- **Cập nhật Factory & Data**:
  - `EnemyFactory.cpp`: `if (role == BOSS && visualId == 7)` tạo `BomberBoss`.
  - `data/stage5.json`: Thêm Wave 10, Batch 1.
  - `GameManager.cpp`: Cho phép test wave 10 của stage 5.
