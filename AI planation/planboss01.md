# Kế Hoạch Triển Khai Boss Stage 1 - Wave 5 (chicken09)

## 1. Mục Tiêu
Tích hợp `chicken09.png` làm Boss cuối cho Stage 1 (Wave 5). Boss này sẽ có kích thước lớn hơn, animation phức tạp (với hiệu ứng tia lửa ở cánh), và sử dụng đạn lửa (`fire_bullet.png`) kết hợp với particle effects thay vì trứng thông thường.

## 2. Phân Tích & Chuẩn Bị Tài Nguyên

### 2.1 Animation Gà Boss (`chicken09.png`)
- **Tình trạng hiện tại:** `chicken09.png` là một sprite đơn (có thể có kích thước lớn như 220x195). Engine game đang hỗ trợ vẽ sprite grid (thường là 100x100 base) cho animation mượt mà qua 24 frames.
- **Bước thực hiện:**
  - Dùng script python (như `batch_warp.py`) hoặc resize `chicken09.png` và sinh ra sprite sheet `chicken09_anim.png` gồm 24 frames. Để giữ chi tiết tốt hơn, có thể sprite sheet của Boss sẽ dùng độ phân giải cao hơn (ví dụ grid 200x200 cho mỗi frame) nhưng vẽ trong game ở kích thước mong muốn.
  - Sẽ cần cập nhật GameManager để load sprite sheet mới này (ví dụ vào index 9 của `texEnemyAnims`).

### 2.2 Hiệu Ứng Bắn Đạn Lửa (`fire_bullet.png`)
- **Tài nguyên:** `assets/fire_bullet.png`.
- **Bước thực hiện:**
  - Thêm `texFireBullet` vào `GameManager`.
  - Cập nhật class `Bullet` để hỗ trợ `bulletType = 100` (Boss Fire). Trong `Bullet::Draw()`, nếu đạn là của Boss, vẽ texture này thay vì `texEnemyBullet`.
  - Tạo hệ thống hạt (Particle System) đơn giản (hoặc tái sử dụng `TrailPoint` đang dùng cho Asteroid) để khi bay, viên đạn lửa để lại vệt lửa/khói (trail) đằng sau để tăng tính thẩm mỹ.

### 2.3 Hiệu Ứng Lửa Động Cơ / Cánh của Boss
- **Tình trạng:** Khi Boss di chuyển (vô cánh), cần có hiệu ứng tia lửa rớt ra.
- **Bước thực hiện:**
  - Kế thừa hoặc mở rộng struct `TrailPoint` trong `Enemy.cpp` thành một list `SparkParticles`.
  - Trong `Enemy::Update()`, nếu `role == EnemyRole::BOSS`, sinh ra các hạt (spark) tại vị trí tương đối so với tọa độ trung tâm (chỗ cánh 2 bên).
  - Vẽ các hạt này bằng `DrawCircle()` (màu Vàng/Cam/Đỏ fading) hoặc bằng texture khói nhỏ trong `Enemy::Draw()`.

## 3. Cập Nhật Code Logic

### 3.1 `Enemy::baseSizeForType()` & Hitbox
- Hiện tại `Enemy.cpp` đang hardcode size vẽ về 100x100 và hitbox về 50x50.
- Cần sửa lại:
  ```cpp
  float Enemy::baseSizeForType() const {
      if (role == EnemyRole::BOSS) return 250.0f; // Boss to gấp 2.5 lần
      if (role == EnemyRole::TANK) return 150.0f;
      return 100.0f;
  }
  
  Rectangle Enemy::GetHitbox() const {
      float s = baseSizeForType() * 0.5f; // Hitbox bằng 50% kích thước base
      return {position.x - s/2, position.y - s/2, s, s};
  }
  ```

### 3.2 Strategy Di Chuyển (BossMovement)
- Cần một pattern di chuyển đặc biệt cho Boss (ví dụ Hover trái/phải, thỉnh thoảng sà xuống hoặc rải đạn theo vòng tròn).
- Xây dựng class `BossHoverMovement : public IMovementBehavior` giúp Boss luôn lảng vảng ở nửa trên màn hình (y = 150 -> 300) và lướt qua lại (Sine wave).

### 3.3 Đạn Dạng Lưới / Đạn Đặc Biệt
- Sửa đổi hàm `Enemy::Update()` (chỗ `eggDropTimer`). Nếu là `EnemyRole::BOSS`, spawn một chùm đạn `fire_bullet` tản ra (Spread Shot) thay vì thả 1 quả trứng rơi tự do. Có thể tạo một hàm `BossShoot()` để gói logic này.

## 4. Thiết Kế Màn Chơi (`data/stage1.json`)
- Cập nhật wave 5 vào JSON (hiện tại chỉ có 4 wave):
  ```json
  {
      "wave_id": 5,
      "batches": [
          {
              "batch_id": 1,
              "visual_id": 9,
              "role": "BOSS",
              "count": 1,
              "layout": {
                  "type": "TARGETED_PLAYER",
                  "start_y": -200.0
              },
              "movement": {
                  "type": "BOSS_HOVER"
              },
              "stats": {
                  "hp": 8000,
                  "damage": 50,
                  "armor": 30,
                  "speed": 120,
                  "egg_rate": 1.5,
                  "score": 1000
              }
          }
      ]
  }
  ```

## 5. Trình Tự Thực Hiện (Step-by-step)

1. **Bước 1: Cấu hình Tài Nguyên & Asset**
   - Viết hoặc sử dụng script chạy tạo `chicken09_anim.png`.
   - Bổ sung việc load `fire_bullet.png` và `chicken09_anim.png` vào `GameManager::Init`.

2. **Bước 2: Cập Nhật Hitbox và Scaling (OOP)**
   - Chỉnh sửa `Enemy.cpp` (`baseSizeForType` và `GetHitbox`) để Boss hiển thị to lớn (size 250) và có vòng hit đúng.

3. **Bước 3: Code Cơ Chế Bắn Lửa & Particles**
   - Implement `BossFireBullet` rendering trong `Bullet::Draw()`.
   - Cập nhật `Enemy::Update` để nếu `role == BOSS`:
     - Bắn 3-5 tia `fire_bullet`.
     - Spawn particles tia lửa (spark) ở đuôi cánh (trái & phải).
   - Render particles trong `Enemy::Draw()`.

4. **Bước 4: Logic Di Chuyển & Stage Config**
   - Thêm class `BossHoverMovement` và ánh xạ ở `WaveManager::SpawnBatch()`.
   - Thêm Wave 5 vào `stage1.json`.

5. **Bước 5: Chạy Thử & Điều Chỉnh**
   - Chơi thử Wave 5, cân bằng HP, sát thương, tốc độ đạn và thẩm mỹ của particle effects. Đảm bảo game không crash.
