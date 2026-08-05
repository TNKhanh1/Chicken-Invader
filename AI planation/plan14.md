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

### Bước 4: Bổ Sung Hiệu Ứng Nảy / Giật Mạnh & Siêu Nhanh Khi Bắn Súng Kể Cả Súng Laser (Enhanced Weapon Recoil & Juice VFX)
Để giao diện chiến đấu trở nên ấn tượng, uy lực và có lực (juice & feedback), bổ sung cơ chế phản lực giật lùi cho phi thuyền khi khai hỏa theo đúng chuẩn kiến trúc hướng đối tượng (OOP):
- **Bao Đóng Thuộc Tính & Phương Thức (OOP Encapsulation trong `Spaceship`):**
  - Thêm các biến trạng thái animation: `recoilOffset`, `recoilTimer`, `recoilDuration`, `maxRecoilDistance`.
  - Thêm phương thức công khai `virtual void TriggerRecoil();` và `virtual float GetRecoilOffset() const;`.
  - Kế thừa và chuyển tiếp trong `SpaceshipDecorator` (Decorator Pattern): Đảm bảo các lớp gia trì (như `DamageStoneDecorator`) khi gọi `TriggerRecoil()` hoặc `GetRecoilOffset()` đều chuyển tiếp chính xác đến đối tượng tàu lõi (`coreShip`).
- **Thuật Toán Animation Giật Mạnh Mẽ & Tốc Độ Siêu Nhanh Tỷ Lệ Theo Tốc Độ Bắn (Dynamic Attack Speed Scaling):**
  - Trong `Spaceship::TriggerRecoil()`, tính toán thời gian và biên độ giật dựa theo chỉ số Tốc độ bắn (`GetAttackSpeed()`) với cường độ giật mạnh và chớp nhoáng hơn:
    - **Thời gian giật (`recoilDuration`):** Rút gọn xuống siêu nhanh, tỷ lệ thuận với chu kỳ bắn ($t_{recoil} = \min(\frac{1}{\text{AttackSpeed}} \times 0.5, 0.12s)$). Súng có chu kỳ bắn càng nhanh (tốc đánh càng cao), thời gian animation diễn ra càng chớp nhoáng ($30ms - 100ms$), đảm bảo animation luôn kịp kết thúc trước viên đạn tiếp theo, tạo hiệu ứng rung gắt nhịp nhàng cực kỳ đắc lực!
    - **Biên độ giật gia tăng gia cố (`maxRecoilDistance`):** Tăng cường độ giật lên mức cực đại ($\max(12.0, \min(26.0, \frac{50}{\text{AttackSpeed}}))$). Ngay cả các loại súng Laser / Beam bắn siêu tốc cũng bị giật rung mạnh mẽ tối thiểu $12$ pixels, trong khi súng hạng nặng bắn chậm (như Neutron Gun) tung ra nhịp nảy giật cực kỳ uy lực lên tới $26$ pixels!
  - **Kích Hoạt Nảy Giật Cho Súng Laser / Beam (Trong `GameManager::Update`):**
    - Súng đạn tia (Beam/Laser như Absolver Beam, Lightning Fryer, Plasma Rifle) liên tục phát xạ cũng gọi trực tiếp `player->Fire()` (kèm kiểm tra `player->CanFire()`) khi giữ phím cách (`SPACE`), buộc con tàu phản ứng giật liên hồi từng chu kỳ bắn thay vì bám cố định bất động như trước.
- **Nâng Tầm Thẩm Mỹ Render (Squash & Stretch trong `Spaceship::Draw`):**
  - Cập nhật hàm `Spaceship::Draw()` trong `src/GameManager.cpp`: Thay vì chỉ dời vị trí vẽ xuống dưới (`position.y + GetRecoilOffset()`), bổ sung hiệu ứng co giãn đàn hồi vi mô (Squash & Stretch) lùi chiều cao ($-\Delta y$) và mở rộng chiều ngang ($+\Delta x$) lên tới $8\%$ ở cực đại giật nảy.
  - Vị trí Hitbox thật của tàu (`position`) không bị tác động bởi animation nảy giật để bảo toàn hoàn hảo độ chính xác khi người chơi né đạn!

### Bước 5: Tạo Đuôi Năng Lượng Tương Tác Động Gắn Sau Phi Thuyền (Dynamic Plasma Thruster & Energy Trail VFX)
Nhờ sự bùng nổ của động cơ phản lực và tải trọng bắn, thiết lập hệ thống đuôi tia plasma đằng sau tàu theo nguyên tắc OOP:
- **Bao Đóng Trạng Thái Đuôi Năng Lượng (OOP Encapsulation trong `Spaceship` & `SpaceshipDecorator`):**
  - Khai báo các thuộc tính protected trong `Spaceship`: `prevPosition` (tọa độ frame liền trước), `thrusterIntensity` (cường độ sáng động cơ, từ 0.3f Idle tới 2.5f Max), `thrusterTiltX` (độ nghiêng quán tính đuôi lửa), và `thrusterLengthMult` (hệ số co giãn chiều dài).
  - Expose các hàm getters: `GetThrusterIntensity()`, `GetThrusterTiltX()`, `GetThrusterLengthMult()`.
  - Nghiêm chỉnh kế thừa và ghi đè ủy quyền lại cho `coreShip` trong `SpaceshipDecorator` nhằm bảo toàn tuyệt đối Decorator Pattern.
- **Thuật Toán Gia Tăng Năng Lượng Theo Quãng Đường & Tốc Độ Bắn (Dynamic Intensity Scaling):**
  - **Phụ thuộc quãng đường / tốc độ bay:** Trong `Spaceship::Update(float deltaTime)`, đo khoảng cách dịch chuyển ($\Delta \vec{p} = \vec{p}_{curr} - \vec{p}_{prev}$). Khi tàu bay với tốc độ cao, cường độ sáng `thrusterIntensity` liên tục bơm gia tốc lên mức cao rực rỡ; khi tiến/lùi theo trục Y, lửa đuôi giãn ra hoặc thu ngắn thầm mỉa mai. Lực quán tính ngang ($\Delta x$) nghiêng ngã đuôi ngược hướng bay để bộc lộ độ đầm cơ học.
  - **Phụ thuộc tốc độ bắn (Attack Speed):** Khi tàu xả đạn (`TriggerRecoil`), năng lượng buồng súng quá tải được xả sang đường xả sau tàu: `thrusterIntensity` gia tăng tỷ lệ thuận với tốc độ bắn (`GetAttackSpeed()`). Bắn đạn laser hoặc spray siêu tốc khiến lửa động cơ sục tung sáng ngời sáng chói!
- **Nghệ Thuật Render Đa Trái Tim Cực Phim (3-Layer Plasma Plume & Embers trong `Spaceship::Draw`):**
  - Trước khi vẽ lớp phi thuyền, dựng 3 tầng Hào quang (Outer Blue Aura $\rightarrow$ Middle Neon Cyan $\rightarrow$ Inner White Incandescence Core) chao đảo sống động ngay tại miệng xả phía sau.
  - Khi cường độ cao ($> 0.8$), hệ thống rải lả tả các hạt plasma (sparks/embers) tan rã sa rây phía sau chớp mây.

---

## 4. Acceptance Criteria
Tiêu chí nghiệm thu cho thấy tính năng scale chỉ số và hiệu ứng hoạt động chính xác tuyệt đối theo hợp đồng dữ liệu CSV:

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

4. **Hiệu Ứng Nảy Giật Phi Thuyền Đẹp Mắt & Linh Hoạt (Recoil & Juice VFX):**
   - Khi phi thuyền khai hỏa (bao gồm cả súng đạn rời và các dòng súng tia Laser/Beam), hình ảnh súng/phi thuyền lập tức giật ngửa mạnh và nhanh gọn về phía sau kèm co giãn uyển chuyển rồi trở lại vị trí gốc chớp nhoáng.
   - Khi dùng súng đạn tia laser bắn liên tục hoặc các vũ khí tốc độ cao, tàu giật rung liên hồi một cách đầm chắc (tối thiểu $12$ pixels), tạo cảm giác súng nã ra nguồn năng lượng khổng lồ.
   - Không gây chệch Hitbox va chạm với quái hay đạn quái.

5. **Hiệu Ứng Đuôi Năng Lượng / Động Cơ Phản Lực Đẹp Mắt & Tuyệt Đối Động (Energy Thruster Trail):**
   - Phía sau chiêu đuôi phi thuyền liên tục bừng lên ngọn lửa plasma uyển chuyển nhiều lớp (Xanh hào quang $\rightarrow$ Cyan rực rỡ $\rightarrow$ Trắng buồng đốt chớp nháy).
   - Khi tàu di chuyển (quãng đường càng xa, vận tốc càng mau lẹ), ngọn lửa càng bừng sáng vút dài rực rỡ, kèm hiệu ứng bẻ xoay ngược chiều theo lực quán tính khi lái sang trái/phải.
   - Khi xả súng thuyên chuyển hoặc phun tia laser cường độ cao, năng lượng nổ tuôn tuôn thắp sáng luồng lửa động cơ đến đỉnh điểm và văng lả tả những tàn thiêu hạt plasma tản rã trong không gian, vô cùng mãn nhãn và sinh động.
