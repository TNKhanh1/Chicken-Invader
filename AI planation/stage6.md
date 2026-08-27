# Thiết Kế Chi Tiết: Stage 6 - Chế Độ Boss Rush

Stage 6 là một chế độ thử thách đặc biệt nơi người chơi sẽ không gặp đám quái vật thông thường nữa, mà thay vào đó là một cuộc diễu hành (Boss Rush) của toàn bộ các con Boss từng xuất hiện từ Stage 1 đến Stage 5.

Đây là màn đánh giá toàn diện sức mạnh của người chơi sau khi thu thập đủ đạn và nâng cấp.

## 1. Cấu Trúc Wave (5 Waves)

Sử dụng lợi thế kiến trúc Data-Driven của Game, Stage 6 được định nghĩa hoàn toàn thông qua file `data/stage6.json` gồm 5 Wave. Mỗi Wave tương ứng với một Boss. 

Do bạn muốn dễ dàng test game, các chỉ số HP của Boss được giữ nguyên ở mức độ gốc của chúng ở các Stage trước, không bị làm cho quá trâu:

1. **Wave 1:** Fire Phoenix Boss (Lửa Hồi Sinh - Stage 1)
   - Visual ID: 10
   - HP: 5000 
   - Đòn đánh: Đạn lửa và Fire Rain.

2. **Wave 2:** Eggsecutioner Boss (Song Sát - Stage 2)
   - Visual ID: 11
   - HP: 4000
   - Khác với Stage 2 có tận 2 Boss xuất hiện cùng lúc, ở Stage 6 này nhằm giữ nhịp độ và không gây nhiễu, tôi chỉ thả **1 Boss** cho người chơi dễ test. Đòn đánh: Đạn Nova đỏ và Bouncing Red Bullets.

3. **Wave 3:** Eskimo Fat Chicken (Cơn Bão Tuyết - Stage 4)
   - Visual ID: 8
   - HP: 10000
   - Đòn đánh: Nở bông tuyết (Snowball) và chùm đạn xanh (Blue burst).

4. **Wave 4:** Bomber Chicken (Kẻ Hủy Diệt - Stage 5)
   - Visual ID: 7
   - HP: 12000
   - Đòn đánh: Rải bom đạn dày đặc góc rộng (3-way, 7-way).

5. **Wave 5:** Void Chicken (Trùm Hư Không - Stage 5)
   - Visual ID: 14
   - HP: 15000
   - Đòn đánh: Bắn Laser chết người và xả đạn tam giác tự dẫn đường (Homing) đã được tinh chỉnh thời gian sống 12 giây.

## 2. Xử Lý Logic C++ 

Mặc dù `stage6.json` lo toàn bộ cấu hình Enemy, chúng ta vẫn cần cập nhật C++ ở file `GameManager.cpp` để kích hoạt tính năng **Thưởng Sức Mạnh (Stat Selection & Core)** phù hợp với nhịp độ Boss Rush.

### A. Chọn Lõi (Argument/Core) Ở Mọi Wave
Trong hàm `GameManager::EnterStatSelection(int nextWave)`:
```cpp
    if (currentStage == 6) {
        pendingArgumentAfterStat = true; // Stage 6 always gives a core
    } else {
        pendingArgumentAfterStat = (nextWave == 5 || nextWave == 10 || nextWave == 15);
    }
```
Thông thường game chỉ cho lõi (Argument) ở các Wave 5, 10, 15. Tuy nhiên, vì Stage 6 là Boss Rush, **ngay sau mỗi Wave (mỗi trận Boss)**, người chơi đều sẽ được tặng 3 gói Chỉ số và **1 Lõi** nhằm đối phó với con Boss mạnh hơn tiếp theo.

### B. Chọn Sức Mạnh Ngay Đầu Trận
Vì Boss xuất hiện ngay từ Wave 1, người chơi sẽ rơi vào thế bị động nếu vào trận với sức mạnh khởi điểm. 
Do đó, khi màn hình `WAVE_SELECTION` (Menu Chọn Màn Test) bấm "Start Test" vào Stage 6 Wave 1, C++ sẽ kích hoạt ngay thẻ chọn bài trước khi đánh:
```cpp
    if (currentStage == 6 && currentWave == 1) {
        EnterStatSelection(1);
    } else {
        ChangeState(GameState::TEST_GAMEPLAY);
    }
```
Và trong vòng lặp chuyển Stage bình thường:
```cpp
    WaveManager::GetInstance()->LoadStage("data/stage" + std::to_string(currentStage) + ".json");
    if (currentStage == 6) {
        EnterStatSelection(1);
    } else {
        StartWave(1);
    }
```
Điều này đảm bảo người chơi có sẵn 1 Lõi và Nâng cấp trước khi chạm trán Fire Phoenix.

## 3. Quá Trình Testing

Stage 6 Boss Rush đã hoàn thiện 100%. Bạn có thể vào Test Menu -> Chọn STAGE 6 WAVE 1 và bấm START.
- Ngay lập tức, thẻ bài chọn Chỉ Số và Lõi sẽ hiện ra.
- Sau khi chọn xong, Boss 1 (Fire Phoenix) sẽ giáng trần.
- Tiêu diệt Boss 1 -> Tặng tiếp Thẻ Bài & Lõi -> Boss 2 (Eggsecutioner) xuất hiện.
- Trải nghiệm cảm giác nhịp độ cao liên tục một cách trơn tru!
