# Thiết Kế Kiến Trúc Hệ Thống Các Loại Đạn (Weapon & Projectile Architecture)

---

## 1. Kiến Trúc OOP & Thuật Toán Cốt Lõi

### 1.1. Kiến Trúc Hướng Đối Tượng (OOP & Design Patterns)
Để quản lý 8 loại vũ khí khác nhau với vô số quỹ đạo và tính chất cơ lý, hệ thống tách biệt hoàn toàn giữa **Vũ khí (Weapon/Shooting Behavior)**, **Thực thể Đạn (Bullet/Beam)** và **Thuật toán Quỹ đạo (Trajectory Strategy)**.

```
       [Spaceship] 
            │ 1..*
            ▼
     [IWeaponBehavior] ──(Sử dụng)──> [BulletPool] (Object Pool Pattern + Silent Drop)
            │                               │
    ┌───────┴───────┐                       ▼
[DiscreteWeapon] [BeamWeapon]          [Bullet] ──> [IBulletTrajectory] (Strategy Pattern)
                                                    ├── [StraightTrajectory]
                                                    ├── [OscillatingTrajectory]
                                                    └── [SpreadTrajectory]
```

#### A. Strategy Pattern Cho Quỹ Đạo Đạn (`IBulletTrajectory`)
- **Vấn đề:** Các loại đạn có quỹ đạo bay vô cùng đa dạng (bay thẳng của *Neutron Gun*, chùm xòe quạt của *Riddler / Ion Blaster*, hay góc dao động hình sin của *Hypergun / Utensil Poker*). Nhét toàn bộ công thức toán học vào class `Bullet` sẽ gây cồng kềnh, khó thử nghiệm và vi phạm nguyên lý SOLID.
- **Giải pháp - Strategy Pattern:**
  - Định nghĩa interface `IBulletTrajectory` với phương thức lõi `UpdatePosition(Vector2& pos, float& angle, float speed, float deltaTime)`.
  - Các chiến lược cụ thể được tiêm (inject) vào thực thể đạn khi bắn:
    - `StraightTrajectory`: Bay thẳng theo hướng góc ban đầu $\vec{V} = (\sin(\theta), -\cos(\theta)) \times \text{speed}$.
    - `OscillatingTrajectory`: Tự động thay đổi góc lệch theo thời gian hoặc chu kỳ theo danh sách mẫu (pattern array).
    - `SpreadTrajectory`: Tính toán vector văng mở rộng dần ra hai bên cánh.

#### B. Object Pool Pattern Cho Quản Lý Bộ Nhớ & Quy Tắc Xử Lý Tràn (`BulletPool` + Overflow Policy)
- **Vấn đề:** Ở các cấp độ cao hoặc chế độ Mana (như *Riddler* xả 13 viên/đợt ở tần suất dày đặc), việc liên tục gọi `new Bullet()` và `delete` khi đạn bay khỏi màn hình sẽ gây lãng phí chu kỳ CPU, phân mảnh bộ nhớ RAM (Memory Fragmentation) và làm tụt FPS nhịp nhàng. Đồng thời, nếu người chơi nhặt được buff "Bắn tốc độ ánh sáng", số lượng đạn trên màn hình có thể vượt mốc tải trước (ví dụ: cần bắn viên thứ 1001 khi cả 1000 viên trong bể đều đang bay). Nếu không xử lý tràn cẩn thận, hàm `Acquire()` có thể trả về con trỏ `nullptr` hoặc thao tác ngoài vùng nhớ gây văng game (Crash / Segfault).
- **Giải pháp - Object Pool kết hợp Cơ Chế Fallback (Silent Drop Policy):**
  - Cung cấp lớp quản lý hồ chứa tĩnh `BulletPool` tải trước (pre-allocate) một mảng liền kề gồm `1000` thực thể `Bullet` trong VRAM/RAM ngay khi bắt đầu game.
  - **Cơ chế tái sử dụng (Acquire/Release):**
    - Mỗi viên đạn duy trì cờ `bool active = false`.
    - **Hàm `Acquire(...)`:** Lấy thực thể `active == false` đầu tiên trong hồ, đặt lại tọa độ, sát thương, sprite, gắn `IBulletTrajectory` tương ứng và chuyển cờ thành `active = true`.
    - **Quy Tắc Xử Lý Tràn (Overflow Policy - Silent Drop):** Trong hàm `Acquire()`, nếu lặp duyệt hết toàn bộ 1,000 phần tử mà `active == true` toàn bộ (bể đã trần định mức max), hệ thống sẽ trả về `nullptr` và bên gọi (ShootingBehavior) sẽ **chủ động bỏ qua không sinh đạn mới (Silent Drop)**. Việc im lặng đánh rơi 1-2 viên đạn trong một "cơn bão đạn" 1,000 viên là hoàn toàn không thể nhận ra bằng mắt thường, nhưng là lớp khiên vững chắc bảo vệ game khỏi rủi ro Crash bộ nhớ hoặc tràn trễ RAM vĩnh viễn!
    - **Hàm `Release(Bullet* b)`:** Khi đạn trúng quái vật hoặc bay vượt biên màn hình ($Y < -50$), chỉ cần chuyển cờ về `active = false` để tái sử dụng ngay lập tức mà không thu hồi bộ nhớ vật lý.

---

## 2. Phân Tích Cơ Chế Bắn Từng Loại Vũ Khí (Dữ liệu từ spaceship.doc)

Hệ thống hỗ trợ cấu hình động logic sinh đạn qua các cấp độ từ Level 1 đến Level 10 và chế độ kích hoạt kỹ năng đặc biệt (Mana Mode).

| Tên Vũ Khí | Loại Hình | Cơ Chế Phát Xạ (Levels 1 - 10) | Chế Độ Đặc Biệt (Mana Mode) |
| :--- | :--- | :--- | :--- |
| **Hypergun** | Đạn rời | **Góc dao động chu kỳ 8 nhịp:** $0^\circ \rightarrow 1.875^\circ \rightarrow 3.75^\circ \rightarrow 1.875^\circ \rightarrow 0^\circ \rightarrow -1.875^\circ \rightarrow -3.75^\circ \rightarrow -1.875^\circ$.<br>• **Lv1-3:** Bắn đạn *Strong* từ nòng giữa (tip), *Weak* từ nòng trước (front pods).<br>• **Lv6-10:** Mở rộng ra các nòng sau (rear pods - tần suất chậm hơn 2 lần) và cánh (wings - chậm hơn 4 lần). | Bắn đồng loạt **7 đạn Strong** tốc lực cực đại từ tất cả các nòng (Tip, Front, Rear, Wings). |
| **Plasma Rifle** | Đạn tia/sóng | Bắn ra dòng sóng năng lượng gia tăng bán kính tác chiến.<br>• **Chiều rộng (Width Scaling):** Lv1 bắt đầu tại **8% width**, mỗi cấp tăng thêm khoảng 8% (Lv2: 17%, Lv6: 50%, Lv10: **83% width**). | Sóng năng lượng bành trướng đạt **100% width** tiêu chuẩn tối đa, chấn động dốc nòng. |
| **Absolver Beam** | Đạn tia (Laser) | Bắn 1 tia laser cô đặc từ trung tâm tàu. Sức mạnh tiến hóa qua 5 nấc hình thái (Starter, Weak, Medium, Strong, Full).<br>• **Tỷ lệ chiều rộng tia Full Beam:** Gia tăng thăng tiến từ **50% (Lv1)** lên đến **80% (Lv10)**. | Kích hoạt luồng la-ze tối thượng tại **100% width**, cắt phăng mọi vật cản trên trục bắn. |
| **Neutron Gun** | Đạn rời (Tốc siêu pháo) | Pháo tốc độ cao theo trục thẳng.<br>• **Lv1-3:** 1 đến 3 viên đạn *Weak* từ tip và front pods.<br>• **Lv4-8:** Nâng cấp dần lên đạn *Medium* và *Strong* từ nòng giữa và nòng sau.<br>• **Lv9-10:** Xả đồng thời 1 đạn Medium, 2 đạn Strong và 2 đạn Weak. | Bắn đồng loạt **5 viên pháo Strong** siêu năng lượng từ toàn bộ tổ hợp nòng chính. |
| **Riddler** | Đạn tán xạ (Xòe quạt) | Gia tăng số lượng viên đạn qua từng level, đồng thời **thu hồi góc xòe (ngắm hội tụ hơn):**<br>• **Lv1:** 1 viên đạn, góc xòe lên đến **13.5°** hai bên.<br>• **Lv5:** 5 viên đạn, góc xòe **11.1°**.<br>• **Lv10:** 10 viên đạn đồng loạt, góc xòe thu lại **8.1°** (tối ưu mật độ sát thương). | Bắn **13 viên đạn** xòe quạt tại góc hội tụ **7.5°**, đồng thời gia tốc tần suất xả đạn (Fire-rate boosted). |
| **Lightning Fryer** | Đạn tia (Luồng sét Tự nhắm) | Dòng điện liên tục **Tự Động Nhắm (Auto-locking)** và bẻ thẳng vào tâm quái vật gần nhất.<br>• **Lv1-6:** 1 dòng sét từ Tip, độ rộng cuộn dãn từ **33% lên 100%**.<br>• **Lv7-10:** Tách thành 2 dòng sét song song từ nòng đôi (Front pods), độ rộng tăng từ **47% lên 87%**, mỗi nòng có khả năng tự động khóa 1 mục tiêu quái độc lập. | Bắn đồng thời **3 chùm luồng sét đôi + giữa** (Tip + Front pods) bùng nổ ở lực cuộn **100% width**, tìm diệt 3 mục tiêu nhanh nhạy. |
| **Ion Blaster** | Đạn rời (Kép/Đơn) | • **Lv1-3:** Bắn đạn *Single* từ các nòng trước.<br>• **Lv4-6:** Các nòng hai bên chĩa ra phía ngoài góc **7.5°** và **15°**.<br>• **Lv7-10:** Tiến hóa lên đạn *Double* (công phá kép), xả bão lửa từ 5 nòng cùng lúc. | Bắn **7 viên đạn Double** chớp lốp: 5 viên xòe góc **30°** phía trước kết hợp thêm 2 đạn Double bổ trợ ở nòng sau. |
| **Utensil Poker** | Đạn rời (Dĩa/Dao phơ) | Phi vũ khí gia dụng thẳng lên cao với khuôn mẫu góc quay chu kỳ (thẳng $\rightarrow$ lệch phải $\rightarrow$ thẳng $\rightarrow$ lệch trái).<br>• **Lv1-4:** Bắn các mũi dĩa (*Fork*), độ lệch từ **1.875° đến 7.5°**.<br>• **Lv5-10:** Lên cấp dần từ Fork thường thành dao dĩa chọc phơ sắc nát (*Carving Fork*). | Phi phong ba **5 Carving Forks** công phá cực tím kèm theo **2 Forks** hỗ trợ phía sau. |

---

## 3. Xử Lý Tài Nguyên (Assets & Data-Driven Pipeline)

### 3.1. Kế Hoạch Chuyển Đổi Toàn Bộ Asset Sang Định Dạng `.PNG`
Hiện tại các tài nguyên vũ khí đang lưu trữ rải rác dưới chuẩn `.webp` tại thư mục `assets/spaceship/`. Để đảm bảo tương thích 100% với engine vẽ VRAM OpenGL gốc của Raylib mà không tiêu tốn tài nguyên giải mã ảnh ở runtime, áp dụng pipeline chuẩn sau:
- **Áp dụng công cụ chuyển đổi tự động (Python Pillow pipeline):**
  - **Tài nguyên tĩnh (Single Sprite):** Chuyển trực tiếp các file `AbsolverBeam*.webp`, `NeutronGun*.webp`, `UtensilPoker*.webp` sang `.png` hệ màu RGBA 32-bit (giữ trọn vẹn lớp alpha bóng mờ).
  - **Tài nguyên ảnh động (Animated Demos - e.g., `demoIonBlaster*.webp`, `demoLightningFryer.webp`):** Các ảnh này mang từ **15 đến 24 khung hình (frames)**. Quy trình sẽ mở trôi từng frame nhúng trong file `.webp` và may dệt ghép ngang theo chiều trục X thành một dải **Horizontal Spritesheet (.png)** liền khối (Ví dụ: 15 frames kích cỡ 300x300 sẽ xuất chu kỳ ra 1 ảnh atlas đơn 4500x300).
  - Trình vẽ `GameManager` chỉ nạp texture `.png` vào thẻ nhớ VRAM duy nhất một lần lúc Init() và cắt khung nhanh nhạy bằng `DrawTexturePro(..., sourceRect, ...)` với độ trễ $0\text{ ms}$.

### 3.2. Thiết Kế Bản Đồ JSON Dẫn Hướng (Data-Driven Schema & Circle Hitbox Optimization)
- **Tối Ưu Hitbox Cho Đạn Rời (Circle thay vì Rectangle):** Các loại đạn như *Riddler, Ion Blaster, Utensil Poker* bay xòe ra các góc chéo (ví dụ $13.5^\circ, 30^\circ$). Nếu sử dụng hình chữ nhật, bounding box sẽ bị xoay nghiêng. Việc tính toán va chạm giữa 2 hình chữ nhật xoay (OBB - Oriented Bounding Box) tiêu tốn cực kỳ nhiều nhịp CPU và phức tạp trong C++.
- **Giải Pháp Circle Hitbox:** Toàn bộ Đạn Rời được chuyển sang sử dụng **Circle Hitbox (Hình tròn)**. Thuật toán va chạm giữa hình tròn (đạn) và hình tròn/chữ nhật (quái vật) chỉ cần tính khoảng cách Euclid cực nhanh và nhẹ nhàng bằng công thức $d = \sqrt{(X_2 - X_1)^2 + (Y_2 - Y_1)^2} < R_1 + R_2$, triệt tiêu 90% hao tổn tính toán va chạm trên CPU!

```json
{
  "weapons": {
    "Riddler": {
      "type": "projectile",
      "texture_path": "assets/spaceship/riddler_sheet.png",
      "animation": {
        "frame_width": 32,
        "frame_height": 64,
        "total_frames": 4,
        "fps": 12.0
      },
      "hitbox": { "radius": 12.0, "offset_x": 0.0, "offset_y": 0.0 },
      "sound_effect": "assets/sounds/riddler_fire.wav",
      "levels": {
        "1": { "count": 1, "max_spread_angle": 13.5, "damage_multiplier": 1.0 },
        "10": { "count": 10, "max_spread_angle": 8.1, "damage_multiplier": 2.5 },
        "mana": { "count": 13, "max_spread_angle": 7.5, "fire_rate_bonus": 1.5, "damage_multiplier": 3.0 }
      }
    },
    "Lightning_Fryer": {
      "type": "beam",
      "auto_locking": true,
      "lock_radius": 650.0,
      "texture_path": "assets/spaceship/lightning_fryer_beam.png",
      "animation": {
        "frame_width": 64,
        "frame_height": 512,
        "total_frames": 15,
        "fps": 30.0,
        "uv_scroll_speed": 400.0
      },
      "hitbox": { "base_width": 48.0, "type": "oriented_ray_box" },
      "levels": {
        "1": { "pods": ["tip"], "width_percentage": 33, "target_count": 1 },
        "6": { "pods": ["tip"], "width_percentage": 100, "target_count": 1 },
        "10": { "pods": ["front_left", "front_right"], "width_percentage": 87, "target_count": 2 },
        "mana": { "pods": ["tip", "front_left", "front_right"], "width_percentage": 100, "target_count": 3 }
      }
    }
  }
}
```

---

## 4. Xử Lý Logic: Đạn Rời vs Đạn Tia (Discrete vs Continuous Beams)

Qua nghiên cứu trọn lọc kỹ lưỡng cấu trúc hình ảnh thực tế từ các file mẫu `demoIonBlaster1.webp`, `demoIonBlaster2.webp` và `demoLightningFryer.webp`, dưới đây là giải pháp thuật toán triệt để phân định ranh giới đồ họa và xử lý va chạm giữa **Đạn Rời (Discrete)** và **Đạn Tia (Continuous Beams)**:

### 4.1. Bản Chất Thuật Toán & Sự Khác Biệt Trừu Tượng

```
[ĐẠN RỜI - Ion Blaster]                         [ĐẠN TIA - Lightning Fryer / Absolver]
  (Sinh ra, rời nòng, bay độc lập)                 (Gắn chết vào nòng tàu, chiếu tia liên tục)
        
        ▲   ▲   ▲                                     █ █ █ (Hitbox Raycast thò dài hết đỉnh hoặc trúng thù)
        │   │   │ (Velocity Vector)                  █ █ █ <--- Auto-Locking Bending & UV Scrolling
      ( o ) ( o ) ( o ) <--- Circle Hitbox          █ █ █ 
         \    │    /                                  █ █ █
          [ Tàu ]                                    [ Tàu ]
```

### 4.2. Giải Pháp Thuật Toán Cho Đạn Rời (Discrete Projectiles)
- **Vòng đời & Tràn Bể:** Được sinh ra từ hồ `BulletPool`, bứt rời khỏi tàu bay lập tức, cập nhật theo gia tốc tuyến tính $X += V_x \cdot \Delta t$, $Y += V_y \cdot \Delta t$. Khi cường độ quá lớn vượt mốc 1000 thực thể, tuân thủ nguyên tắc `Silent Drop` để giữ mượt game.
- **Hitbox:** Khách quan là một **Circle Hitbox (`radius = 12.0`)** ngay trọng tâm viên đạn.
- **Render:** Vẽ frame tương ứng của đạn xoay nghiêng đúng góc bắn của nòng (`origin` xoay tại trọng tâm viên đạn).

### 4.3. Giải Pháp Thuật Toán Cho Đạn Tia (Beam / Laser Engine & Auto-Locking)
- **Vòng đời:** Không khởi tạo object trôi dạt! Mỗi tia laser/chùm sét là một **thực thể gắn chết liền nhịp với tọa độ Nòng Tàu ($Pod_{X}, Pod_{Y}$)**. Tuổi thọ của tia kéo dài liên tục chừng nào phím bắn được giữ.
- **Thuật Toán Tự Động Nhắm (Auto-locking - Đặc Sản Lightning Fryer):**
  - Trong cốt truyện gốc, điểm đặc trưng lớn nhất của *Lightning Fryer* là tia sét không bắn đơ cứng hướng thẳng lên đỉnh trời mà **tự động thám tử bẻ cong, bám riết vào quái vật gần nhất**.
  - **Quy trình thuật toán Auto-Locking:**
    1. Quét mảng `activeEnemies` trên màn hình trong phạm vi bán kính giới hạn (`lock_radius = 650.0f`).
    2. Tìm quái vật có khoảng cách Euclid nhỏ nhất đến tọa độ nòng súng ($Pod_{X}, Pod_{Y}$).
    3. Tính toán góc quay của tia hướng thẳng tới tâm mục tiêu:
       $$\theta = \text{atan2}(Y_{enemy} - Y_{pod}, X_{enemy} - X_{pod}) + \frac{\pi}{2}$$
    4. Cập nhật góc $\theta$ này cho tia sét, biến tia thẳng $Y=0$ thành luồng chớp ngoặt thẳng vào cơ thể kẻ địch!
    5. **Đa mục tiêu theo Level:** Ở các cấp độ cao (Lv7 -> Lv10, Mana Mode), mỗi nòng bắn (`tip`, `front_left`, `front_right`) sẽ tự động duy trì một bộ theo dõi độc lập để khóa trúng các quái vật khác nhau trên không phận (khóa tối đa 3 mục tiêu riêng rẽ cùng lúc)!
- **Hitbox & Phép Chiếu Va Chạm (Raycast Collision):**
  - Sử dụng thuật toán chiếu tia vuông góc **Line-Segment Raycast / Oriented Bounding Box (OBB)** dọc theo trục góc tự nhắm $\theta$.
  - Chiều cao tia đạn kéo dài đến vị trí tâm của quái vật bị khóa $P_{target}$. Khi trúng quái, tia dừng cuộn và tạo hiệu ứng nổ giật điện giật rực sáng tại vùng va chạm.
  - Chiều rộng hitbox của tia ($W_{hitbox}$) được co dãn theo tỉ lệ phần trăm quy ước tại level (`width_percentage`).
- **Thuật Toán Hiệu Ứng Cuộn & Co Dãn Động (Texture Scrolling & Scaling):**
  - Để tái hiện chân thực luồng sấm chớp cuồn cuộn mãnh liệt như quan sát trong `demoLightningFryer.webp`, áp dụng song song 3 hiệu ứng xử lý trực tiếp trên GPU thông qua thao tác cắt `sourceRect` và vẽ `destRect`:
  1. **Dynamic Width Scaling:** Biến đổi chiều rộng `destRect.width` của hình ảnh vẽ ra màn hình tỷ lệ thuận theo `% width` của level hiện tại (Lv1 mảnh dẻ 33%, Lv6 dày vĩ đại 100%).
  2. **UV Scrolling (Cuộn tọa độ nguồn):** Đặt cờ texture ở chế độ `RL_TEXTURE_WRAP_REPEAT`. Tại mỗi khung hình, tịnh tiến tọa độ Y của `sourceRect` đi một khoảng tốc độ cao: `sourceRect.y -= scrollSpeed * deltaTime`. Ngay lập tức, hình ảnh sấm sét/laser trên tia sẽ tạo cảm giác như dòng điện lan tràn chảy thét cuồn cuộn ngược lên chọc giặc bão táp.
  3. **High-speed Animation Indexing:** Xoay vòng nhịp nhàng qua 15 khung hình spritesheet gốc với tần suất 30 FPS (`currentFrame = (int)(elapsedTime * 30.0f) % 15`) tạo cường độ rung lắc quang phổ lấp lánh (bloom jitter) mà không làm suy yếu hiệu năng xử lý toán học.

---

## 5. Kịch Bản Kiểm Thử (Test Cases & Sandbox Walkthrough)

Dưới đây là hướng dẫn từng bước chi tiết để bạn có thể tự thiết lập môi trường cô lập, test kiểm định chính xác quỹ đạo, hitbox và hình dạng đạn của tất cả các cấp độ mà không bị ảnh hưởng bởi nhịp điệu sinh quái vật ngẫu nhiên trong game.

### 5.1. Bước 1: Thiết Lập Môi Trường "Weapon Sandbox Mode"
Mở file [GameManager.cpp](file:///d:/APCS/CS202%20-%20Programming%20Systems/Chicken-Invader-main/Chicken-Invader/src/GameManager.cpp), trong hàm `Init()`, tạm thời đổi cờ khởi tạo nhịp điệu từ gameplay thường sang trạng thái kiểm thử (hoặc kích hoạt qua các tổ hợp phím kiểm thử nhúng thẳng vào `UpdateTestGameplay`):

```cpp
// Thêm vào khối nhận dạng phím trong GameManager::Update() để tự do chuyển đổi
if (IsKeyPressed(KEY_F1)) debugSandboxMode = !debugSandboxMode; // Bật / tắt sandbox
if (debugSandboxMode) {
    // Phím 1 -> 8: Chuyển đổi qua lại giữa 8 loại vũ khí
    if (IsKeyPressed(KEY_ONE))   player->SetWeapon("Hypergun");
    if (IsKeyPressed(KEY_TWO))   player->SetWeapon("Plasma_Rifle");
    if (IsKeyPressed(KEY_THREE)) player->SetWeapon("Absolver_Beam");
    if (IsKeyPressed(KEY_FOUR))  player->SetWeapon("Neutron_Gun");
    if (IsKeyPressed(KEY_FIVE))  player->SetWeapon("Riddler");
    if (IsKeyPressed(KEY_SIX))   player->SetWeapon("Lightning_Fryer");
    if (IsKeyPressed(KEY_SEVEN)) player->SetWeapon("Ion_Blaster");
    if (IsKeyPressed(KEY_EIGHT)) player->SetWeapon("Utensil_Poker");

    // Mũi Tên Lên / Xuống: Nâng hoặc hạ trực tiếp Level đạn (Lv1 -> Lv10 -> Mana)
    if (IsKeyPressed(KEY_UP))    player->SetLevel(std::min(player->GetLevel() + 1, 11));
    if (IsKeyPressed(KEY_DOWN))  player->SetLevel(std::max(player->GetLevel() - 1, 1));
    
    // Phím H: Toggle Bật / Tắt khung xương Hitbox màu đỏ xanh
    if (IsKeyPressed(KEY_H))     showDebugHitboxes = !showDebugHitboxes;
    
    // Phím F: Chức năng Slow-Motion (giảm deltaTime xuống 0.1x để ngắm kỹ góc bay đạn)
    if (IsKeyDown(KEY_F))        deltaTime *= 0.1f;
}
```

### 5.2. Bước 2: Thực Thi Cục Bộ Các Test Cases Nghiệm Thu

#### 🧪 Test Case 1: Kiểm Định Quỹ Đạo Bắn & Độ Xòe Góc (Trajectory & Angles)
- **Mục đích:** Xác minh các công thức tính góc bay, số lượng đạn phát xạ chính xác theo tài liệu specs.
- **Thực hiện:**
  1. Khởi chạy game, ấn **F1** để bật Sandbox, ấn **Phím số 5** để chuyển sang vũ khí **Riddler**.
  2. Bấm phím **Down Arrow** đưa về **Level 1**, bấm bắn: Quan sát ra chính xác **1 viên đạn** với **Circle Hitbox hình tròn xanh lục** bao quanh đạn, quỹ đạo tự do lệch trong biên xòe quạt tối đa **13.5°**.
  3. Bấm phím **Up Arrow** nâng liên tục lên **Level 10**, giữ phím **F (Slow-motion)** rồi kích hoạt nhấp thả bắn 1 băng đạn: Đếm qua hình ảnh quay chậm hiển thị đủ **10 viên đạn đồng bộ** xòe quạt với góc được bó hẹp chuẩn xác tại **8.1°**.
  4. Ấn **Phím số 1** chuyển sang **Hypergun**, giữ phím bắn liên thanh trong 2 giây chao lượn: Nhận diện rõ rệt chu kỳ **8 nhịp lắc quỹ đạo** qua pháo giữa, đồng thời quan sát đạn nòng hai bên cánh bay nhịp nhành **chậm hơn gấp 4 lần** đạn nòng trung tâm.

#### 🧪 Test Case 2: Nghiệm Thu Auto-Locking Tự Nhắm, Hitbox & Cơ Chế Đạn Tia (Beam Raycast & Auto-Locking)
- **Mục đích:** Đảm bảo tia sét của *Lightning Fryer* tự động ngắm uốn lượn khóa trúng mục tiêu gần nhất, co dãn chính xác tỷ lệ hitbox.
- **Thực hiện:**
  1. Ấn **Phím H** để bật vạch Debug Hitbox.
  2. Ấn **Phím số 6** để chuyển sang vũ khí đạn tia **Lightning Fryer**, hạ về **Level 1**.
  3. Đặt/thả 1 quái vật di chuyển lệch bên trái màn hình ($X=400, Y=300$). Khi nhấn giữ phím bắn, quan sát tia sét và khung hitbox **tự động bẻ nghiêng góc $\theta$** lao thẳng chốt khóa vào tâm quái vật thay vì bắn vút mù quáng thẳng lên trời!
  4. Nâng lên **Level 6**: Khung hitbox nở rổ chấn dốc đạt chuẩn tối đa **100% width** tiêu chuẩn.
  5. Nâng lên **Level 10** hoặc **Mana Mode** và thả 3 quái vật ở 3 góc xa nhau: Nhận diện tức thì hệ thống 3 nòng (`tip`, `front_left`, `front_right`) tự động chia ra **tác chiến khóa độc lập vào 3 con quái vật khác nhau cùng lúc**!

#### 🧪 Test Case 3: Chịu Tải, Chứng Thức Silent Drop & Tái Sử Dụng Hồ (Object Pool & Overflow Test)
- **Mục đích:** Xác nhận không có hiện tượng rò rỉ RAM, rớt FPS hay văng game khi xả bão đạn vượt tải.
- **Thực hiện:**
  1. Mở bộ theo dõi FPS tích hợp ở góc phải màn hình (`DrawFPS(10, 10)`).
  2. Chọn phím số 5 (**Riddler**) hoặc phím số 7 (**Ion Blaster**), bấm phím mũi tên lên kịch trần đến chế độ tối thượng **Level 11 (Mana Mode)**.
  3. Bật cờ kiểm thử "Bắn tốc độ ánh sáng" (giảm `fireTimer` về `0.001s`) để phi thuyền xả cuồng loạn hàng ngàn viên đạn ra khắp không trung.
  4. **Tiêu chuẩn nghiệm thu thành công:**
     - Khi số đạn đang bay chạm mốc 1000 trong `BulletPool`, quy tắc **Silent Drop** ngay lập tức kích hoạt: các đợt bắn thứ 1001+ im lặng bị từ chối sinh ra mà không hề văng lỗi bộ nhớ hay ngắt chương trình (Zero crash / Zero segfault)!
     - Tốc độ khung hình (FPS) giữ vững nguyên mượt mà ở **60 FPS / 144 FPS**.
     - Lượng RAM trò chơi tiêu thụ **Bất Biến (Static Resident Memory)**, xác nhận 1000 viên đạn trong `BulletPool` lặp đi lặp lại luân phiên vòng đời thu nhận và tái sinh mà không phát sinh thêm bất kỳ 1 phép cấp phát động `new/delete` vô ích nào!
