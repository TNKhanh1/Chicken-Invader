# Bản Thiết Kế Kiến Trúc Tổng Thể: Hệ Thống Kẻ Địch và Các Đợt Chơi (Dài Hạn)

## 1. Lưu Trữ Dữ Liệu (Data-Driven Design) bằng JSON

Việc sử dụng file JSON là một phương án **TUYỆT VỜI** và linh hoạt nhất để tách bạch hoàn toàn phần Data (cân bằng game) ra khỏi phần Code C++. Bạn có thể tùy ý mix 10 ngoại hình gà, với các kích thước và chỉ số máu khác nhau ở mỗi Wave mà không cần đụng vào code.

### a. Kiến trúc "Component-Override" (Phân lớp dữ liệu)
Thay vì tạo ra hàng chục Class con (`Chicken1Boss`, `Chicken5Tank`), một đối tượng Quái sẽ được lắp ghép từ 3 lớp độc lập khi sinh ra:

1. **Lớp Hình Ảnh (Visual Layer):** Chỉ số ID từ 1 đến 10 (`visual_id`). Xác định con gà sẽ dùng spritesheet nào (`chicken01.png` đến `chicken10.png`).
2. **Lớp Vai Trò (Role Layer):** (`NORMAL`, `SWARM`, `TANK`, `BOSS`). Lớp này quyết định kích thước render trên màn hình (Scale) và bán kính va chạm (Hitbox).
3. **Lớp Chỉ Số (Stat Layer):** (`HP, Speed, EggRate, Score`). Lớp này quyết định độ khó thực tế của con gà.

### b. Bảng dữ liệu Đợt chơi (`waves.json`)
Toàn bộ sức mạnh của kiến trúc nằm ở file này. Nó cho phép bạn **override (ghi đè)** chỉ số của bất kỳ ngoại hình gà nào tùy theo từng đợt chơi.

Ví dụ file `waves.json`:
```json
"waves": [
  {
    "wave_id": 1,
    "batches": [
      {
        "batch_id": 1,
        "visual_id": 1,             // Dùng ngoại hình gà 1
        "role": "NORMAL",           // Size bình thường
        "count": 15,
        "pattern": "HOVER_GRID",
        "stats": {                  // Chỉ số chính xác cho Wave 1
          "hp": 100,
          "speed": 125,
          "egg_rate": 3.0,
          "score": 10
        }
      }
    ]
  },
  {
    "wave_id": 10,
    "batches": [
      {
        "batch_id": 1,
        "visual_id": 1,             // Vẫn là ngoại hình gà 1
        "role": "BOSS",             // Nhưng bị bơm to thành Size Boss
        "count": 1,
        "pattern": "BOSS_MOVEMENT",
        "stats": {                  // Chỉ số khủng khiếp cho Wave 10
          "hp": 15000,
          "speed": 200,
          "egg_rate": 0.5,          
          "score": 5000
        }
      }
    ]
  }
]
```

---

## 2. Giải đáp thắc mắc: "Nếu có ý tưởng dị, chế độ đặc biệt thì làm sao đáp ứng?"

Chìa khóa ở đây là: **File dữ liệu (JSON) CHỈ LÀ THÔNG SỐ, còn LUẬT CHƠI (Logic) nằm ở C++ (Interface IBattleMode).**

1. **Trộn nhiều loại gà, thiên thạch:** 
   Trong JSON, mảng `batches` có thể chứa vô hạn phần tử. Tốp 1 là thiên thạch (visual_id: 11), Tốp 2 là Gà Tank (visual_id: 5). Trình đọc dữ liệu sẽ lặp qua toàn bộ và sinh ra đủ các loại cùng lúc.

2. **Chế độ Blind (Mù) hoặc Môi trường đặc biệt:**
   File dữ liệu của trận đấu đó sẽ truyền vào cờ: `"environment": "BLIND"`. Khi C++ đọc thấy, nó kích hoạt `BlindBattleMode` (chứa code vẽ màn đêm). File dữ liệu KHÔNG chứa code sương mù, nó chỉ BẢO C++ hãy bật tính năng đó lên.

3. **Chế độ không có Wave, tiêu diệt 100 quái là thắng:**
   Sử dụng class C++ `KillCountBattleMode` kế thừa `IBattleMode`. Chế độ này bỏ qua biến đếm Wave. Nó liên tục bốc random cấu hình quái từ mảng JSON và thả vào màn hình. Đủ 100 kill là tự động Victory.

---

## 3. Kiến Trúc Lập Trình OOP (C++)

Để đáp ứng hệ thống trên, Code C++ cần áp dụng các mẫu thiết kế:

### a. WaveManager & Factory Pattern (Đọc JSON và Sinh Quái)
- **`WaveManager` (Singleton):** Sử dụng thư viện `nlohmann/json.hpp` để nạp `waves.json` vào bộ nhớ. Nó sẽ tự động trích xuất các thông số của Batch hiện tại.
- **`EnemyFactory`:** Hàm `CreateEnemy` giờ đây sẽ nhận vào `visual_id`, `role`, và một struct `EnemyStats`. Factory không còn chứa lệnh `switch-case` hardcode chỉ số nữa, mà chỉ thuần túy copy dữ liệu từ JSON sang Object Enemy.

### b. FormationBuilder Pattern (Dịch Layout sang Tọa Độ)
- File JSON chỉ mô tả hình dáng (VD: `type: "V_SHAPE"`, `count: 15`). Làm sao để biến nó thành tọa độ X, Y chính xác trên màn hình?
- Lớp **`FormationBuilder`** sẽ nhận cục JSON `"layout"` này và tính toán ra một mảng `std::vector<Vector2>`. 
- Nó chứa các hàm toán học như `BuildGrid()`, `BuildVShape()`, giúp `WaveManager` biết chính xác phải đặt quái ở đâu mà không cần viết lại công thức toán.

### c. Component Pattern (Cho Nội Tại / Chiêu Thức)
- Giữ nguyên thiết kế **1 Class `Enemy` duy nhất**.
- Các hiệu ứng đặc biệt (chia nhỏ khi chết, hồi máu, tạo khiên) được code thành các class kế thừa `IEnemyAbility` (VD: `SplitAbility`).
- Nếu JSON có ghi `"abilities": ["SPLIT_ON_DEATH"]`, Factory sẽ cấy `SplitAbility` vào con quái đó.

### d. Strategy Pattern (Cho Các Phương Thức Trận Đấu)
- Tạo Interface **`IBattleMode`** chịu trách nhiệm điều phối luật chơi (`Update`, `DrawOverlay`).
- **`StandardBattleMode`:** Chơi màn cơ bản, diệt sạch quái mới qua Wave.
- **`EndlessBattleMode`:** Quái ra liên tục dựa theo chu kỳ thời gian (Timer).

---

## 4. Các Bước Triển Khai Thực Tế

- **Giai đoạn 1:** Tích hợp thư viện JSON C++ (`nlohmann/json`). Cập nhật `EnemyFactory` và `Enemy` class để chấp nhận struct `EnemyStats` và `visualId` một cách độc lập. Tự động nạp 10 Sprite sheet vào mảng.
- **Giai đoạn 2:** Tạo file `waves.json` và class `WaveManager`. Dịch thuật các logic Wave 1-4 (đang nằm cứng trong `GameManager.cpp`) sang file JSON để hệ thống bắt đầu chạy Data-Driven 100%.
- **Giai đoạn 3:** Xây dựng Component Pattern (`IEnemyAbility`) để gán chiêu thức đặc biệt cho các quái thông qua mảng `"abilities"` trong JSON.
- **Giai đoạn 4:** Thiết lập `IBattleMode` để làm nền tảng cho các chế độ chơi dị (Blind, Endless) trong tương lai.
