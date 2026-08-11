# Implementation Plan: Stage 4 - Mưa Trứng Góc Rộng (Waves 1 - 4)

## Mục Tiêu
- **Kẻ thù chủ đạo:** `chicken04` (Sẽ tương ứng với `visualId = 4` do `chicken04_anim.png` được load ở index 3 trong mảng `texEnemyAnims`).
- **Cơ chế đặc trưng:** Mỗi lần `chicken04` bắn sẽ nhả ra **3 quả trứng xòe thành hình quạt** (Fan spread), tạo ra mật độ đạn dày đặc mang hơi hướng Bullet-Hell.
- **Tuân thủ OOP & Toàn vẹn Code cũ:** Tuyệt đối không thay đổi hay đập đi xây lại hệ thống bắn của quái vật cũ. Cơ chế 3 trứng sẽ được tích hợp khéo léo vào luồng xử lý đạn hiện tại.

## 1. Phân Tích & Thiết Kế Animation `chicken04`

Theo yêu cầu, tôi đã dùng lệnh Python scan trực tiếp file ảnh của `chicken04` để đảm bảo độ chính xác tuyệt đối trước khi lên kế hoạch. 
- **Kết quả scan:**
  - File `assets/enemy/chicken04.png` (Ảnh gốc): Kích thước 150x150.
  - File `assets/enemy/chicken04_anim.png` (Sprite sheet): Kích thước 2400x100.
- **Phân tích toán học:** 
  - Chiều rộng 2400px chia đều cho 24 khung hình (frames), ta được chính xác **100px mỗi khung hình** (Kích thước mỗi frame là 100x100). Hình vuông hoàn hảo.
- **Tích hợp vào hệ thống (OOP):**
  - Trong `Enemy.cpp::Draw()`, hệ thống đang sử dụng biến `frameSize = tex.height`. Với `chicken04_anim.png`, `tex.height` là 100, do đó `frameSize` tự động khớp với 100px.
  - Kích thước render gốc `baseSizeForType()` được cài mặc định là 100.0f.
  - **Kết luận:** Sprite sheet của `chicken04` sẽ được hệ thống Render cắt (crop) tự động với toạ độ `(currentAnimFrame * 100, 0, 100, 100)` và vẽ ra màn hình không bị méo (Tỉ lệ 1:1, không bị co giãn vỡ hạt). Không cần phải sửa đổi thêm hệ thống animation gốc, đảm bảo tính toàn vẹn tuyệt đối.

---

## 2. Hướng Tiếp Cận Lập Trình (OOP & C++ Logic)

Trong `Enemy::Update()`, hệ thống bắn đạn (thả trứng) hiện tại được điều khiển bởi biến `canShoot` và `eggDropTimer`. Để thêm cơ chế cho `chicken04` mà không làm hỏng logic của `chicken01`, `02`, `03` hay Boss, ta sẽ áp dụng cách rẽ nhánh an toàn tương tự như cách `chicken03` đang làm (nhả 2 trứng).

**Chi tiết đoạn code dự định chèn vào `Enemy::Update()`:**
Khi `eggDropTimer <= 0.0f`, thay vì chỉ gọi 1 lệnh tạo `Bullet` hướng thẳng xuống, ta sẽ kiểm tra:
```cpp
if (visualId == 4) {
    // chicken04: Bắn 3 viên xòe hình quạt (Fan Spread)
    float angleSpread = 20.0f; // Góc lệch giữa các viên đạn (độ)
    for (int i = -1; i <= 1; i++) {
        // Góc bắn: thẳng xuống là 90 độ. Viên bên trái: 70 độ, viên bên phải: 110 độ.
        float rad = (90.0f + i * angleSpread) * (PI / 180.0f);
        Vector2 vel = { cosf(rad) * 175.0f, sinf(rad) * 175.0f };
        
        auto egg = std::make_shared<Bullet>(position, damage, 175.0f, false);
        egg->SetVelocity(vel); // Hàm SetVelocity đã có sẵn trong class Bullet
        GameManager::GetInstance()->AddBullet(egg);
    }
} else {
    // Các loại gà khác: Rơi thẳng xuống (Logic cũ giữ nguyên)
    auto egg = std::make_shared<Bullet>(position, damage, 175.0f, false);
    GameManager::GetInstance()->AddBullet(egg);
    
    // Logic của chicken03 thả 2 quả liên tiếp (giữ nguyên)
    if (visualId == 3 && GetRandomValue(1, 100) <= 30) { ... }
}
```
**Ưu điểm:**
- Sử dụng hàm `SetVelocity()` có sẵn của `Bullet`, hoàn toàn chuẩn OOP và tận dụng tốt class `Bullet`.
- Không đụng chạm hay phá vỡ hành vi của các stage cũ.

---

## 3. Thiết Kế Đội Hình (Wave 1 đến Wave 4)

Vì `chicken04` bắn 3 viên đạn tỏa ra xung quanh, nếu để chúng đứng quá gần nhau hoặc tạo đội hình quá dày, người chơi sẽ không có chỗ để né (Góc mù). Do đó, thiết kế sẽ tập trung vào sự phân tán, kết hợp các kiểu di chuyển sáng tạo.

### Wave 1: Khởi động - Làm quen với "Góc Quạt"
- **Batch 1:** Đội hình cơ bản `GRID` (3 hàng ngang x 5 cột). 
  - Toàn bộ là `chicken04`.
  - Di chuyển: `HORIZONTAL_BOUNCE` với `drift` lớn (150px) để khắc phục lỗi giật lắc. Quái sẽ trượt qua lại một khoảng rộng và bắn xòe 3 tia, ép người chơi di chuyển liên tục theo chiều ngang.
- **Batch 2:** Sự kết hợp đồng điệu.
  - Hàng đầu tiên: `chicken01` di chuyển `SINE_ZIGZAG`.
  - Hàng thứ hai: `chicken04` (Bắn góc quạt) cũng di chuyển `SINE_ZIGZAG` với cùng tốc độ.
  - Tốc độ đung đưa (`frequency`) được điều chỉnh giảm xuống để bớt chóng mặt, và cả 2 đàn đều đồng bộ tốc độ lướt xuống (Speed = 100), giải quyết triệt để lỗi hai đàn đâm sầm và đè lên nhau. Tạo ra ma trận đạn lượn lờ kết hợp với đạn quạt chéo mượt mà.

### Wave 2: Giao thoa Đạn & Cơn mưa Thiên thạch
- **Batch 1:** `ASTEROID` (Thiên thạch rơi) kết hợp với `chicken04`.
  - Thiên thạch chia làm 2 bên lề màn hình rơi xuống liên tục để thu hẹp không gian di chuyển của người chơi.
  - Ở giữa là 2 cột dọc `chicken04` di chuyển `STRAIGHT` từ từ xuống, nhả đạn hình quạt. Người chơi phải căn lách qua kẽ hở giữa thiên thạch và các tia trứng.
- **Batch 2:** Đội hình V (Chữ V ngược).
  - Con đi đầu và 2 con bọc hậu là `chicken03` (Thả trứng liên tiếp).
  - Cánh của chữ V là `chicken04`. 
  - Di chuyển: `VERTICAL_ZIGZAG` (Tiến lùi liên tục), tạo cảm giác đội hình này đang thở và bơm đạn liên tục.

### Wave 3: Đội Hình Sáng Tạo - Gọng Kìm (Pincer Movement)
- **Batch 1:** 
  - 2 hàng `chicken04` xuất hiện từ 2 góc đối diện (Ví dụ một hàng từ trái sang, một hàng từ phải sang).
  - Di chuyển: `HORIZONTAL_SWEEP`. Chúng sẽ quét chéo qua nhau (Crossing) ngay giữa màn hình. Khi 2 nguồn đạn góc quạt đan chéo nhau, người chơi sẽ phải tìm ra "Tâm bão" để đứng an toàn.
- **Batch 2:** Bức tường xe tăng (Không đè đội hình).
  - Hàng đầu: `chicken02` (Gà Tanker, máu cực trâu) làm lá chắn sống, dừng ở mốc Y=300 (Thấp nhất).
  - Hai hàng sau: `chicken04` nấp sau lưng xả đạn góc quạt, dừng ở mốc Y=100 và 200 (Cao hơn, ở phía sau lưng Tanker). Đội hình này không hề bị đè lên nhau.
  - Toàn bộ đều di chuyển `HORIZONTAL_BOUNCE` với biên độ trượt cực rộng (`drift` = 150) để lướt qua lại mượt mà, không giật cục.

### Wave 4: Tinh Nhuệ (Elite Swarm - Tiền Boss)
- **Batch 1 (Phục kích):** Đội hình vòng cung (Sử dụng `WAYPOINT` để tạo đường bay).
  - Một nhóm `chicken04` nối đuôi nhau bay theo đường vòng cung từ góc trên bên phải uốn lượn sang góc dưới bên trái, sau đó bay vút lên. 
  - Trứng tỏa ra liên tục dọc theo quỹ đạo uốn lượn tạo ra một dải thiên hà trứng cực kỳ đẹp mắt và nguy hiểm.
- **Batch 2 (Đại chiến):** 
  - Đội hình Khối hình vuông khổng lồ (4 hàng x 7 cột).
  - Hàng 1: `chicken01`
  - Hàng 2: `chicken03`
  - Hàng 3 & 4: `chicken04`
  - Di chuyển: Rơi thẳng (`STRAIGHT`) với tốc độ rất nhanh (Speed = 60). Chúng được cấu hình xuất phát sát rạt mép trên màn hình để ngay lập tức lao ập xuống người chơi, gây áp lực cực mạnh. Bài test cuối cùng trước khi đối mặt với Boss (Wave 5).

## User Review Required
> [!IMPORTANT]
> - Thiết kế đạn quạt (Fan spread) dựa trên góc toán học (70 độ, 90 độ, 110 độ).
> - Kế hoạch giữ nguyên 100% logic cấu trúc cũ.

