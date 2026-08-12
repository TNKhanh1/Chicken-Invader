# Kế hoạch 19 (Hoàn thiện Trận chiến Boss Stage 7 & Hiệu ứng Xuất hiện)

Dưới đây là tổng hợp các tính năng và cơ chế cuối cùng đã được hoàn thiện trong thời gian qua.

## 1. Các tính năng đã hoàn thành (Hoàn thiện Boss Stage 7)
- **Cơ chế Boss Kép (Dual Bosses):** Màn 7 được thiết kế không có Wave/Batch chuyển tiếp, mà xuất hiện trực tiếp đồng thời 2 Boss khổng lồ (Military Chicken và Super Chick).
- **Tăng tiến Sức mạnh (Enrage Mechanic):** Tích hợp bộ đếm thời gian. Cứ mỗi 2 phút trôi qua, tốc độ xả đạn của cả 2 Boss sẽ nhanh hơn (thời gian hồi chiêu giảm 20%), tạo nên thử thách dồn dập về cuối.
- **Di chuyển Độc lập & Ngẫu nhiên:** Hai Boss di chuyển liên tục, độc lập và ngẫu nhiên ở nửa trên màn hình. Có cơ chế đẩy lùi (repulsion) để hai Boss không bao giờ bị đè hình lên nhau.
- **Sát thương & Đạn đạo:** 
  - Dao của Military Chicken được tinh chỉnh lại thành 1 phi tiêu duy nhất nhắm thẳng vào người chơi.
  - Sát thương đạn của Boss được thiết lập hợp lý: Trứng (50), Lựu đạn (100), Dao (150).
- **Thanh máu Boss (Boss HUD):** Bổ sung 2 thanh máu độc lập ở sát mép trên màn hình, đi kèm tên Boss và hiển thị tỷ lệ phần trăm máu chi tiết đến 2 chữ số thập phân (VD: 99.75%). Đã khắc phục lỗi hiển thị nhầm lẫn ID giữa 2 Boss.
- **Thứ tự Vẽ (Layering):** Đạn và tia Laser của người chơi sẽ được vẽ ở lớp dưới, đè lên trên là hình ảnh của Boss, tạo cảm giác đạn "găm" vào Boss có chiều sâu.
- **Animation Tự nhiên:** Chỉnh sửa lại thông số chuyển động (wing amplitude, pivot, leg spread) cho riêng từng Boss để cánh vỗ và chân di chuyển tự nhiên, chính xác. Sprite sheet mới độ phân giải cao 7200x300.
- **Cân bằng Game:** Boss sở hữu 150,000 HP, thiết kế cho trận chiến kéo dài 20-30 phút. Boss liên tục thả đùi gà mỗi 5 giây để người chơi hồi phục.

- **Hiệu ứng xuất hiện Boss & Hội thoại (Cutscene):**
  - **Trạng thái Cutscene (Giới thiệu 8 giây):** Khi bắt đầu Stage 7, 2 Boss sẽ bắt đầu bay từ ngoài màn hình (phía trên) hạ cánh dần xuống vị trí chiến đấu (thấp hơn vị trí gốc 150px để không che khuất hội thoại) trong vòng 2 giây đầu.
  - **Khóa vũ khí và Đòn đánh:** Trong suốt 8 giây Cutscene, người chơi có thể điều khiển tàu di chuyển nhưng hoàn toàn bị khóa chức năng bắn (nút SPACE). Các Boss cũng đứng yên không thể ném đạn, lựu đạn hay dao.
  - **Hội thoại (Dialog) Bong bóng thoại:** Sau khi Boss hạ cánh (từ giây thứ 2 đến giây thứ 8), bong bóng thoại xuất hiện bên cạnh mỗi Boss với chỉ dẫn đuôi thoại (tail) trỏ trực tiếp vào Boss. 
    - Text được hiển thị thông qua hiệu ứng **Typewriter** (hiện từng chữ một) giống như truyện tranh.
    - Cụ thể: Military Chicken: *"You've come far, pilot... but this is your end!"* | Super Chick: *"Prepare to be scrambled!"*
  - **Thanh máu ẩn:** Trong thời gian Cutscene diễn ra, thanh máu của Boss tạm thời ẩn đi để tránh rối mắt và đè lên hội thoại.
  - **Kết thúc Cutscene:** Đúng tại mốc 8 giây, hội thoại kết thúc, các bong bóng biến mất, thanh máu Boss đồng loạt hiện lên. Khóa vũ khí được dỡ bỏ và trận đại chiến chính thức nổ ra.
