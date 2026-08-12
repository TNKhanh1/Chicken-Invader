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
  - Di chuyển: Rơi thẳng (`STRAIGHT`) với tốc độ rất nhanh (Speed = 60). Chúng được cấu hình xuất phát sát rạt mép trên màn hình để ngay lập tức lao ập xuống người chơi, gây áp lực cực mạnh. Bài test cuối cùng trước khi bước vào Wave 5.

---

## 4. Wave 5: Thử Thách Sinh Tồn - Kẻ Hủy Diệt Đột Biến (Mutant Bullet-Hell)

Theo đúng yêu cầu của bạn, Wave 5 sẽ gồm 2 Batch riêng biệt, cả hai đều sử dụng đặc trưng xòe đạn của `chicken04` nhưng sẽ có sự pha trộn giữa **Gà Kích thước Nhỏ** và **Gà Đột Biến Khổng Lồ** (Size to hơn, HP trâu hơn, Sát thương khủng hơn). 

### Nâng cấp Hệ thống C++ (Bắt buộc)
1. **Dynamic Scale:** Thêm `float scale` vào `EnemyStats`, cập nhật hàm `baseSizeForType()` và `GetHitbox()` để nhân với `scale`, cho phép vẽ gà to/nhỏ tùy ý (Đã hoàn thành).
2. **Custom Movement Speed Multipliers:** Để đáp ứng yêu cầu "tốc độ đi xuống nhanh hơn 30% nhưng trái phải giữ nguyên" cho `VERTICAL_ZIGZAG` ở Batch 2:
   - Sửa class `VerticalZigzagMovement` để nhận 2 tham số mới: `downwardSpeedMult` (mặc định 0.3) và `horizontalSpeedMult` (mặc định 0.5).
   - Trong `WaveManager.cpp`, đọc 2 tham số này từ trường `movement` trong JSON.
   - Đối với Wave 5 Batch 2, ta sẽ set `downward_speed_mult = 0.39` (tức là 0.3 + 30%).

### Thiết kế Đội hình (JSON Configuration)

Việc "trộn" nhiều kích thước vào một đội hình sẽ được thực hiện bằng kỹ thuật **Ghép lớp (Overlay Batches)**. Nghĩa là trong cùng 1 Batch Time, ta spawn nhiều nhóm bù trừ vị trí cho nhau.

#### Batch 1: Ma trận Chữ Nhật (The Rectangle Matrix)
Gồm 2 lớp xếp chồng nhau tạo thành một hình chữ nhật đặc:
- **Lớp Vỏ (Gà Nhỏ):** Lưới `GRID` 3 hàng x 4 cột `chicken04` nhỏ (`scale: 1.0`, `hp: 300`). 
- **Lớp Lõi (Gà Khổng Lồ):** Lưới `GRID` 2 hàng x 3 cột `chicken04` đột biến (`scale: 1.3`, `hp: 400`). Nằm lọt thỏm vào những khoảng trống ở giữa các con gà nhỏ.
- **Chuyển động:** `HORIZONTAL_BOUNCE` đồng điệu.

#### Batch 2: Đội Hình Tam Giác (The Triangle Spearhead)
- **Đỉnh Tam Giác:** 1 con `chicken04` to khổng lồ (`scale: 1.5`, `hp: 600`) bay ở ngay đỉnh mũi nhọn, `target_base_y: 200`.
- **Hai Cánh Tam Giác:** 9 con `chicken04` (`scale: 1.0`) xếp hình `V_SHAPE`, `target_base_y: 100`.
- **Chuyển động:** `VERTICAL_ZIGZAG` với `downward_speed_mult: 0.39` (Nhanh hơn 30% khi lao xuống).

---

## 5. Kế hoạch Wave 6, 7, 8, 9 (Chặng đường cuối trước Boss)

Theo yêu cầu, mỗi Wave sẽ có chính xác 2 Batch, đội hình đơn giản tinh tế, và chỉ sử dụng gà từ loại 1 đến 4. Dưới đây là plan chi tiết:

### Wave 6: Bão Mưa Đá & Bắn Tỉa (Meteor Sniper)
- **Batch 1:** Mưa thiên thạch `METEOR_DIVE` rơi liên tục. Cùng lúc đó, 1 hàng ngang (GRID 1x5) `chicken03` (Sniper - Bắn tỉa nhắm thẳng người chơi) lướt ngang ở mép trên màn hình. Ép người chơi phải luồn lách qua đá trong khi bị nhắm bắn.
- **Batch 2:** 2 cột dọc `chicken02` (Tanker - Trâu bò) từ từ trôi xuống từ 2 mép trái phải màn hình. Ở giữa là 1 đàn `chicken01` (Spread - Đạn tỏa) bay zíc zắc (`SINE_ZIGZAG`) đi xuống.

### Wave 7: Gọng Kìm Tỏa Đạn (Pincer Sweep)
- **Batch 1:** 1 hàng `chicken04` (Đạn quạt) đứng lơ lửng ở trên. Hai nhóm `chicken01` (Đạn tỏa) sử dụng `SWEEP_TO_GRID` bay vòng cung từ 2 góc trái/phải màn hình vào giữa để tạo thành một lớp rào chắn đạn.
- **Batch 2:** Chữ X cắt chéo (`INTERSECTING_V`). 2 nhóm `chicken03` (Bắn tỉa) và 2 nhóm `chicken04` bay cắt chéo qua nhau. Khi gặp nhau ở giữa màn hình, đạn của chúng sẽ hòa trộn tạo thành một mạn nhện đạn cực kỳ đẹp mắt.

### Wave 8: Hộ Giá Khổng Lồ (Giant Escort)
Sử dụng tính năng Scale vừa phát triển để tạo ra các "Tiểu Boss".
- **Batch 1:** 1 Tiên phong Khổng lồ `chicken02` (Tanker, Scale 2.5, HP siêu trâu 3000) được bao bọc bởi một lớp vỏ chữ V (V_SHAPE) gồm 10 con `chicken01` (Scale 1.0). Tất cả di chuyển zíc zắc dọc (`VERTICAL_ZIGZAG`).
- **Batch 2:** 2 Pháo đài Khổng lồ `chicken03` (Sniper, Scale 2.0, HP 1500) được yểm trợ bởi 2 cụm `chicken04` (Fan). Tất cả lướt ngang qua lại `HORIZONTAL_BOUNCE`.

### Wave 9: Tử Chiến Tiền Phương (Vanguard Last Stand)
Wave cuối cùng trước khi gặp Boss Stage 4, tạo ra một bức tường đạn thực sự.
- **Batch 1 (Bức Tường Hỗn Loạn):** Dùng kỹ thuật Overlay Batch tạo một lưới `GRID` 3x5 đan xen giữa `chicken01`, `chicken03`, và `chicken04`. Chúng sẽ từ từ trôi thẳng xuống (`STRAIGHT`) chậm rãi như một chiếc máy ép khổng lồ.
- **Batch 2 (Mũi Khoan Kép):** 2 hàng `chicken04` khổng lồ (Scale 1.5) dùng `WAYPOINT` lướt siêu tốc hình chữ Z qua lại màn hình và liên tục xả đạn quạt.

## User Review Required
> [!IMPORTANT]
> 1. Tính năng **tốc độ lao xuống 30%** sẽ được hiện thực hóa bằng 2 tham số `downward_speed_mult` và `horizontal_speed_mult` cực kỳ linh hoạt cho game.
> 2. Plan cho Wave 6, 7, 8, 9 đã được xây dựng theo đúng yêu cầu: **2 Batch/Wave, Đội hình đơn giản nhưng kết hợp khéo léo, Sử dụng gà từ 1-4 và tận dụng triệt để tính năng Scale đột biến**.
> 3. Xin vui lòng kiểm tra và phản hồi xem bạn có ưng ý với cấu trúc 4 Wave cuối này không để tôi tiến hành viết C++ và JSON luôn!

