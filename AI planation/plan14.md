# Plan 14: Chuẩn Hóa và Nạp Độc Lập Dữ Liệu Chỉ Số Phi Thuyền (Spaceship Stats Scaling from CSV)

1. **Thiếu Tự Động Load & Refresh Dữ Liệu Khi Khởi Tạo Game:**
   - Hiện tại, lệnh gọi `LoadCSV("assets/spaceship/spaceship.csv")` chỉ xảy ra 1 lần duy nhất bên trong `GameManager::Init()` tại thời điểm bắt đầu chạy chương trình `game.exe`.
   - Khi bắt đầu một lượt chơi mới, quay lại màn hình chính hay tiến vào các chế độ thử nghiệm (Sandbox Mode / `TEST_GAMEPLAY`), hệ thống không có chu trình đọc mới hoặc làm sạch cache từ CSV. Nếu tệp CSV thay đổi chỉ số cân bằng, game phải tắt toàn bộ mở lại mới nhận.

2. **Mất Đồng Bộ Chỉ Số Khi Thay Đổi Tên Tàu / Loại Vũ Khí (`SetWeapon` Fault):**
   - Khi khởi tạo phi thuyền, hàm `SpaceshipFactory::CreateSpaceship("Hypergun", 1, pos)` đọc dữ liệu cho tàu `Hypergun` Level 1 từ `SpaceshipDataManager`.
   - Tuy nhiên, khi người chơi (hoặc chế độ test F1) chuyển qua lại giữa các vũ khí/tàu bằng hàm `Spaceship::SetWeapon(const std::string& weaponName)` trong `src/Spaceship.cpp`:
     ```cpp
     void Spaceship::SetWeapon(const std::string& weaponName) {
         currentWeapon = weaponName;
         SetShootingBehavior(CreateWeaponBehavior(weaponName));
     }
     ```
   - *Vấn đề lõi:* Phương thức `SetWeapon` **không** gán lại thuộc tính `name` (vẫn bị kẹt là `"Hypergun"`) và **hoàn toàn bỏ qua việc gọi ngược lại `GetStats()` để gia cố chuỗi chỉ số phi thuyền**! 
   - *Hậu quả:* Ngay cả khi người chơi chuyển sang tàu `Neutron_Gun` (chính xác phải có HP gốc 1500, Damage 35, Speed 263 theo CSV) hay `Plasma_Rifle` (HP 900, Speed 356), tàu của người chơi vẫn mang toàn bộ bộ chỉ số cũ của `Hypergun` (HP 1000, Damage 15). Kể cả khi thăng cấp bằng `LevelUp()`, code vẫn tiếp tục dùng chuỗi tên cũ `name` (`"Hypergun"`) để lấy dữ liệu cấp phát mới!

---

## 3. Action Plan & Next Steps
Để khắc phục hoàn toàn lỗ hổng đồng bộ chỉ số và bảo đảm tính vững chắc (robustness) của hệ thống quản lý chỉ số phi thuyền theo nguyên tắc Data-Driven, chúng ta sẽ tiến hành refactor theo quy trình 3 bước:

### Bước 1: Bổ Sung Phương Thức Cập Nhật Chỉ Số Chuyên Dụng Trầm Lặng (Refactor `Spaceship.h` & `Spaceship.cpp`)
- **Định nghĩa hàm mới trong `Spaceship.h` & `Spaceship.cpp`:**
  - Thêm phương thức `void ReloadStatsFromCSV();` (hoặc `void UpdateStats(const std::string& shipName, int targetLevel);`) có trách nhiệm liên hệ trực tiếp với `SpaceshipDataManager::GetInstance()->GetStats(name, level)` để tái đồng bộ đồng loạt 8 thuộc tính cấu thành:
    - `maxHp` & `currentHp` (điều chỉnh tỷ lệ máu nếu đang sống hoặc gán đầy nếu mới init/level up).
    - `damage`, `armor`, `moveSpeed`.
    - `critChance`, `critDamage`, `maxMana` (`currentMana = maxMana`), và `attackSpeed`.
- **Sửa chữa logic hàm `Spaceship::SetWeapon(const std::string& weaponName)`:**
  - Gán đồng bộ `this->name = weaponName;` cùng với `this->currentWeapon = weaponName;`.
  - Kêu gọi ngay hàm `ReloadStatsFromCSV();` ở bên trong `SetWeapon` để ngay khi chuyển vũ khí/loại tàu, toàn bộ thuộc tính cơ sức mạnh chuyển hoá chính xác sang dòng tàu tương xứng theo file CSV!
- **Tối ưu hàm `Spaceship::LevelUp()`:**
  - Tái sử dụng lời gọi `ReloadStatsFromCSV();` trong `LevelUp()` để bảo đảm DRY (Don't Repeat Yourself).

### Bước 2: Bảo Đảm Làm Mới Dữ Liệu Từ File CSV Ở Vòng Đời Khởi Dựng (`SpaceshipFactory` & `GameManager`)
- **Cập nhật `SpaceshipFactory::CreateSpaceship()` trong `src/SpaceshipFactory.cpp`:**
  - Bổ sung lệnh tường minh `SpaceshipDataManager::GetInstance()->LoadCSV("assets/spaceship/spaceship.csv");` ngay đầu phương thức `CreateSpaceship()`, bảo đảm bất kể khi nào trò chơi khởi tạo hay bắt đầu một màn chơi mới (game startup / initialization), toàn bộ thông số đều được nạp lại và scale chuẩn hóa từ đĩa cứng theo bản lưu CSV cập nhật mới nhất!
- **Rà soát `src/GameManager.cpp`:**
  - Kiểm tra và xác minh mọi phân đoạn tái tạo người chơi (tại `StartGame()`, chuyển về `MAIN_MENU` để `RETRY`, hoặc kích hoạt `TEST_GAMEPLAY`) đều thông qua `SpaceshipFactory` hoặc kích hoạt thành phần khởi dựng chỉ số hợp lệ.

### Bước 3: Ghi Logs Nghiệm Thu Bắt Buộc Trong Chế Độ Sandbox
- Tích hợp một dòng ghi chú log nhạy bén (Debug Logger) vào trong phương thức cập nhật của tàu:
  ```cpp
  std::cout << "[STAT SCALING] Reloaded CSV Stats -> Ship: " << name << " | Level: " << level 
            << " | HP: " << maxHp << " | DMG: " << damage << " | AS: " << attackSpeed 
            << " | MoveSpd: " << moveSpeed << " | Crit: " << critChance << "%" << std::endl;
  ```
- Nhờ log này, dev và QC dễ dàng tra cứu ngay lập tức trên Terminal mà không cần lục tìm bộ nhớ RAM.

---

## 4. Acceptance Criteria
Tiêu chí nghiệm thu cho thấy tính năng scale chỉ số hoạt động chính xác tuyệt đối theo hợp đồng dữ liệu CSV:

1. **Nạp Động Khi Khởi Động / Re-Init Game:**
   - Mỗi lần một phiên chơi bắt đầu hoặc một phi thuyền được khởi dựng bằng `SpaceshipFactory`, file `assets/spaceship/spaceship.csv` tự động được nạp lại.
   - Bất kỳ thay đổi con số nào trong file CSV (ví dụ sửa HP, Sát thương, tốc độ bay của Level bất kỳ) lập tức có hiệu lực ở ván thi đấu mới mà không gây va chạm bộ nhớ hay gián đoạn gameplay.

2. **Chuyển Đổi Vũ Khí Chuẩn Lực Trong Game & Sandbox:**
   - Khi ở trong thế giới game (hoặc khi bật phím F1 trong Sandbox và dùng các phím từ số `1` đến số `8` để biến đổi phi thuyền):
     - Chuyển sang **Hypergun (Phím 1, Lv1)**: HP đạt chính xác `1000`, Damage `15`, Tốc độ bay `330`, Tốc độ bắn `5.0`.
     - Chuyển sang **Plasma_Rifle (Phím 2, Lv1)**: HP co lại `900`, Damage gia tăng lên `18`, Tốc độ bay nhanh nhạy `356`, Tốc độ bắn `6.0`.
     - Chuyển sang **Neutron_Gun (Phím 4, Lv1)**: HP cường hoành lên `1500`, Damage đầm chắc `35`, Tốc độ bay siêu đầm `263`, Tốc độ bắn chậm mà chắc `2.0`.
     - Chuyển sang **Utensil_Poker (Phím 8, Lv1)**: HP đạt `1300`, Damage `28`, Tốc độ bay `277`, Tốc độ bắn `1.0`.
   - Log Debug xuất hiện rành mạch trên Terminal xác thực con số được tải tương đương 100% với dữ liệu quy định trong CSV.

3. **Scale Tiến Hóa Thăng Cấp Độ (Level 1 $\rightarrow$ 11):**
   - Khi phi thuyền tích lũy đủ điểm kinh nghiệm hoặc nhấn phím test thăng cấp (`L`), toàn bộ 8 trường chỉ số đồng loạt gia tăng theo chính xác dãy số đã cấu hình ở dòng Level tương đương trong `spaceship.csv`, giữ nguyên sự trung thực của hệ thống chiến đấu.
