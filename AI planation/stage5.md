# Stage 5 - The Void Run

Stage 5 là một trong những màn khó khăn nhất, thử thách người chơi với số lượng kẻ địch đông đảo và các mẫu quái vật cấp độ cao.

## Danh sách Wave

### Wave 1 đến 10
*(Xem các bản thiết kế trước)*
- Wave 10: **Bomber Boss** (Boss Stage 5).

### Wave 11
- **Batch 1:** Đàn `chicken01` (NORMAL). Mảng GRID (3x4), rơi xuống mức `Y = 100` và bật nảy ngang (HORIZONTAL_BOUNCE, drift = 200). 
- **Batch 2:** Đàn `chicken02` (SWARM). Mảng V_SHAPE 2 lớp, rơi xuống mức `Y = 350` (nằm dưới Batch 1) và nảy ngang (drift = 250).

### Wave 12
- **Batch 1:** Đàn `chicken03` (TANK). INTERSECTING_V, bay chéo thành chữ X ở trung tâm (`Y = 100`).
- **Batch 2:** Đàn `chicken04` (NORMAL). SWEEP_TO_GRID, vòng từ ngoài vào trong và kết thúc dưới dạng Grid (`Y = 280`).

### Wave 13
- **Batch 1:** Đàn `chicken05` (NORMAL). GRID mỏng trải ngang (2x6), rơi xuống mức `Y = 80`. Bật nảy ngang nhẹ (drift = 150).
- **Batch 2:** Đàn `chicken06` (TANK). V_SHAPE 2 lớp làm tấm khiên phòng thủ bên dưới (`Y = 250`). Bật nảy ngang theo cụm (drift = 180).

### Wave 14
- **Batch 1:** Đàn `chicken01` (SWARM). SWEEP_TO_GRID chia 2 bên, ập vào rất nhanh và xếp lưới tại `Y = 100`.
- **Batch 2:** Đàn `chicken03` (TANK). INTERSECTING_V đan chéo bên dưới (`Y = 280`).

## Ghi chú Cân Bằng (Balancing)
- Hiện tại, chỉ số (HP, Damage) của Wave 11-14 đã được **làm yếu đi** (HP: 50 - 200) để phục vụ mục đích kiểm thử (testing). 
- Trong bản phát hành chính thức, cần cân bằng lại để phù hợp với sức mạnh của phi thuyền sau Boss Wave 10.
