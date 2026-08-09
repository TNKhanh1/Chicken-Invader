# Kế hoạch 19 (Hoàn thiện Trận chiến Boss Stage 7 & Chuẩn hóa Asset)

Dưới đây là tổng hợp các tính năng và cơ chế cuối cùng đã được hoàn thiện, bỏ qua các bước fix bug trung gian.

## 1. Thiết lập Màn đấu Boss Đặc biệt (Stage 7)
- **Cơ chế Boss Kép (Dual Bosses):** Màn 7 được thiết kế là một trận chiến sinh tử không có Wave/Batch chuyển tiếp. Xuất hiện đồng thời 2 Boss khổng lồ (Military Chicken và Super Chick).
- **Tăng tiến Sức mạnh (Enrage Mechanic):** Tích hợp bộ đếm thời gian (Battle Timer). Cứ mỗi 2 phút trôi qua, tốc độ xả đạn của cả 2 Boss sẽ nhanh hơn (thời gian hồi chiêu giảm 20%), tạo nên một thử thách Bullet Hell dồn dập về cuối.
- **Di chuyển Độc lập & Ngẫu nhiên:** Hai Boss không còn đứng im cạnh nhau mà sẽ liên tục và độc lập trượt đến các vị trí ngẫu nhiên ở nửa trên màn hình.
- **Sát thương & Đạn đạo:** 
  - Điều chỉnh mức sát thương đạn của Boss hợp lý để người chơi có thể sinh tồn: Trứng (50), Lựu đạn (100), Dao (150).
  - Dao của Military Chicken được tinh chỉnh lại thành 1 phi tiêu duy nhất nhắm thẳng vào người chơi.
  - Tốc độ đạn trứng và lựu đạn được làm chậm lại một chút, đồng thời kích thước dao được phóng to gấp đôi.
- **Cơ chế Thăng cấp trong trận:** Boss liên tục thả vật phẩm (Đùi gà) mỗi 5 giây để người chơi nhặt, hồi phục và nâng cấp hỏa lực trong suốt trận chiến.
- **Máu siêu Khủng:** Máu (HP) của Boss được đẩy lên mức 150,000 HP (kèm Giáp 20), thiết kế cho một màn chơi cày cuốc hardcore kéo dài từ 20 đến 30 phút.

## 2. Giao diện & Trải nghiệm (UI/UX)
- **Thanh máu Boss (Boss HUD):** Bổ sung 2 thanh máu độc lập cho từng Boss ở sát mép trên màn hình, đi kèm tên Boss và hiển thị tỷ lệ phần trăm máu chi tiết đến 2 chữ số thập phân (VD: 99.75%).
- **Cải thiện Hitbox (Khung va chạm):** Ghi đè (override) và mở rộng hitbox của Boss lên kích thước siêu lớn (200x200) để khớp với Sprite hiển thị, giúp đạn của người chơi va chạm chính xác tuyệt đối mà không bị "xuyên thấu".
- **Thứ tự Vẽ (Layering):** Tinh chỉnh Layer đồ họa: Đạn và tia Laser của người chơi sẽ được vẽ ở lớp dưới, đè lên trên là hình ảnh của Boss, tạo cảm giác đạn "găm" vào Boss có chiều sâu # Kế hoạch 19 (Hoà
## 3. Đồ họa & Quản lý Asset
- **Nâng cấp Độ phân giải Boss:** Tạo mới lại (Regenerate) toàn bộ sprite animation cho `MilitaryChicken` và `SuperChick_Summer` với độ phân giải cao gấp 3 lần (300x300) để khắc phục triệt để hiện tượng bị mờ/vỡ nét.
- **Chuẩn hóa Định dạng ảnh:** Quét toàn bộ hệ thống asset, chuyển đổi tất cả các file định dạng `.webp` thành `.png` chuẩn để tối ưu hóa tương thích với Raylib, sau đó dọn dẹp toàn bộ file `.webp` rác.
- **Cân bằng Phi Thuyền:** Giữ nguyên các thông số gốc của phi thuyền trong `spaceship.csv`. Chỉ áp dụng cơ chế nhân 3 máu (x3 HP Buff) dành riêng cho người chơi khi bước vào thử thách khốc liệt của Stage 7.
hơi.


> [!IMPORTANT]
> LƯU Ý KHI THIẾT KẾ CÁC WAVE VÀ STAGE MỚI: Tuyệt đối không được sửa đổi, tái cấu trúc (refactor), hay làm ảnh hưởng đến logic của các wave và stage cũ đã được duyệt. Phải luôn đảm bảo tính toàn vẹn của code cũ.
