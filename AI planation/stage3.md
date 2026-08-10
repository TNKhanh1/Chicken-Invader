# Thiết Kế Chi Tiết: Stage 3 - Chế Độ Sinh Tồn (Kill Count Mode)

Trong Stage 3, thay vì dọn dẹp từng đợt (Wave/Batch) quái được thả ra cùng lúc như Stage 1 và 2, người chơi sẽ phải đối mặt với một cơn mưa quái vật rơi xuống liên tục. Hệ thống sẽ đếm số lượng quái bị tiêu diệt (Kills) và chỉ kết thúc Stage khi đạt đủ mốc Kills yêu cầu (200 Kills).

Dưới đây là toàn bộ cách thiết kế, công thức toán học và logic C++ được sử dụng cho Stage 3.

---

## 1. Cơ Chế Lõi: CONTINUOUS_STREAM (Thả Nhỏ Giọt)

Thay vì viết lại toàn bộ một GameMode mới, chúng ta tái sử dụng vòng lặp xử lý Batch hiện tại của C++ bằng cách định nghĩa một loại `layout` mới tên là `"CONTINUOUS_STREAM"`.

**Thông số truyền vào từ file `data/stage3.json`:**
- `target_kills`: Số lượng quái cần giết để qua màn (Ví dụ: 200).
- `spawn_interval`: Khoảng thời gian tính bằng giây giữa mỗi lần thả 1 con quái (Ví dụ: 0.6 giây).

**Logic C++ hoạt động (trong `WaveManager.cpp`):**
- Có một bộ đếm `spawnTimer`. Mỗi frame, `spawnTimer -= deltaTime`.
- Khi `spawnTimer <= 0`, sinh ra 1 con quái và reset `spawnTimer = spawnInterval`.
- Biến `spawnedCount` tăng lên 1 (để theo dõi số lượng đã thả).
- Khi viên đạn hoặc tia lade của người chơi kết liễu 1 con quái, `GameManager` sẽ gọi `WaveManager::GetInstance()->AddKill()`, biến `currentKills` tăng lên 1.
- Khi `currentKills >= targetKills`, cờ `isContinuousStream` tắt, và Batch kết thúc khi toàn bộ quái trên màn hình bị dọn dẹp.

**Tính năng "Bảo đảm Cố định Kills":** 
Nếu người chơi không bắn trúng quái và để chúng trôi tuột khỏi màn hình, C++ sẽ gỡ chúng khỏi danh sách `activeEnemies` nhưng **không cộng Kill**. Nghĩa là C++ sẽ tiếp tục thả thêm quái cho đến khi người chơi thực sự bắn nổ ĐỦ 200 con.

---

## 2. Hệ Thống Phân Làn (Lane-based Waterfall)

Để quái rơi xuống rải rác, đẹp mắt và không bị đè lên nhau, chúng ta sử dụng hệ thống "Phân làn đường".

**Toán học phân làn:**
- Chiều ngang màn hình: 800px.
- Ta chia màn hình thành `7` làn, mỗi làn rộng `100px`.
- Tâm X của các làn sẽ lần lượt là: `100, 200, 300, 400, 500, 600, 700`.

**Thuật toán rải quái (Anti-Overlap):**
C++ sử dụng một biến `lastSpawnLane` để nhớ làn vừa thả quái.
```cpp
int lane = GetRandomValue(0, laneCount - 1);
if (lane == lastSpawnLane) {
    lane = (lane + 1) % laneCount; // Tịnh tiến sang làn bên cạnh nếu trùng
}
lastSpawnLane = lane;
Vector2 startPos = { (float)(lane * laneWidth + laneWidth / 2), -100.0f };
```
Với thuật toán này, 2 con quái thả liên tiếp sẽ nằm ở 2 làn khác nhau, hoàn toàn loại bỏ tình trạng đè dính lên nhau.

**Mật độ quái trên màn hình:**
- Vận tốc rơi trung bình: 80 px/s. Chiều cao màn hình: 900 px.
- Thời gian để 1 con gà rơi khỏi màn hình: `900 / 80 = 11.25 giây`.
- Với `spawn_interval = 0.6s`, tổng số quái luôn có mặt trên màn hình = `11.25 / 0.6 = ~18 con quái`.
- 18 con rải đều trên 7 làn tạo ra một khung cảnh nhộn nhịp, trải dài khắp nửa trên màn hình nhưng không quá hỗn loạn.

---

## 3. Hệ Thống Cân Bằng (Fixed Power Phases)

Việc Scale (Tăng sức mạnh) quái theo Level của người chơi dễ tạo ra hiệu ứng "Rubber-banding" (Người chơi thọt -> quái cũng yếu). Thay vào đó, chúng ta dùng các Mốc (Phases) cố định, dựa vào việc: **Giết 1 con = Rớt 1 đùi = 10 EXP**. 200 con = 2000 EXP = Lên Max Level 10.

Cấu hình các giai đoạn này được lưu ngay trong `stage3.json` để dễ chỉnh sửa:
```json
"phases": [
    { "spawn_count_threshold": 0,   "hp_multiplier": 1.0, "speed_multiplier": 1.0 }, // Cấp 1-3
    { "spawn_count_threshold": 50,  "hp_multiplier": 1.4, "speed_multiplier": 1.1 }, // Cấp 4-5
    { "spawn_count_threshold": 100, "hp_multiplier": 1.8, "speed_multiplier": 1.2 }, // Cấp 6-7
    { "spawn_count_threshold": 150, "hp_multiplier": 2.5, "speed_multiplier": 1.3 }  // Cấp 8-10
]
```
Trong hàm `Update()`, C++ sẽ dò xem `spawnedCount` (Số lượng quái đã thả) đã vượt mốc nào, từ đó nhân Máu gốc và Tốc độ gốc với các chỉ số tương ứng. Rất công bằng và chuẩn xác.

---

## 4. Sự Kiện Hỗn Loạn Cuối Game (Mid-Game Chaos)

Để game bớt nhàm chán và tăng kịch tính, khi người chơi vượt mốc **100 Kills**, hai cơ chế ẩn (không thông báo) sẽ được kích hoạt:

### A. Tăng Tần Suất Sinh Quái (Spawn Rate Buff)
```cpp
if (currentKills >= 100) {
    spawnInterval = originalSpawnInterval * 0.75f;
}
```
- Việc giảm thời gian chờ đi 25% đồng nghĩa với việc Tần suất xuất hiện tăng lên ~33% (từ 0.6s/con xuống 0.45s/con).
- Số lượng quái rơi trên màn hình lúc này sẽ tăng từ 18 con lên khoảng 24 - 25 con.

### B. Mưa Thiên Thạch Tập Kích (Asteroid Ambush)
- Mỗi khi Kills tăng thêm một mốc 10 (110, 120, 130...), C++ sẽ kích hoạt lệnh thả 1 đến 2 Thiên thạch khổng lồ.
- **Tốc độ:** Thiên thạch ở Stage 3 được giảm tốc 20-30% so với Stage 1, 2 (Tốc độ từ 350-500 giảm xuống còn 250-350 px/s). Mục đích là để cân bằng với sự đông đúc của quái vật trên màn hình, giúp người chơi kịp thời quan sát.
- **Bám đuổi người chơi (Direct Targeting):**
  - Thiên thạch không rớt ở toạ độ ngẫu nhiên. Lúc sinh ra, nó lập tức bắt Toạ độ X của người chơi:
    ```cpp
    float startX = GameManager::GetInstance()->GetPlayer()->GetPosition().x;
    Vector2 startPos = { startX, -100.0f - i * 150.0f };
    ```
  - Điều này buộc người chơi bắt buộc phải di chuyển khỏi vị trí hiện tại thay vì đứng im cắm chuột xả đạn.

---

## 5. Cập Nhật UI & Xử Lý Lỗi

### A. Hiển Thị Kills
Ở màn hình chơi, một lệnh DrawText màu CAM được chèn vào góc trên bên phải (cạnh Điểm số) để hiển thị mốc sinh tồn:
`KILLS: 105 / 200`

### B. Lỗi Văng Menu (Premature Wave End)
**Vấn đề:** Khi bắt đầu Stage 3, Game tự động văng lại về TEST_MENU. Lần 2 vào lại thì bình thường.
**Phân tích nguyên nhân:** Ở frame đầu tiên của `CONTINUOUS_STREAM`, quái chưa kịp sinh ra (`activeEnemies.empty() == true`). Trong khi đó ở GameManager, cứ thấy mảng quái trống là hệ thống tưởng rằng Wave đã bị tiêu diệt hết và tiến hành qua màn/thoát.
**Giải pháp:** Bổ sung ngoại lệ vào điều kiện kiểm tra kết thúc Wave:
```cpp
} else if (activeEnemies.empty() && !WaveManager::GetInstance()->IsContinuousStream()) {
    // Chỉ kích hoạt qua màn nếu KHÔNG PHẢI ĐANG Ở CHẾ ĐỘ STREAM
}
```
Khắc phục 100% hiện tượng kết thúc nhầm.
