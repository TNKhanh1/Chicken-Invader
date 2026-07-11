# Kế Hoạch Triển Khai & Kiến Trúc OOP Cho Game Chicken Invaders

Tài liệu này tổng hợp cấu trúc cốt lõi, lộ trình thực hiện và các Design Pattern được lựa chọn để xây dựng dự án game Chicken Invaders. Việc áp dụng các nguyên lý này giúp mã nguồn dễ bảo trì, dễ mở rộng (thêm súng, thêm quái, thêm tính năng) và tuân thủ nghiêm ngặt tư duy lập trình hướng đối tượng (OOP).

---

## 1. Hệ Thống Design Pattern

**1. Singleton Pattern**
* **Áp dụng cho:** `GameManager`
* **Mục đích:** Đảm bảo toàn bộ vòng đời của trò chơi (khởi tạo đồ họa Raylib, xử lý sự kiện hệ thống, vòng lặp game chính) chỉ được quản lý bởi một thực thể duy nhất. Giúp các class khác truy cập dễ dàng mà không cần truyền tham chiếu/con trỏ phức tạp.

**2. State Pattern**
* **Áp dụng cho:** Hệ thống quản lý màn hình (`MainMenu`, `Playing`, `Paused`, `GameOver`, `Shop`, `Encyclopedia`).
* **Mục đích:** Phân tách logic của từng màn hình thành các class riêng biệt. Giúp việc chuyển đổi giữa sảnh chờ, cửa hàng và trong trận đấu mượt mà, tránh tình trạng dùng quá nhiều lệnh `if/else` hoặc `switch/case` khổng lồ trong vòng lặp chính.

**3. Strategy Pattern**
* **Áp dụng cho:** Cơ chế Vũ khí (`ShootingBehavior`) và Di chuyển (`MovementBehavior`).
* **Mục đích:** Tách biệt logic loại đạn và cách bắn ra khỏi class `Spaceship` cốt lõi. Khi đổi 1 trong 3 loại súng đầu phase, hệ thống chỉ cần thay đổi Strategy hiện tại mà không phải viết lại code quản lý phi thuyền.

**4. Factory Method Pattern**
* **Áp dụng cho:** `SpaceshipFactory`, `EnemyFactory`.
* **Mục đích:** Khởi tạo đối tượng linh hoạt dựa trên chuỗi (string) hoặc enum thay vì dùng `new` thủ công khắp mọi nơi.
  * `EnemyFactory`: Sinh ra các loại gà khác nhau tùy theo Phase (gà máu trâu, gà nhanh, gà xuất hiện số lượng lớn).
  * `SpaceshipFactory`: Khởi tạo phi thuyền với chỉ số đặc thù riêng biệt theo cấu hình người chơi chọn.

**5. Decorator Pattern**
* **Áp dụng cho:** Hệ thống Lõi Nâng Cấp (`Upgrades`) và Trang Bị (`Equipment`).
* **Mục đích:** Cho phép "bọc" thêm các hiệu ứng và chỉ số phụ vào phi thuyền một cách linh động.
  * Cộng dồn hiệu ứng đặc biệt (bắn thiêu đốt, giật điện, sát thương theo % máu) khi chọn lõi ở round 5, 10, 15.
  * Cộng dồn chỉ số từ ngọc/đá trang bị ngoài trận (+10 damage, +5% crit chance).

**6. Observer Pattern**
* **Áp dụng cho:** Hệ thống Giao diện (UI) và Quản lý Vật phẩm.
* **Mục đích:** Tách rời Dữ liệu (Model) khỏi Hiển thị (View). Khi phi thuyền nhặt đùi gà, nhận sát thương, hoặc đầy mana, nó tự động "phát" sự kiện. Các thanh UI (Máu, Mana, EXP) lắng nghe sự kiện này và tự động vẽ lại mà không cần kiểm tra liên tục mỗi khung hình.

**7. Component / Base Class (Kiến trúc kế thừa)**
* **Áp dụng cho:** Cây phân cấp thực thể (`GameObject`, `Character`).
* **Mục đích:** Định hình bộ khung chuẩn. `Spaceship` và `Enemy` đều kế thừa từ `Character` để dùng chung các chỉ số nền tảng (Máu, Damage, Tốc chạy) và chung logic tính giảm sát thương bằng công thức $f(X,Y)$ dựa vào giáp.

---

## 2. Kế Hoạch Triển Khai Chi Tiết (Roadmap)

| Giai đoạn | Mục tiêu chính | Chi tiết thực hiện |
| :--- | :--- | :--- |
| **1. Core & Window** | Thiết lập nền tảng | Viết lại `Makefile` để hỗ trợ biên dịch hàng loạt file `.cpp`. Khởi tạo `GameManager` (Singleton) và vòng lặp Raylib cơ bản ở độ phân giải 1280x720. |
| **2. Base Entities** | Cấu trúc đối tượng | Xây dựng class `GameObject` và `Character` chứa các chỉ số cơ bản. Viết hàm xử lý công thức giảm sát thương qua giáp. |
| **3. Player System** | Hoàn thiện Phi thuyền | Tạo `Spaceship` và áp dụng `Strategy Pattern` cho điều khiển WASD/Chuột. Cài đặt cơ chế tích mana để dùng chiêu thức đặc biệt. |
| **4. Enemy & Spawner** | Gà & Vật phẩm | Dùng `EnemyFactory` để sinh quái theo Phase. Code logic va chạm gây mất máu. Cài đặt tỷ lệ rơi vật phẩm (đùi gà, bom, tim). |
| **5. Meta-Game** | Vòng lặp nâng cấp | Cấu hình 15 round/phase. Áp dụng `Decorator Pattern` cho phép chọn lõi sức mạnh ở các mốc round 5, 10, 15. |
| **6. Giao diện (UI)** | Hiển thị thông tin | Tích hợp `Observer Pattern` để vẽ thanh UI. Bổ sung tính năng tùy chỉnh nút bấm vào Setting. |
| **7. Out-game** | Cửa hàng & Từ điển | Xây dựng Main Menu. Code logic mua đồ bằng vàng. Hoàn thiện hệ thống Sách thông tin gà và phi thuyền. |

---

## 3. Cấu Trúc File & Mã Nguồn Đề Xuất

Áp dụng chặt chẽ nguyên lý Single Responsibility (Mỗi file đảm nhiệm một chức năng riêng biệt).

### Khung chương trình chính
* **`Makefile`**: Cập nhật rules để tự động nhận diện và biên dịch toàn bộ các file `*.cpp` trong thư mục `src/`.
* **`src/main.cpp`**: Điểm bắt đầu của chương trình, gọi `GameManager::GetInstance()->Run()`.
* **`include/GameManager.h`**: Lớp Singleton chứa vòng lặp `Init()`, `Update()`, `Draw()`.
* **`include/GameState.h`**: Định nghĩa các trạng thái màn hình.

### Thực thể cơ sở
* **`include/GameObject.h`**: Lớp thuần ảo cơ bản nhất (chứa tọa độ Vector2, Texture, Hitbox).
* **`include/Character.h`**: Kế thừa từ `GameObject`, chứa các chỉ số chung của mọi sinh vật (HP, Damage, Speed, Armor).

### Hệ thống Phi thuyền
* **`include/Spaceship.h`**: Chứa các chỉ số cụ thể của người chơi (Mana, Crit, Level, Exp).
* **`include/WeaponStrategy.h`**: Giao diện chung cho các loại súng (Strategy Pattern).
* **`include/SpaceshipFactory.h`**: Chịu trách nhiệm khởi tạo các loại phi thuyền có sẵn.

### Hệ thống Quái vật & Vật phẩm
* **`include/Enemy.h`**: Kế thừa `Character`, bổ sung logic rơi vật phẩm (DropRate).
* **`include/EnemyFactory.h`**: Nơi quản lý việc sinh sản các bầy gà theo phase.
* **`include/Item.h`**: Lớp cho Đùi gà, Trái tim, Bom, Trứng.

### Hệ thống Meta & UI
* **`include/Upgrades.h`**: Triển khai các lõi sức mạnh (Decorator Pattern).
* **`include/UIManager.h`**: Quản lý việc vẽ Thanh Máu, Mana, EXP và Bảng chỉ số (Observer Pattern).
