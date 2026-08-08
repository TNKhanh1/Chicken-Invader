# Plan Boss 01: Fire Phoenix Boss — Stage 1, Wave 5

## 1. Mục Tiêu

Thêm **Boss Fire Phoenix (chicken09)** vào Stage 1, Wave 5. Boss này sẽ:
- Có **2 mạng (2 phases)** — khi mất hết HP lần 1, boss hồi sinh và chuyển dạng.
- Dạng 1 (Phase 1) có animation phức tạp hơn quái thường, với hiệu ứng lửa mờ nhẹ ở cánh.
- Dạng 2 (Phase 2) có animation phong phú hơn Dạng 1, tấn công thường xuyên hơn, hiệu ứng lửa mạnh hơn.
- Hệ thống tấn công gồm **đánh thường** + **kỹ năng** (sau N lần đánh thường thì dùng skill).
- Kỹ năng có 2 loại: bắn 1 quả đạn lửa to hoặc bắn rất nhiều đạn lửa nhỏ tản ra.

---

## 2. Phân Tích Kiến Trúc Hiện Tại

### 2.1 Hệ thống Boss đã có sẵn
Dự án **đã có sẵn** một hệ thống Boss hoàn chỉnh trong [Bosses.h](file:///e:/ChickenInvader_local/ChickenInvader/include/Bosses.h) và [Bosses.cpp](file:///e:/ChickenInvader_local/ChickenInvader/src/Bosses.cpp):

```
Enemy (Character → GameObject)
  └── Boss (base class cho tất cả boss)
        ├── MilitaryChickenBoss (ném lựu đạn + dao)
        └── SuperChickBoss (bắn trứng vòng tròn)
```

**Class `Boss`** đã cung cấp:
- `Boss::Update()`: Di chuyển ngẫu nhiên hover nửa trên màn hình, wobble effect, thả drumstick mỗi 5s.
- `Boss::Draw()`: Vẽ sprite 350×350 với wobble rotation, sử dụng `texEnemyAnims[visualId-1]`.
- `Boss::GetHitbox()`: Hitbox 200×200.
- `battleTime`: Đếm thời gian chiến đấu (dùng cho enrage scaling).

**Hệ thống đạn boss đã có:**
- `GrenadeBullet` — đạn lựu đạn với texture riêng.
- `KnifeBullet` — đạn dao có góc quay theo hướng bay.
- `EggBullet` — đạn trứng lớn.
- Tất cả kế thừa `Bullet`, override `Draw()`, sử dụng `SetVelocity()`.

### 2.2 Cách Boss hiện tại được spawn
Boss hiện tại chỉ spawn ở **Stage 7** (hardcode trong `GameManager.cpp`):
```cpp
auto mcb = std::make_shared<MilitaryChickenBoss>(11, stats, pos);
auto scb = std::make_shared<SuperChickBoss>(12, stats, pos);
activeEnemies.push_back(mcb);
```
Boss **không đi qua WaveManager/JSON** — được tạo trực tiếp trong GameManager.

### 2.3 Cách quái thường chết
```
Character::TakeDamage() → currentHp <= 0 → Die()
Enemy::Die() → isActive = false → GameManager xóa khỏi activeEnemies
```
**Không có cơ chế hồi sinh.** Cần thêm logic chặn `Die()` ở phase 1 để chuyển sang phase 2.

### 2.4 Asset hiện có
| Asset | Tình trạng |
|---|---|
| `chicken09.png` | ✅ Có — 220×195, gà lửa đỏ |
| `chicken09_anim.png` | ✅ Có — 2400×100, 24 frames (chuẩn 100×100) |
| `fire_bullet.png` | ❌ Chưa có — cần tạo |
| `texEnemyAnims[8]` | ✅ Đã load tự động (index = visualId 9 - 1 = 8) |

---

## 3. Thiết Kế Chi Tiết Boss Fire Phoenix

### 3.1 Hệ Thống 2 Mạng (Phase System)

```cpp
// Thêm vào class FirePhoenixBoss
enum class BossPhase { PHASE_1, TRANSITIONING, PHASE_2 };

BossPhase currentPhase = BossPhase::PHASE_1;
float phase1MaxHp;        // HP tối đa của phase 1
float phase2MaxHp;        // HP tối đa của phase 2
float transitionTimer;    // Timer cho animation chuyển dạng
bool isInvulnerable;      // Bất tử trong lúc chuyển dạng
```

**Flow khi hết HP lần 1:**
1. `Die()` bị override → kiểm tra `currentPhase == PHASE_1`.
2. Nếu đang Phase 1: **KHÔNG chết** → set `currentPhase = TRANSITIONING`.
3. Trong `TRANSITIONING` (khoảng 2 giây):
   - Boss bất tử (`isInvulnerable = true`).
   - Animation đặc biệt: nhấp nháy, tia lửa bùng cháy.
   - Hồi đầy HP phase 2 (`currentHp = phase2MaxHp`, `maxHp = phase2MaxHp`).
4. Sau khi transition xong → `currentPhase = PHASE_2`.
5. Nếu hết HP ở Phase 2 → `Die()` thực sự (gọi `Enemy::Die()`).

### 3.2 Hệ Thống Tấn Công

```
┌─────────────────────────────────────────────────┐
│                  Attack Cycle                    │
│                                                  │
│  [Đánh thường] × N lần → [Kỹ năng] → lặp lại   │
│                                                  │
│  Phase 1: N = 4, cooldown đánh thường = 2.5s     │
│  Phase 2: N = 3, cooldown đánh thường = 1.8s     │
└─────────────────────────────────────────────────┘
```

#### 3.2.1 Đánh Thường (Normal Attack)
- Bắn **3 viên đạn lửa nhỏ** hướng xuống, spread nhẹ (góc -15°, 0°, +15°).
- Phase 2: Bắn **5 viên**, spread rộng hơn (-30°, -15°, 0°, +15°, +30°).

#### 3.2.2 Kỹ Năng (Skill Attack) — xen kẽ 2 loại
- **Skill 1 — Fireball (Đạn lửa to):**
  - Bắn 1 quả đạn lửa LỚN (radius 20, damage cao, speed chậm hơn).
  - Có trail lửa đằng sau (tái sử dụng TrailPoint).
  - Phase 2: Bắn 2 quả cùng lúc.

- **Skill 2 — Fire Rain (Mưa đạn nhỏ):**
  - Bắn 12 viên đạn nhỏ tản ra theo vòng tròn (giống `SuperChickBoss::FireEggBurst()`).
  - Phase 2: Bắn 20 viên, tốc độ nhanh hơn.

- Boss sẽ **xen kẽ** giữa Skill 1 và Skill 2 mỗi lần dùng kỹ năng.

#### 3.2.3 Tracking Variables
```cpp
int normalAttackCount = 0;     // Đếm số lần đánh thường
int normalAttacksBeforeSkill;  // Phase 1: 4, Phase 2: 3
float attackTimer = 0.0f;      // Cooldown giữa các đòn
float attackCooldown;           // Phase 1: 2.5s, Phase 2: 1.8s
int nextSkillType = 0;         // 0: Fireball, 1: Fire Rain (xen kẽ)
```

### 3.3 Animation & Visual Effects

#### 3.3.1 Kích Thước
- Sử dụng `Boss::Draw()` sẵn có: vẽ ở **350×350** (gấp 3.5 lần quái thường).
- Hitbox: **200×200** (đã có trong `Boss::GetHitbox()`).

#### 3.3.2 Hiệu ứng lửa ở cánh (Wing Fire Sparks)
Mỗi frame, spawn các hạt lửa nhỏ ở 2 bên cánh:

```cpp
struct SparkParticle {
    Vector2 pos;
    Vector2 velocity;
    float alpha;     // 1.0 → 0.0
    float size;      // Bắt đầu 3-6px, co dần
    Color color;     // Vàng/Cam/Đỏ ngẫu nhiên
};

std::vector<SparkParticle> sparks;
```

**Phase 1:** Spawn 1-2 spark mỗi 0.1s ở mỗi cánh. Màu vàng/cam nhạt, alpha bắt đầu ~0.4 (mờ mờ).

**Phase 2:** Spawn 3-5 spark mỗi 0.05s. Màu đỏ/cam sáng, alpha bắt đầu ~0.8 (rực rỡ hơn). Kích thước lớn hơn.

**Vị trí spawn cánh** (tương đối so với tâm boss, tính theo destSize 350):
- Cánh trái: `(-120 + random(-15,15), -30 + random(-20,20))`
- Cánh phải: `(+120 + random(-15,15), -30 + random(-20,20))`

**Vẽ trong Draw():**
```cpp
for (auto& s : sparks) {
    DrawCircle(s.pos.x, s.pos.y, s.size, ColorAlpha(s.color, s.alpha));
}
```

#### 3.3.3 Hiệu ứng chuyển dạng (Phase Transition)
- Boss nhấp nháy trắng 5 lần trong 1 giây.
- Vụ nổ tia lửa: spawn 30-50 spark particles bay tứ phía.
- Screen shake nhẹ (±5px trong 0.5s) — xử lý ở `GameManager::Draw()`.
- Sau vụ nổ: boss "tái sinh" với tint color chuyển từ trắng sáng về màu bình thường.

#### 3.3.4 Thanh HP đặc biệt cho Boss
- Thanh HP luôn hiển thị (không cần hover), vẽ phía trên boss.
- Kích thước: rộng 300px, cao 10px.
- Phase 1: Thanh màu xanh lá.
- Phase 2: Thanh màu đỏ/cam (gradient).
- Hiện text "PHASE 1" hoặc "PHASE 2" phía trên thanh HP.

---

## 4. Đạn Lửa (Fire Bullet)

### 4.1 Class `FireBullet : public Bullet`
```cpp
class FireBullet : public Bullet {
private:
    bool isLarge;  // true = Skill 1 (đạn to), false = đạn thường
public:
    FireBullet(Vector2 startPos, Vector2 velocity, bool large = false);
    void Draw() override;
};
```

### 4.2 Visual
- **Đạn nhỏ (normal/skill2):** Vẽ bằng `DrawCircleGradient` — tâm vàng sáng, viền cam đỏ, size 8-10px. Có glow effect nhẹ (vẽ thêm 1 circle lớn hơn với alpha thấp).
- **Đạn to (skill1):** Giống đạn nhỏ nhưng size 20-25px, glow rộng hơn, có trail particles.

> **Không cần file `fire_bullet.png`** — vẽ procedural bằng code sẽ đẹp và linh hoạt hơn (gradient + glow), giống cách asteroid trail đang hoạt động.

---

## 5. Class `FirePhoenixBoss` — Thiết Kế OOP

### 5.1 Kế thừa
```
Enemy
  └── Boss (di chuyển hover, wobble, draw 350×350, hitbox 200×200)
        └── FirePhoenixBoss (2 phases, fire attacks, wing sparks)
```

### 5.2 Header (`Bosses.h` — thêm vào cuối file)
```cpp
class FirePhoenixBoss : public Boss {
public:
    enum class Phase { PHASE_1, TRANSITIONING, PHASE_2 };

private:
    Phase currentPhase = Phase::PHASE_1;
    float phase1MaxHp;
    float phase2MaxHp;
    
    // Transition
    float transitionTimer = 0.0f;
    bool isInvulnerable = false;
    int flashCount = 0;
    
    // Attack system
    float attackTimer = 0.0f;
    int normalAttackCount = 0;
    int normalAttacksBeforeSkill;  // Phase1: 4, Phase2: 3
    float attackCooldown;          // Phase1: 2.5s, Phase2: 1.8s
    int nextSkillType = 0;        // 0: Fireball, 1: Fire Rain
    
    // Wing spark particles
    struct SparkParticle {
        Vector2 pos;
        Vector2 velocity;
        float alpha;
        float size;
        Color color;
    };
    std::vector<SparkParticle> sparks;
    float sparkSpawnTimer = 0.0f;
    
    // Attack methods
    void FireNormalAttack();
    void FireLargeFireball();
    void FireRain();
    void SpawnWingSparks(float deltaTime);
    void UpdateSparks(float deltaTime);
    void EnterPhase2();
    
public:
    FirePhoenixBoss(int visualId, const EnemyStats& stats, Vector2 pos,
                    float p1Hp, float p2Hp);
    
    void Update(float deltaTime) override;
    void Draw() override;
    void Die() override;
    void TakeDamage(float incomingDamage) override;
    
    Phase GetPhase() const { return currentPhase; }
};
```

### 5.3 Luồng xử lý chính

```
FirePhoenixBoss::Update(dt)
├── Boss::Update(dt)  ← di chuyển hover + wobble + drumstick
├── SpawnWingSparks(dt)  ← tạo particle lửa ở cánh
├── UpdateSparks(dt)  ← cập nhật/xóa particle
│
├── if (TRANSITIONING)
│   ├── transitionTimer -= dt
│   ├── Nhấp nháy, burst sparks
│   └── if (done) → EnterPhase2()
│
└── if (PHASE_1 || PHASE_2)
    ├── attackTimer -= dt
    └── if (attackTimer <= 0)
        ├── if (normalAttackCount < N) → FireNormalAttack()
        └── else → FireLargeFireball() or FireRain() (xen kẽ)

FirePhoenixBoss::Die()
├── if (PHASE_1) → KHÔNG CHẾT, chuyển sang TRANSITIONING
└── if (PHASE_2) → Enemy::Die() thực sự
    
FirePhoenixBoss::TakeDamage(dmg)
├── if (isInvulnerable) → return (bỏ qua damage)
└── else → Boss::TakeDamage(dmg) → kế thừa toàn bộ
```

---

## 6. Cập Nhật `stage1.json`

Thêm Wave 5 với 1 batch duy nhất (boss wave):
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
                "type": "GRID",
                "rows": 1,
                "cols": 1,
                "spacing_x": 0,
                "spacing_y": 0,
                "start_y": -200.0,
                "target_base_y": 150.0
            },
            "movement": {
                "type": "HORIZONTAL_BOUNCE",
                "drift": 0.0
            },
            "stats": {
                "hp": 5000,
                "damage": 40,
                "armor": 15,
                "speed": 120,
                "egg_rate": 0.0,
                "score": 1000
            }
        }
    ]
}
```

> **Lưu ý quan trọng:** Boss được spawn qua WaveManager như quái thường, nhưng WaveManager cần nhận biết `role == "BOSS"` để gọi `EnemyFactory::CreateEnemy()` với logic đặc biệt → tạo `FirePhoenixBoss` thay vì `Enemy` thông thường.

---

## 7. Cập Nhật Code Cần Thiết

### 7.1 `EnemyFactory::CreateEnemy()` — Tích hợp Boss Factory
```cpp
std::unique_ptr<Enemy> EnemyFactory::CreateEnemy(
    int visualId, EnemyRole role, const EnemyStats& stats, Vector2 pos
) {
    if (role == EnemyRole::BOSS && visualId == 9) {
        // Fire Phoenix Boss: Phase1 HP = stats.hp, Phase2 HP = stats.hp * 1.5
        return std::make_unique<FirePhoenixBoss>(
            visualId, stats, pos, stats.hp, stats.hp * 1.5f
        );
    }
    // ... các loại boss khác (tương lai) ...
    
    auto enemy = std::make_unique<Enemy>(visualId, role, stats, pos);
    return enemy;
}
```

### 7.2 `WaveManager::SpawnBatch()` — Không cần thay đổi
`SpawnBatch()` đã gọi `EnemyFactory::CreateEnemy()` với đúng `role` và `visualId` từ JSON. Chỉ cần Factory biết tạo đúng class là đủ.

### 7.3 `Enemy::canShoot` — Tắt egg dropping cho Boss
Boss tự quản lý tấn công riêng (không dùng `eggDropTimer`). Trong `WaveManager::SpawnBatch()` hoặc trong constructor `FirePhoenixBoss`, set `canShoot = false`.

### 7.4 `Boss::Update()` — Boss tự di chuyển
Class `Boss` base đã có logic di chuyển hover (random target position, di chuyển 150px/s). Boss **bỏ qua** `movementBehavior` từ JSON vì `Boss::Update()` ghi đè logic di chuyển. Nên `movement.type = "HORIZONTAL_BOUNCE"` trong JSON sẽ bị bỏ qua (hoạt động đúng).

---

## 8. Tóm Tắt Files Cần Thay Đổi

| File | Thay Đổi |
|---|---|
| [Bosses.h](file:///e:/ChickenInvader_local/ChickenInvader/include/Bosses.h) | Thêm class `FireBullet` và `FirePhoenixBoss` |
| [Bosses.cpp](file:///e:/ChickenInvader_local/ChickenInvader/src/Bosses.cpp) | Implement toàn bộ logic boss mới |
| [EnemyFactory.cpp](file:///e:/ChickenInvader_local/ChickenInvader/src/EnemyFactory.cpp) | Thêm điều kiện tạo `FirePhoenixBoss` |
| [stage1.json](file:///e:/ChickenInvader_local/ChickenInvader/data/stage1.json) | Thêm Wave 5 |

**Không cần thay đổi:** `GameManager.cpp/h`, `Enemy.cpp/h`, `WaveManager.cpp/h`, `Bullet.cpp/h`.

---

## 9. Trình Tự Thực Hiện

### Bước 1: Thêm `FireBullet` class
- Thêm khai báo vào `Bosses.h`.
- Implement `Draw()` bằng procedural drawing (gradient circles + glow).

### Bước 2: Thêm `FirePhoenixBoss` class
- Khai báo đầy đủ trong `Bosses.h`.
- Implement trong `Bosses.cpp`:
  - Constructor: set `canShoot = false`, init phase params.
  - `Die()` override: chặn chết ở Phase 1, chuyển sang Transitioning.
  - `TakeDamage()` override: bỏ qua damage khi bất tử.
  - `Update()`: attack cycle, spark particles, phase transition.
  - `Draw()`: gọi `Boss::Draw()` + vẽ sparks + thanh HP + phase indicator.

### Bước 3: Cập nhật `EnemyFactory`
- Thêm include `Bosses.h`.
- Thêm điều kiện `if (role == BOSS && visualId == 9)`.

### Bước 4: Cập nhật `stage1.json`
- Thêm Wave 5 với boss config.

### Bước 5: Build & Test
- Compile, chơi thử Stage 1 Wave 5.
- Cân bằng HP, damage, cooldown, particle density.
- Kiểm tra phase transition, death logic, score.

---

## 10. Thông Số Cân Bằng (Balance)

| Thông số | Phase 1 | Phase 2 |
|---|---|---|
| HP | 5000 | 7500 (×1.5) |
| Armor | 15 | 15 |
| Normal attack count trước skill | 4 | 3 |
| Attack cooldown | 2.5s | 1.8s |
| Normal attack: số đạn | 3 | 5 |
| Skill Fireball: số đạn to | 1 | 2 |
| Skill Fire Rain: số đạn nhỏ | 12 | 20 |
| Di chuyển speed | 150 px/s | 150 px/s (kế thừa Boss base) |
| Spark density (mỗi cánh) | 1-2 / 0.1s | 3-5 / 0.05s |
| Spark alpha | ~0.4 (mờ) | ~0.8 (rực) |
| Score khi chết | 1000 | — |
| Transition time | — | 2.0s |

---

## 11. Design Patterns Sử Dụng

| Pattern | Áp Dụng |
|---|---|
| **Inheritance (OOP)** | `FirePhoenixBoss → Boss → Enemy → Character → GameObject` |
| **Factory Method** | `EnemyFactory::CreateEnemy()` quyết định tạo class nào |
| **Strategy** | `IMovementBehavior` (Boss base tự override di chuyển) |
| **State** | `BossPhase` enum quản lý trạng thái 2 mạng |
| **Template Method** | `Boss::Update()` gọi base rồi subclass extend thêm |
| **Singleton** | `GameManager::GetInstance()` để truy cập textures & add bullets |
