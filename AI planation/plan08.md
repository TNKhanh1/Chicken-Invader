# Plan 08: Chỉnh trang OOP, Bổ sung Cơ chế Mana và Vũ khí mới

Sau khi hoàn thành đợt cập nhật âm thanh và cơ chế EXP ở Plan 07, Plan 08 sẽ tập trung giải quyết triệt để "nợ kỹ thuật" (technical debt) về OOP được phát hiện ở giai đoạn trước, đồng thời bổ sung thêm 2 tính năng cơ bản gọn nhẹ để làm phong phú gameplay. 

## 1. Khắc phục lỗi OOP của Hệ thống Vật phẩm (Item Refactoring)
**Vấn đề:** Hiện tại vật phẩm đùi gà đang được lưu trữ dưới dạng một `struct MeatItem` và bị `GameManager::Update()` can thiệp trực tiếp để thay đổi tọa độ (tính toán trọng lực, dao động ngang). Điều này vi phạm nguyên lý Đóng gói (Encapsulation) và Đa hình (Polymorphism).
**Giải pháp triển khai:**
- **Kế thừa đúng cấu trúc:** Thay thế hoàn toàn `struct MeatItem` bằng cách tạo class `Meat` kế thừa từ `Item` (đã có sẵn trong `Item.h`).
- **Đóng gói Logic (Encapsulation):** Chuyển toàn bộ thuật toán vật lý (rơi tự do, dao động sin) vào hàm `Meat::Update(float deltaTime)`. Chuyển logic vẽ (`DrawTexture`) vào hàm `Meat::Draw()`.
- **Đa hình (Polymorphism):** Cập nhật danh sách lưu trữ trong `GameManager` thành mảng con trỏ đa hình `std::vector<std::shared_ptr<Item>> activeItems`. Vòng lặp chính của game chỉ cần duyệt qua mảng này và gọi `item->Update()` và `item->Draw()` chung cho mọi loại vật phẩm (Đùi gà, Trái tim, Bom, v.v.).

## 2. Hoàn thiện Giao diện Năng lượng (Mana Bar UI)
**Mục tiêu:** Ở những plan đầu tiên, phi thuyền đã được thiết kế cơ chế tích lũy Mana khi bắn đạn (`GainMana()`), nhưng tới nay người chơi vẫn chưa thấy được thông số này trên UI.
**Chi tiết triển khai:**
- Trong cụm code vẽ thanh máu và EXP tại `GameManager::Draw()`, bổ sung thêm một thanh nhỏ thứ 3 (màu xanh lam/tím) dùng để biểu thị Mana.
- Chiều dài của thanh này được tính toán động dựa vào phép chia `GetCurrentMana() / GetMaxMana()`.
- Trực quan hóa đầy đủ bộ 3 trạng thái của phi thuyền: HP - EXP - Mana.

## 3. Khởi tạo Vũ khí mới: Đạn Tỏa (SpreadShot)
**Mục tiêu:** Chứng minh sức mạnh của `Strategy Pattern` (`IShootingBehavior`) bằng cách tạo thêm một loại vũ khí mới mà không làm ảnh hưởng đến mã nguồn của phi thuyền, chuẩn bị nền tảng cho hệ thống cửa hàng nâng cấp vũ khí.
**Chi tiết triển khai:**
- Viết class `SpreadShot` kế thừa từ interface `IShootingBehavior` (đặt trong `WeaponStrategy.h`).
- Trong hàm `Shoot()`: Thay vì tạo ra 1 viên đạn đi thẳng như `SingleShot`, class này sẽ tạo ra 3 viên đạn cùng một lúc:
  - Viên 1: Bay thẳng đứng lên trên.
  - Viên 2: Bay chéo sang góc trên-trái.
  - Viên 3: Bay chéo sang góc trên-phải.
- Áp dụng thử `SpreadShot` cho phi thuyền trong môi trường `TEST_SPACESHIP` để người chơi có thể vào bắn thử ngay lập tức.
