# Giao diện chính (Main Menu) & Stage Selection


## 1. Kiến trúc Hệ thống UI (Đã hoàn thành)
- **Tách biệt Logic (OOP):** Toàn bộ code liên quan đến hiển thị Menu được đóng gói trong class `MenuManager` (`include/MenuManager.h`, `src/MenuManager.cpp`), không còn nằm hỗn loạn trong vòng lặp chính của `GameManager`.
- **Hệ thống lưu tiến trình (ProgressManager):** Sử dụng Design Pattern Singleton.
  - Tự động lưu và tải `highestUnlockedStage` từ file `data/progress.dat`.
  - Hỗ trợ hàm `UnlockStage(int)` để mở khóa màn chơi sau khi chiến thắng.

## 2. Thiết kế Giao diện Chọn màn (Stage Selection)
Giao diện được thiết kế với phong cách **Glassmorphism**, sử dụng thuần Code-driven UI (Raylib) để không phụ thuộc quá nhiều vào asset ảnh tĩnh, giúp UI nhẹ, mượt và hiển thị sắc nét ở mọi kích thước.

### Bố cục (Layout)
- **Lưới Stage 1 đến 6:** Sắp xếp theo tỷ lệ 3 cột x 2 hàng nằm bên trái màn hình. Mỗi ô có kích thước 180x180 px.
- **Stage 7 (Boss Stage):** Đặt độc lập ở phía bên phải, kích thước lớn hơn (220x250 px) để làm nổi bật đây là màn đấu trùm quan trọng.
- **Shop & Settings:** 2 nút nhỏ được đặt ở góc trên bên phải màn hình.

### Hiệu ứng Thị giác (Visual Effects)
- **Trạng thái Khóa (Locked):** Nút hiển thị màu xám tối mờ, có chữ "LOCKED" và không phản hồi khi click.
- **Trạng thái Đã vượt qua (Cleared):** Nút sáng lên, màu nền Glassmorphism xanh dương pha tím, có chữ "CLEARED" màu xanh lá.
- **Trạng thái Boss Stage:** Nút Stage 7 được thiết kế riêng với viền ánh kim Đỏ/Vàng, kèm hiệu ứng nhấp nháy (Pulsing) liên tục để báo hiệu mức độ nguy hiểm.
- **Tương tác (Hover):** Khi người chơi di chuột vào các nút chưa bị khóa, nút sẽ tự động phình to (Scale up) nhẹ và sáng viền lên, kết hợp đổ bóng để tạo cảm giác chạm 3D nổi trên nền vũ trụ.

## 3. Tích hợp & Cập nhật
- Khi màn 7 được chọn, game sẽ bật cờ hiệu `isBossCutscene = true` để load kịch bản xuất hiện của Boss trước trận đấu.
- Bất cứ khi nào đánh bại quái ở đợt cuối cùng hoặc tiêu diệt Boss, hệ thống sẽ tự động gọi `ProgressManager::GetInstance()->UnlockStage(currentStage + 1)` để mở khóa cấp bậc tiếp theo và quay lại Menu.
