# Plan Boss 02: The Egg-secutioner (Chicken 11) — Stage 2, Wave 5

## 1. Mục Tiêu

Thêm **Boss The Egg-secutioner (chicken11)** vào Stage 2, Wave 5. Boss này sẽ:
- Đóng vai trò là Mid-Boss (Boss giữa màn) của Stage 2.
- Chỉ có **1 mạng (1 phase)** với lượng HP khổng lồ (40,000 HP).
- Sử dụng hoàn toàn **Đạn màu Đỏ (Red Bullets)** để tách biệt hoàn toàn về mặt thiết kế (OOP) so với Boss Stage 1 (dùng đạn vàng/cam).
- Cách di chuyển đặc biệt (Figure-8) kết hợp lao nhanh (Dash) thay vì chỉ hover như Boss Stage 1.
- Tận dụng quy chuẩn tạo High-Resolution Boss Sprite từ `chicken_animation_reference.md` để tự động hóa kích thước frame ảnh (không bị lỗi 16/24 frames).

---

## 2. Phân Tích & Giải Quyết Lỗi Animation Cũ

**Vấn đề cũ:** Boss hiển thị cánh bị cứng, hoặc cánh không đập mà chỉ có phần lông vặn vẹo. Chân không đối xứng hoặc biến dạng.
**Giải pháp theo `chicken_animation_reference.md` (Mục 6.5) và tinh chỉnh Geometry:**
- Sử dụng ảnh tĩnh `chicken11.png` (500x500) mà bạn vừa sửa lại.
- Dạy script Python `batch_warp_v5.py` warp ảnh với `target_size=200` dành riêng cho Boss.
- **Tinh chỉnh Geometry đặc biệt cho gà 11:** 
  - Đặt Pivot cánh (khớp vai) chính xác vào `X = 80` và `X = 120` để khớp với độ rộng đầu/thân của Boss, giúp TẤT CẢ phần cánh màu đỏ và lông trắng đập mượt mà.
  - Đặt lại toạ độ chia chân (`Y = 130`) và chia đều từ tâm (`X = 100.5`) để đảm bảo 2 chân di chuyển đối xứng và không bị rách phần thân màu đỏ.
- Output sinh ra sẽ là `chicken11_anim.png` kích thước **4800x200** (24 frames x 200px).
- Trong `Boss::Draw()` base class, code đã có sẵn logic tự động lấy chiều cao của ảnh làm kích thước frame (`frameSize = tex.height;`). Nhờ vậy, game sẽ tự nhận diện frame width là `200` một cách chuẩn xác, không bao giờ bị lỗi cắt sai pixel.

---

## 3. Thiết Kế Chi Tiết "The Egg-secutioner"

### 3.1 Chỉ Số Cơ Bản (1 Phase - Không Hồi Sinh)
- **HP**: 40,000 HP.
- **Armor**: 15 (Người chơi cần súng cấp cao để bắn xuyên giáp).
- **Hitbox**: Tròn, bán kính 200x200 (kế thừa từ `Boss`).
- **Texture Size**: 200x200 px in-game (vẽ theo tỷ lệ 1:1 pixel-perfect từ ảnh sinh ra).

### 3.2 Hệ Thống Di Chuyển (Hover & Dash)
Boss sẽ không di chuyển hỗn loạn liên tục, mà đan xen các khoảng thời gian dừng lại để tấn công:
1. **Bay lơ lửng tại chỗ (Hover Standby)**:
   - Boss dừng lại ở một vị trí hoặc trôi rất chậm trong khoảng 10-15 giây để nhả đạn liên tục.
   - Giúp giảm sự rối mắt và tạo cửa sổ (window) cho người chơi xả đạn.
2. **Di chuyển đổi vị trí (Reposition)**:
   - Sau khi hết thời gian hover, Boss sẽ di chuyển nhẹ nhàng sang một vị trí mới (ví dụ: chuyển từ bên trái sang bên phải màn hình) rồi lại dừng lại.
3. **Dash (Lao sầm)**:
   - Rất hiếm khi xảy ra, Boss lấy đà và lao nhanh (Dash) một lần rồi lại trở về trạng thái Hover.

### 3.3 Hệ Thống Tấn Công Đặc Trưng (Toàn Đạn Đỏ)

Boss sẽ xoay vòng 3 kỹ năng theo thứ tự: **Bouncing Bullets -> Red Darts -> Red Nova**.

- **Kỹ năng 1: Bouncing Red Bullets (Đạn nảy)**
  - Bắn 5 viên đạn đỏ cực lớn bay tản ra 5 hướng.
  - Khi đạn chạm viền màn hình sẽ **nảy lại** (Bội số tối đa 3 lần nảy).
  - Tốc độ: 250 px/s.
  - Cooldown sau đòn: 2.0s.

- **Kỹ năng 2: Red Feather Darts (Phi tiêu đỏ đuổi mục tiêu)**
  - Tương tự tính năng Auto-Aim (`atan2`) nhưng tốc độ bay cực nhanh (450 px/s).
  - Boss ngắm thẳng vào vị trí người chơi và bắn 3 viên đạn đỏ sát nhau (chênh góc 15 độ).
  - Cooldown sau đòn: 1.5s.

- **Kỹ năng 3: Red Egg Nova (Nổ siêu đạn)**
  - Boss vận nội công, bắn ra một chùm 36 viên đạn đỏ rực tản ra 360 độ quanh bản thân.
  - Mỗi góc cách nhau 10 độ.
  - Tốc độ đạn: 180 px/s.
  - Cooldown sau đòn: 4.0s (Cho người chơi thời gian thở).

---

## 4. Thiết Kế OOP Tách Biệt Hoàn Toàn

Để đảm bảo không ảnh hưởng code Stage 1, mọi class mới sẽ được lập trình độc lập và tuân thủ chặt chẽ OOP.

### 4.1 Hệ thống Đạn Đỏ (Red Bullets)
Thay vì xài chung `FireBullet` của Stage 1, ta sẽ tạo hẳn 2 class đạn mới kế thừa từ `Bullet`:
1. `BouncingRedBullet`
   - Ghi đè `Update()` để tính toán đập tường (check bounding box X/Y).
   - Ghi đè `Draw()` để vẽ hình tròn phát sáng màu đỏ (Gradient Đỏ nhạt -> Đỏ sẫm).
2. `RedBossBullet`
   - Đạn thường bay thẳng, không nảy.
   - Vẽ hình tròn Gradient màu đỏ, có kích thước nhỏ hơn Bouncing Bullet một chút.

### 4.2 Lớp `EggsecutionerBoss`
```cpp
class EggsecutionerBoss : public Boss {
public:
    EggsecutionerBoss(int visualId, const EnemyStats& stats, Vector2 pos);
    void Update(float deltaTime) override;
    void Draw() override;
    void Die() override; // Chết 1 lần là nổ luôn (kế thừa logic thường)
    
private:
    float attackTimer;
    int nextSkillType;
    float attackCooldown;
    
    // Variables for Figure-8 and Dash
    float moveTimer;
    bool isDashing;
    Vector2 dashTarget;
    float dashSpeed;
    float normalSpeed;

    // Skills
    void FireBouncingBullets();
    void FireRedDarts();
    void FireRedNova();
    void DrawBossHPBar(); // Thanh máu màu tím mộng mơ (hoặc đỏ sẫm)
};
```

---

## 5. Tích hợp Hệ thống (Data-Driven)

### 5.1 Cập nhật JSON (`data/stage2.json`)
Thêm cấu hình Boss vào JSON của Stage 2:
```json
{
    "batch_id": 1,
    "visual_id": 11,
    "role": "BOSS",
    "count": 1,
    ...
}
```

### 5.2 Cập nhật Factory (`EnemyFactory.cpp`)
```cpp
if (role == EnemyRole::BOSS) {
    if (visualId == 9) {
        return std::make_unique<FirePhoenixBoss>(...);
    } else if (visualId == 11) {
        return std::make_unique<EggsecutionerBoss>(...);
    }
}
```

---

## 6. Trình Tự Triển Khai (Dự kiến)

1. **Chuẩn bị Asset**: Gọi script python resize và warp lưới ảnh `chicken11.png` mới ra dạng 24 frames, target_size=200px.
2. **Code Đạn**: Viết class `BouncingRedBullet` và `RedBossBullet` trong `Bosses.h/cpp`.
3. **Code Boss**: Viết logic Figure-8, Dash và xoay vòng 3 kỹ năng cho `EggsecutionerBoss`.
4. **Tích hợp**: Cập nhật `EnemyFactory` và `data/stage2.json`.
5. **Biên dịch & Chạy thử**: Xác nhận đạn Stage 1 (FireBullet) vẫn màu cam, đạn Stage 2 (RedBossBullet) màu đỏ rực. Kích thước animation không bị rách frame.
