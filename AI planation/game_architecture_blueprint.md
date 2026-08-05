# Bản Thiết Kế Kiến Trúc Tổng Thể: Hệ Thống Kẻ Địch và Các Đợt Chơi (Dài Hạn)

## 1. Lưu Trữ Dữ Liệu (Data-Driven Design) - Dùng CSV
Việc sử dụng file CSV (như cách chúng ta đã làm với phi thuyền qua `spaceship_data.csv`) là một phương án **TUYỆT VỜI** và rất phổ biến trong lập trình game chuyên nghiệp. Việc này giúp tách bạch hoàn toàn phần Data (cân bằng game) ra khỏi phần Code. Bạn chỉ cần sửa Excel, thêm 100 loại gà hoặc 1000 wave mà không cần phải mở C++ ra compile lại.

### a. Bảng dữ liệu Quái vật (`enemy_data.csv`)
Mỗi loại gà sẽ là 1 dòng trong file CSV. Các cột dự kiến:
- `EnemyID`: Mã quái (Ví dụ: `CHICKEN_MINION`, `CHICKEN_TANK`, `CHICKEN_BOSS_1`).
- `TextureIndex`: Xác định dùng file png nào (VD: `1` cho `chicken01.png`, `2` cho `chicken02.png`).
- `BaseScale`: Tỷ lệ kích thước gốc (1.0 = 100px, 1.5 = 150px).
- `BaseHP`, `BaseDamage`, `Armor`, `Speed`: Các chỉ số gốc.
- `Abilities`: Các kỹ năng nội tại/đặc biệt, ngăn cách bằng dấu `|` (VD: `SHIELDED|SPLIT_ON_DEATH`).

### b. Bảng dữ liệu Đợt chơi (`wave_data.json` hoặc `.csv`)
*Lưu ý: Vì cấu trúc của một đợt chơi có thể phức tạp (trộn nhiều loại quái, thiên thạch, sự kiện), dùng **JSON** sẽ linh hoạt hơn CSV ở mảng này.*
- `BattleID`: Xác định trận đấu (VD: `CAMPAIGN_01`, `KILL_100_MODE`, `BLIND_MATCH`).
- `WaveData`: Danh sách các Wave.
  - Trong mỗi Wave lại có danh sách các `Batch` (Tốp lính).
  - Một `Batch` có thể chứa NHIỀU loại quái cùng lúc (VD: 5 Gà Nhỏ + 2 Thiên Thạch), xếp theo các Formation khác nhau.
- `SpecialEvents`: Các cờ (flag) đặc biệt (Ví dụ: `"isBlind": true`, `"bossSpawn": true`).

---

## Giải đáp thắc mắc: "Nếu có ý tưởng dị, chế độ đặc biệt thì CSV/JSON làm sao đáp ứng?"

Bạn hoàn toàn đúng khi lo lắng rằng dữ liệu file (như CSV) thì cứng nhắc. Chìa khóa ở đây là: **File dữ liệu (CSV/JSON) CHỈ LÀ THÔNG SỐ, còn LUẬT CHƠI (Logic) nằm ở C++ (Interface IBattleMode).**

1. **Trộn nhiều loại gà, thiên thạch:** 
   Trong file dữ liệu, một đợt (Wave) không bị giới hạn chỉ 1 loại quái. Ta thiết kế để 1 đợt chứa một danh sách (List) các `Batch`. Tốp 1 là 5 thiên thạch, Tốp 2 là 10 Gà Tank cùng xuất hiện. Trình đọc dữ liệu sẽ lặp qua toàn bộ và sinh ra đủ các loại.

2. **Chế độ Blind (Mù) hoặc Môi trường đặc biệt:**
   File dữ liệu của màn đó sẽ truyền vào một tham số: `Environment = "BLIND"`. Khi `GameManager` đọc thấy tham số này, nó sẽ kích hoạt `BlindBattleMode` (chứa code vẽ màn đêm bằng Raylib). Nếu sau này bạn nảy ra ý tưởng môi trường "Gió thổi dạt phi thuyền", bạn chỉ cần code thêm class `WindyBattleMode` trong C++ và để CSV gọi tên nó. File dữ liệu KHÔNG chứa code gió, nó chỉ BẢO hệ thống C++ hãy bật tính năng gió lên.

3. **Chế độ không có Wave, tiêu diệt 100 quái là thắng:**
   Đúng vậy, chế độ này không chạy theo luồng Wave thông thường. Lúc này bạn sẽ tạo một class C++ tên là `KillCountBattleMode` kế thừa `IBattleMode`.
   - `KillCountBattleMode` sẽ phớt lờ cơ chế chờ hết quái mới ra Wave tiếp theo.
   - Nó sẽ có biến đếm `killedCount`.
   - Cứ mỗi vài giây nó lại gọi ngẫu nhiên một Tốp lính từ Data. Đủ 100 kill thì qua màn.
   - Khi đó trong file Data của trận này, bạn chỉ cần ném vào danh sách "Các loại quái có thể xuất hiện", phần còn lại để C++ lo!

=> **Kết luận:** Nếu bạn nảy ra ý tưởng mới, bạn sẽ code luật chơi đó thành một Class C++ (áp dụng Strategy Pattern). Còn CSV/JSON chỉ cung cấp thông số (Máu bao nhiêu, số lượng 100 hay 200, thời gian 5s hay 10s) cho Class C++ đó chạy. Nó CỰC KỲ dễ mở rộng mà không làm hỏng các màn chơi cũ!

## 2. Kiến Trúc Lập Trình (OOP & Design Patterns)

Để game đáp ứng được hệ thống CSV đồ sộ trên, code C++ cần áp dụng các Design Pattern sau:

### a. Data Manager & Factory Pattern (Quản lý và Sinh Quái)
- **`EnemyDataManager` (Singleton):** Chỉ chạy 1 lần khi bật game. Nó đọc `enemy_data.csv` và lưu vào `std::map<std::string, EnemyProfile>`.
- **`EnemyFactory`:** Khi GameManager yêu cầu sinh quái `CHICKEN_TANK`, Factory sẽ tra cứu DataManager, copy các chỉ số, và trả về đối tượng `Enemy` tương ứng.

### b. Component Pattern (Cho Nội Tại / Chiêu Thức)
- Thay vì tạo ra hàng chục Class con (`TankChicken`, `PoisonChicken`), ta **chỉ dùng đúng 1 Class `Enemy`**.
- Lớp `Enemy` sẽ chứa một danh sách các "Mảnh ghép" nội tại: `std::vector<std::unique_ptr<IEnemyAbility>>`.
- Interface `IEnemyAbility` chứa: `Update()`, `OnTakeDamage()`, `OnDeath()`.
- **Ví dụ:** Nếu CSV ghi `SPLIT_ON_DEATH`, Factory sẽ nhét class `SplitAbility` vào con quái đó. Khi quái chết, `Enemy::Die()` sẽ tự động kích hoạt `SplitAbility::OnDeath()` (sinh ra 3 con gà nhỏ).

### c. Modifier Pattern (Scaling Kích thước và Độ Khó)
- Mọi chỉ số trong CSV là **Chỉ số gốc (Base Stats)**.
- Khi tạo màn chơi, ta truyền vào một `DifficultyContext` (Ví dụ: Wave 10 hệ số máu x2.5).
- Factory sẽ lấy: `BaseHP * DifficultyContext.HpMult` = HP Thực tế.

### d. Builder Pattern (Sinh Đội Hình Tọa Độ)
- Hàm `SpawnWaveBatch` hiện tại của bạn đang lặp `for` và tính tọa độ x, y thủ công rất mệt mỏi.
- Cần tạo lớp **`FormationBuilder`**. Nhiệm vụ:
  - Input: Nhận biến `Count` (số lượng) và kiểu `Formation` (`V_SHAPE`, `GRID`).
  - Output: Trả về một mảng tọa độ `std::vector<Vector2>`.
- Code sinh quái sẽ rất gọn: gọi Builder lấy tọa độ, sau đó gọi Factory sinh quái đặt vào tọa độ đó.

### e. Strategy / State Pattern (Cho Các Phương Thức Trận Đấu)
- Tạo Interface **`IBattleMode`** chịu trách nhiệm điều phối luật chơi, bao gồm:
  - `virtual void Update(float deltaTime) = 0;`
  - `virtual void DrawOverlay() = 0;` // Để vẽ các hiệu ứng riêng
- Các Class con:
  - `StandardBattleMode`: Chơi theo màn cơ bản, phải diệt sạch lính thì Wave mới xuất hiện Tốp (Batch) tiếp theo.
  - `EndlessBattleMode`: Bỏ qua việc lính còn hay chết, cứ đếm Timer đủ `SpawnDelay` là quái tràn ra màn hình liên tục.
  - `BlindBattleMode`: Trong hàm `DrawOverlay()`, vẽ một lớp Texture màu đen đục phủ kín màn hình, chỉ để lộ vòng tròn nhỏ quanh Phi Thuyền (sương mù chiến tranh).

---

## 3. Các Bước Triển Khai Thực Tế
*(Theo đúng yêu cầu, chúng ta chưa code vội, đây là thứ tự sẽ làm khi bắt đầu code)*

- **Giai đoạn 1:** Soạn thảo 2 file `enemy_data.csv` và `wave_data.csv`. Viết class `DataManager` đọc file này đưa vào RAM.
- **Giai đoạn 2:** Xây dựng `FormationBuilder` (chứa thuật toán xếp đội hình chữ V, lưới, tròn). Chỉnh sửa `GameManager` để load màn chơi từ file CSV thay vì `if-else` cứng.
- **Giai đoạn 3:** Áp dụng Component Pattern cho quái (viết interface `IEnemyAbility` và thử nghiệm 1 nội tại đầu tiên).
- **Giai đoạn 4:** Thiết kế `IBattleMode` và đưa logic `StandardMode`, `BlindMode` vào hoạt động.
