# Kế hoạch thiết kế tính năng SHOP

## 1. Phần A: Chuyển đổi Egg Assets (webp → png)

### Mục tiêu
Toàn bộ file `egg01.webp` đến `egg20.webp` trong thư mục `assets/egg/` phải được chuyển sang định dạng `.png` để Raylib có thể load trực tiếp không cần thêm thư viện. Sau đó xóa sạch file `.webp` dư thừa.

### Cách thực hiện
Dùng Python PIL/Pillow để batch convert, tương tự cách đã làm với `chicken08.webp` trước đây.

```
Script: convert_eggs.py
Input:  assets/egg/egg01.webp ... egg20.webp
Output: assets/egg/egg01.png  ... egg20.png
Xóa:    Tất cả *.webp trong assets/egg/
```

---

## 2. Phần B: Kiến trúc OOP/Design Pattern cho Shop

### 2.1 Phân tích các class liên quan hiện tại

| Class | Mô tả | Liên kết |
|---|---|---|
| `CoinManager` (Singleton + Observer) | Quản lý tiền tệ, có sẵn `SpendCoins(int)` | **Đã có** |
| `ICoinStrategy` (Strategy) | Interface tính coin từ enemy | **Đã có** |
| `ProgressManager` (Singleton) | Lưu tiến trình màn chơi | **Đã có** |
| `Spaceship::SetWeapon(name)` | Thay vũ khí qua Strategy | **Đã có** |
| `CreateWeaponBehavior(name)` | Factory tạo vũ khí | **Đã có** |
| `GameManager::texEnemyBullet` | Texture trứng đang dùng trong game | **Đã có** |

### 2.2 Design Pattern được sử dụng

#### Singleton – `ShopManager`
- Quản lý dữ liệu toàn cục của Shop (skin nào đã mua, vũ khí nào đã mở khóa, lựa chọn hiện tại).
- Lưu/Load dữ liệu vào file JSON (tương tự `CoinManager`, `ProgressManager`).

#### Strategy (đã có) – Áp dụng cho Weapon
- `Spaceship::SetWeapon(weaponName)` gọi `CreateWeaponBehavior` (Factory) → set `shootingBehavior`.
- Shop chỉ cần gọi `player->SetWeapon("NeutronGun")` là xong, không cần thay đổi logic Spaceship.

#### Template Method / Data Driven – Skin Egg
- `GameManager` hiện load `texEnemyBullet` từ `"assets/egg.png"`.
- Sau khi thêm Shop, `GameManager::Init()` sẽ hỏi `ShopManager::GetSelectedEggSkin()` để biết cần load file nào.
- Mỗi lần game bắt đầu, load đúng skin được chọn → `Bullet.cpp` và `Bosses.cpp` tự nhận texture đúng qua `GetTexEnemyBullet()`.

#### Observer (tùy chọn)
- `ShopManager` có thể Notify `GameManager` khi người dùng đổi skin/vũ khí để reload texture ngay lập tức.

---

## 3. Phần C: Các Class cần tạo mới

### 3.1 `ShopManager` (Singleton)

**File:** `include/ShopManager.h` / `src/ShopManager.cpp`

```cpp
class ShopManager {
    static ShopManager* instance;

    std::set<std::string> unlockedWeapons;   // tên vũ khí đã mua
    std::set<int>         unlockedEggSkins;  // index skin trứng đã mua (1-20)
    std::string           selectedWeapon;    // tên vũ khí được chọn
    int                   selectedEggSkin;   // index skin trứng được chọn (0=default)

    struct ShopItem {
        std::string id;
        std::string name;
        int price;
        std::string previewPath;
    };

    std::vector<ShopItem> weaponItems;
    std::vector<ShopItem> eggItems;

    void Save() const;
    void BuildCatalog();

public:
    static ShopManager* GetInstance();
    static void DestroyInstance();

    void Load();

    bool BuyWeapon(const std::string& weaponId);
    bool BuyEggSkin(int skinIndex);
    bool SelectWeapon(const std::string& weaponId);
    bool SelectEggSkin(int skinIndex);

    std::string GetSelectedWeapon() const;
    int         GetSelectedEggSkin() const;
    std::string GetSelectedEggTexturePath() const;
    bool        IsWeaponUnlocked(const std::string& weaponId) const;
    bool        IsEggSkinUnlocked(int skinIndex) const;

    const std::vector<ShopItem>& GetWeaponItems() const;
    const std::vector<ShopItem>& GetEggItems() const;
};
```

### 3.2 `ShopUI` (thuần UI, quản lý bởi `MenuManager`)

**File:** `include/ShopUI.h` / `src/ShopUI.cpp`

- Không chứa logic nghiệp vụ. Chỉ vẽ + nhận input.
- HAS-A `ShopManager*` để query dữ liệu.
- Giao tiếp 1 chiều: `ShopUI` gọi `ShopManager::BuyWeapon/SelectWeapon`, không ngược lại.

```
ShopUI
├── Tab "WEAPONS" (lưới vũ khí 3 cột)
│   ├── Thẻ mỗi vũ khí: ảnh icon, tên, giá
│   ├── Trạng thái: [Chưa mua → giá coin] | [Đã mua → SELECT] | [Đang dùng → ✓ EQUIPPED]
│   └── Click vào → Mua nếu đủ coin, hoặc Equip nếu đã mua
└── Tab "EGG SKINS" (lưới 4 cột, 5 hàng = 20 skin)
    ├── Preview thumbnail từ assets/egg/egg01.png ... egg20.png
    ├── Trạng thái tương tự như vũ khí
    └── Preview tooltip: hover vào thì hiện to hơn
```

---

## 4. Phần D: Chỉnh sửa Code hiện tại

### 4.1 `GameManager::Init()` – Load đúng egg skin
```diff
- texEnemyBullet = LoadTexture("assets/egg.png");
+ std::string eggPath = ShopManager::GetInstance()->GetSelectedEggTexturePath();
+ texEnemyBullet = LoadTexture(eggPath.c_str());
```

### 4.2 `GameManager::Init()` – Load vũ khí từ Shop
```cpp
// Sau khi tạo player
std::string savedWeapon = ShopManager::GetInstance()->GetSelectedWeapon();
if (!savedWeapon.empty()) player->SetWeapon(savedWeapon);
```

### 4.3 `MenuManager` – Tích hợp `ShopUI`
- Khi click nút SHOP → `currentState = GameState::SHOP`.
- Trong `GameManager::Draw()` case `SHOP` → gọi `shopUI.Draw()`.
- Trong `GameManager::Update()` case `SHOP` → gọi `shopUI.Update()`.

### 4.4 `GameManager::Cleanup()`
```cpp
ShopManager::GetInstance()->Save();
ShopManager::DestroyInstance();
```

---

## 5. Phần E: Catalog dữ liệu Shop

### Danh sách Vũ khí
| ID | Tên hiển thị | Giá (coin) |
|---|---|---|
| `Hypergun` | Hypergun | 0 (free, default) |
| `NeutronGun` | Neutron Gun | 300 |
| `Riddler` | Riddler | 500 |
| `IonBlaster` | Ion Blaster | 700 |
| `UtensilPoker` | Utensil Poker | 900 |
| `PlasmaRifle` | Plasma Rifle | 1200 |
| `LaserCannon` | Laser Cannon | 1500 |
| `SpreadShot` | Spread Shot | 1000 |
| `Beam` | Photon Swarm (Mana) | 2000 |

### Danh sách Egg Skin
| Index | Asset | Giá (coin) |
|---|---|---|
| 0 | `assets/egg.png` | 0 (free, default) |
| 1–5 | `assets/egg/egg01.png` ... `egg05.png` | 100 mỗi skin |
| 6–10 | `assets/egg/egg06.png` ... `egg10.png` | 200 mỗi skin |
| 11–15 | `assets/egg/egg11.png` ... `egg15.png` | 300 mỗi skin |
| 16–20 | `assets/egg/egg16.png` ... `egg20.png` | 500 mỗi skin |

---

## 6. Phần F: File lưu trữ persistent

**Đường dẫn:** `saves/shop.json`

```json
{
  "selectedWeapon": "Hypergun",
  "selectedEggSkin": 0,
  "unlockedWeapons": ["Hypergun", "NeutronGun"],
  "unlockedEggSkins": [0, 1, 3]
}
```

---

## 7. Thứ tự thực hiện (Implementation Order)

```
[1]  Convert egg webp → png (Python script, xóa webp sau khi xong)
[2]  Tạo include/ShopManager.h
[3]  Tạo src/ShopManager.cpp (Load/Save, BuyWeapon, BuyEggSkin, Select...)
[4]  Thêm GameState::SHOP vào enum GameState trong GameManager.h
[5]  Tích hợp ShopManager vào GameManager::Init() và Cleanup()
[6]  Chỉnh GameManager::Init() load texEnemyBullet đúng skin
[7]  Chỉnh GameManager: load vũ khí từ ShopManager sau khi tạo player
[8]  Tạo include/ShopUI.h + src/ShopUI.cpp (vẽ tab Weapon + Egg)
[9]  Xử lý case SHOP trong GameManager::Update() và Draw()
[10] Kết nối nút SHOP trên MenuManager → ChangeState(SHOP)
[11] Thêm ShopManager/ShopUI vào Makefile
[12] Test đầy đủ (mua, chọn skin, vào game, verify)
```

---

## 8. Verification Plan

- Mua skin trứng → vào game → gà rớt trứng đúng loại skin.
- Mua vũ khí → vào game → tàu bắn đúng loại súng đó.
- Không đủ coin → hiện thông báo "Not enough coins".
- Đã mua → chỉ hiện nút SELECT (không cho mua lại).
- Đang trang bị → hiện "EQUIPPED".
- Dữ liệu mua sắm được lưu sau khi thoát game và khôi phục khi mở lại.
- Tổng coin hiển thị chính xác và trừ đúng sau khi mua.
