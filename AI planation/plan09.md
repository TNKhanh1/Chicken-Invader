# Kế hoạch thực thi (Plan 09)

Mục tiêu: Tái cấu trúc mã nguồn (tách file `.h` và `.cpp` riêng biệt), đọc chỉ số từ file CSV, và cài đặt hệ thống đạn Hypergun đa cấp độ sử dụng thông số từ JSON.

## User Review Required

> [!IMPORTANT]
> - Việc tách toàn bộ các class ra các file `.h` và `.cpp` riêng biệt sẽ thay đổi cấu trúc của thư mục `include` và `src` rất nhiều, đồng thời phải cập nhật lại `Makefile`.
> - Để xử lý file JSON cho cấu hình tàu (`hypergun.json`), thư viện tiêu chuẩn của C++ không hỗ trợ sẵn. Tôi dự định sẽ tải thư viện single-header `json.hpp` của nlohmann (rất phổ biến cho C++) vào thư mục `include/` để đọc cấu hình một cách chuẩn mực và tuân thủ OOP. Bạn có đồng ý với việc thêm thư viện này không?

## Open Questions

> [!WARNING]
> - Trong file CSV, level bắt đầu từ 1 đến 11, nhưng trong mô tả súng Hypergun có nhắc đến **Level 0**. Tôi sẽ ngầm hiểu Level 0 tương đương với Level 1 trong hệ thống, hoặc gán một mức mặc định nếu tàu ở cấp độ chưa xác định.
> - Cấu trúc `IShootingBehavior` hiện tại hàm `Shoot(Vector2 position, float damage = 10.0f)` không có thông tin về cấp độ (level) của tàu để thay đổi pattern đạn. Tôi sẽ cần thay đổi Interface này thành `Shoot(Spaceship* ship)` hoặc bổ sung thuộc tính level vào Behavior.

## Proposed Changes

---

### 1. Refactoring (Tách file .h và .cpp)
Tất cả các file header hiện tại chứa cả định nghĩa class và phần triển khai (implementation). Tôi sẽ tách chúng ra:

#### [MODIFY] `include/` và `src/`
- **Các file cơ bản**:
  - `GameObject.h` / `GameObject.cpp`
  - `Character.h` / `Character.cpp`
  - `Spaceship.h` / `Spaceship.cpp`
  - `Enemy.h` / `Enemy.cpp`
  - `Bullet.h` / `Bullet.cpp`
- **Chia nhỏ các file gộp**:
  - `Item.h` -> `Item.h/cpp` và `Meat.h/cpp`
  - `Observer.h` -> `Observer.h/cpp`
  - `UIManager.h` -> `UIManager.h/cpp`
  - `WeaponStrategy.h` -> Chứa interface. Triển khai vào `SingleShot.h/cpp` và `SpreadShot.h/cpp`
  - `MovementStrategy.h` -> Tách ra các class movement riêng lẻ.
  - `Upgrades.h` -> Tách ra các decorator riêng biệt.
  - `SpaceshipFactory.h` / `EnemyFactory.h` -> Tách phần thân hàm vào `.cpp`.
- **Cập nhật `Makefile`**: Thay đổi script build để tự động tìm và biên dịch tất cả các file `.cpp` trong thư mục `src`.

---

### 2. Xây dựng đối tượng đọc file CSV (OOP)
Xây dựng một hệ thống quản lý dữ liệu gốc cho phi thuyền.

#### [NEW] `include/SpaceshipDataManager.h` và `src/SpaceshipDataManager.cpp`
- Tạo class `SpaceshipDataManager` theo Singleton pattern.
- Đọc file `assets/spaceship/spaceship.csv`.
- Lưu trữ dữ liệu vào một cấu trúc Map: `std::map<std::pair<std::string, int>, SpaceshipStats>`.
- Cung cấp hàm `GetStats(string name, int level)` để lấy chỉ số gốc.
- **Cập nhật**: `SpaceshipFactory` sẽ dùng class này để nạp máu, sát thương, tốc bắn,... khi tạo tàu.

---

### 3. Cài đặt hệ thống tàu Hypergun
Hypergun có hệ thống weapon pods phức tạp với nhiều vị trí bắn (tip, front, rear, wings) và các level bắn khác nhau.

#### [NEW] `assets/spaceship/hypergun.json`
- Tạo file JSON cấu hình cho Hypergun chứa đường dẫn ảnh và tọa độ các nòng súng:
  ```json
  {
    "textures": {
      "ship": "assets/spaceship/hypergun_spaceship.jpg",
      "bullet_strong": "assets/spaceship/hypergun_strong.webp",
      "bullet_weak": "assets/spaceship/hypergun_weak.webp"
    },
    "pods": {
      "tip": {"x": 0, "y": -30},
      "front_left": {"x": -20, "y": -10},
      "front_right": {"x": 20, "y": -10},
      "rear_left": {"x": -30, "y": 20},
      "rear_right": {"x": 30, "y": 20},
      "wing_left": {"x": -50, "y": 10},
      "wing_right": {"x": 50, "y": 10}
    }
  }
  ```

#### [NEW] `include/HypergunShootingBehavior.h` và `src/HypergunShootingBehavior.cpp`
- Implement class `HypergunShootingBehavior` kế thừa `IShootingBehavior`.
- Nạp cấu trúc tọa độ từ `hypergun.json`.
- Viết logic tạo đạn dựa vào Level theo yêu cầu:
  - Áp dụng tốc độ bắn giảm dần (2x slower, 4x slower) thông qua biến đếm (timer) độc lập cho các weapon pods phía sau (rear pods) ở các level cao (5 và 9).
  - Khởi tạo góc bắn và độ delay phù hợp cho từng viên đạn Strong/Weak theo đặc tả của bạn.

## Verification Plan

### Automated Tests
- Kiểm tra `make` chạy thành công không báo lỗi linker do trùng lặp hoặc thiếu định nghĩa.
- Đảm bảo game khởi chạy bình thường.

### Manual Verification
- Cần người dùng chạy game và xác nhận rằng hệ thống tải ảnh từ json hoạt động.
- Cần quan sát pattern đạn của tàu Hypergun khi thay đổi Level để đảm bảo logic góc bắn và weapon pods hoạt động chính xác theo mô tả.
