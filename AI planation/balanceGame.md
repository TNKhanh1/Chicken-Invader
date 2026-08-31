# Game Balancing Plan (Chi Tiết – Phiên Bản 2)

> **Quy tắc thực thi plan này:**
> Người thực hiện plan **chỉ quan tâm đến những gì được đề cập trong plan**, tuyệt đối không ảnh hưởng đến logic của những phần khác, **không tự động gộp hay chỉnh sửa logic hoạt động**.

---

## Tổng Quan Phạm Vi Thay Đổi

| File | Loại thay đổi |
|---|---|
| `src/Spaceship.cpp` | Sửa 2 giá trị: `maxExp(100→60)`, `*=1.2f → *=1.15f` |
| `src/GameManager.cpp` | Sửa Meat EXP `10→20`, thêm `SetLevel` cho Stage 6/7 |
| `assets/spaceship/spaceship.csv` | Rebalance toàn bộ 8 weapon × 11 level |
| `data/stage1.json` | Điều chỉnh stats enemy và boss |
| `data/stage2.json` | Điều chỉnh stats enemy và boss |
| `data/stage3.json` | Nâng base stats, sửa phase 4 multiplier |
| `data/stage4.json` | Nâng enemy HP ~20%, nâng boss |
| `data/stage5.json` | Rebalance toàn bộ 15 wave — sửa lỗi boss wave 10 > boss wave 15 |
| `data/stage6.json` | Rebalance 5 boss |

---

## Mục 1: EXP / Level Up – `src/Spaceship.cpp` & `src/GameManager.cpp`

### Phân Tích Hiện Tại
- `maxExp` ban đầu = 100, tăng ×1.2 mỗi level → cần ~2080 EXP tổng để lên Lv10.
- Mỗi kill Stage 1 (score=10) → 5 EXP. Cả Stage 1 ~38 enemy/wave × 4 wave = ~152 kill → 760 EXP → chưa đủ lên Lv3.
- Meat EXP = 10 (quá thấp).

### Thay Đổi

**File `src/Spaceship.cpp` – Constructor (line 12):**
```diff
- level(1), currentExp(0), maxExp(100), name(n)
+ level(1), currentExp(0), maxExp(60), name(n)
```

**File `src/Spaceship.cpp` – `LevelUp()` (line 275):**
```diff
- maxExp *= 1.2f;
+ maxExp *= 1.15f;
```

**File `src/GameManager.cpp` – Meat pickup (~line 1154):**
```diff
- whoPicked->GainExp(10.0f);
+ whoPicked->GainExp(20.0f);
```

### Kết Quả Sau Sửa
| Level | EXP cần | Cộng dồn |
|---|---|---|
| 1→2 | 60 | 60 |
| 2→3 | 69 | 129 |
| 3→4 | 79 | 208 |
| 4→5 | 91 | 299 |
| 5→6 | 105 | 404 |
| 6→7 | 121 | 525 |
| 7→8 | 139 | 664 |
| 8→9 | 160 | 824 |
| 9→10 | 184 | **~1008 tổng** |

Stage 1 ~760 EXP → lên khoảng Lv5-6. Stage 5 (15 wave) đủ để đạt Lv10. Hợp lý!

---

## Mục 2: Starting Level Stage 6/7 – `src/GameManager.cpp`

**Theo yêu cầu:** Stage 6 bắt đầu Lv7, Stage 7 bắt đầu Lv10.

Vị trí chèn: trong `GameManager::StartStage()`, **sau dòng** `RuneManager::GetInstance()->ApplyAll(...)` (hiện tại line ~403), **trước dòng** `pendingEggSkinLoad = true`:

```diff
  RuneManager::GetInstance()->ApplyAll(player.get(), player2 ? player2.get() : nullptr);

+ // Set starting level for boss-only stages (no leveling possible)
+ if (currentStage == 6) {
+     player->SetLevel(7);
+     player->ReloadStatsFromCSV();
+     if (player2) { player2->SetLevel(7); player2->ReloadStatsFromCSV(); }
+ }
+ if (currentStage == 7) {
+     player->SetLevel(10);
+     player->ReloadStatsFromCSV();
+     if (player2) { player2->SetLevel(10); player2->ReloadStatsFromCSV(); }
+ }

  pendingEggSkinLoad = true;
```

> **Lưu ý:** Gọi `ReloadStatsFromCSV()` sau `SetLevel()` để đảm bảo HP/Damage được cập nhật đúng theo level mới. Method `ReloadStatsFromCSV()` đã có sẵn trong `Spaceship` (được gọi trong `LevelUp()`), không cần tạo mới.

---

## Mục 3: CSV Weapon Rebalance – `assets/spaceship/spaceship.csv`

### Nguyên Tắc
1. Vũ khí đắt hơn → DPS tổng thể cao hơn.
2. Mỗi vũ khí có cá tính riêng (tank, crit, speed, burst).
3. Mỗi level tăng ~50-70% stat so với Lv1 ở Lv11.
4. AttackSpeed = cooldown (số nhỏ = bắn nhanh hơn).

### Toàn Bộ File CSV Mới (thay thế hoàn toàn nội dung file)

```
Spaceship_Name,Level,HP,Damage,Armor,AttackSpeed,MoveSpeed,MaxMana,CritChance(%),CritDamage(%)
Hypergun,1,800,14,10,5.0,330,100,5,150
Hypergun,2,880,15,11,5.1,336,105,6,155
Hypergun,3,960,16,13,5.2,342,110,7,160
Hypergun,4,1040,18,14,5.3,348,115,8,165
Hypergun,5,1120,19,15,5.4,354,120,9,170
Hypergun,6,1200,20,16,5.5,360,125,10,175
Hypergun,7,1280,21,17,5.6,366,130,11,180
Hypergun,8,1360,22,18,5.7,372,135,12,185
Hypergun,9,1440,23,19,5.8,378,140,13,188
Hypergun,10,1520,24,20,5.9,384,145,14,190
Hypergun,11,1600,26,21,6.0,390,150,15,195
Neutron_Gun,1,1100,26,20,2.2,260,90,10,180
Neutron_Gun,2,1210,29,22,2.3,264,95,11,186
Neutron_Gun,3,1320,32,24,2.4,268,100,12,192
Neutron_Gun,4,1430,34,26,2.5,272,105,13,198
Neutron_Gun,5,1540,36,28,2.6,276,110,14,204
Neutron_Gun,6,1650,38,29,2.7,280,115,15,210
Neutron_Gun,7,1760,40,31,2.8,284,120,16,216
Neutron_Gun,8,1870,42,33,2.8,288,125,17,222
Neutron_Gun,9,1980,43,34,2.9,292,130,18,228
Neutron_Gun,10,2090,44,35,2.9,296,135,19,234
Neutron_Gun,11,2200,46,36,3.0,300,140,20,240
Riddler,1,750,18,8,3.8,360,85,15,150
Riddler,2,820,20,9,3.9,366,90,16,155
Riddler,3,890,22,10,4.0,372,95,17,160
Riddler,4,960,23,11,4.1,378,100,18,165
Riddler,5,1030,24,12,4.2,384,105,19,170
Riddler,6,1100,26,13,4.3,390,110,20,175
Riddler,7,1170,27,14,4.4,396,115,21,180
Riddler,8,1240,28,15,4.5,402,120,22,185
Riddler,9,1310,29,15,4.7,408,125,24,190
Riddler,10,1380,30,16,4.8,414,130,26,195
Riddler,11,1450,32,17,5.0,420,135,28,200
Ion_Blaster,1,950,20,15,5.2,340,100,8,160
Ion_Blaster,2,1040,22,17,5.3,346,105,9,165
Ion_Blaster,3,1130,23,18,5.4,352,110,10,170
Ion_Blaster,4,1220,24,19,5.4,358,115,11,175
Ion_Blaster,5,1310,26,20,5.5,364,120,12,180
Ion_Blaster,6,1400,27,22,5.6,370,125,13,185
Ion_Blaster,7,1490,28,23,5.7,376,130,14,190
Ion_Blaster,8,1580,30,24,5.8,382,135,15,195
Ion_Blaster,9,1670,31,26,5.9,388,140,16,200
Ion_Blaster,10,1760,33,27,6.0,394,145,17,205
Ion_Blaster,11,1850,35,28,6.2,400,150,18,210
Utensil_Poker,1,1200,26,22,3.0,275,105,10,175
Utensil_Poker,2,1320,28,24,3.1,280,110,11,182
Utensil_Poker,3,1440,30,26,3.2,285,115,12,189
Utensil_Poker,4,1560,32,28,3.3,290,120,13,196
Utensil_Poker,5,1680,34,30,3.4,295,125,14,203
Utensil_Poker,6,1800,36,32,3.5,300,130,15,210
Utensil_Poker,7,1920,38,34,3.6,305,135,16,217
Utensil_Poker,8,2040,40,36,3.7,310,140,17,224
Utensil_Poker,9,2160,42,37,3.8,316,150,18,229
Utensil_Poker,10,2300,43,38,3.9,322,158,19,234
Utensil_Poker,11,2450,46,40,4.0,330,165,20,240
Lightning_Fryer,1,850,24,14,3.3,340,115,14,185
Lightning_Fryer,2,930,26,15,3.4,346,121,15,191
Lightning_Fryer,3,1010,28,16,3.5,352,127,16,197
Lightning_Fryer,4,1090,30,18,3.6,358,133,17,203
Lightning_Fryer,5,1170,32,19,3.7,364,139,18,209
Lightning_Fryer,6,1250,34,20,3.8,370,145,19,215
Lightning_Fryer,7,1330,36,22,3.9,376,151,20,221
Lightning_Fryer,8,1410,38,23,4.0,382,157,21,228
Lightning_Fryer,9,1490,39,24,4.1,388,163,23,235
Lightning_Fryer,10,1570,41,25,4.2,394,169,25,242
Lightning_Fryer,11,1650,44,27,4.3,400,176,27,250
Plasma_Rifle,1,950,22,14,5.5,350,105,10,165
Plasma_Rifle,2,1040,24,15,5.6,356,110,11,170
Plasma_Rifle,3,1130,26,17,5.7,362,115,12,175
Plasma_Rifle,4,1220,27,18,5.8,368,120,13,180
Plasma_Rifle,5,1310,29,19,5.9,374,125,14,185
Plasma_Rifle,6,1400,31,20,6.0,380,130,15,190
Plasma_Rifle,7,1490,32,22,6.1,386,135,16,195
Plasma_Rifle,8,1580,34,23,6.2,392,140,17,200
Plasma_Rifle,9,1670,35,24,6.3,398,148,18,207
Plasma_Rifle,10,1760,37,25,6.5,406,155,19,213
Plasma_Rifle,11,1850,40,27,6.7,415,162,20,220
Laser_Cannon,1,1100,32,22,2.3,285,125,12,215
Laser_Cannon,2,1210,35,24,2.4,290,133,13,222
Laser_Cannon,3,1320,38,26,2.5,295,141,14,229
Laser_Cannon,4,1430,40,28,2.6,300,149,15,236
Laser_Cannon,5,1540,43,30,2.7,305,157,16,243
Laser_Cannon,6,1650,46,32,2.8,310,165,17,250
Laser_Cannon,7,1760,49,34,2.9,315,173,18,257
Laser_Cannon,8,1870,52,36,3.0,320,181,19,264
Laser_Cannon,9,1980,54,38,3.1,326,189,20,271
Laser_Cannon,10,2090,56,40,3.2,332,197,21,278
Laser_Cannon,11,2200,60,42,3.3,340,207,22,290
```

### So Sánh Điểm Mạnh Riêng Từng Weapon
| Vũ khí | Điểm mạnh |
|---|---|
| **Hypergun** (0c) | All-round, tiêu chuẩn, dễ chơi |
| **Neutron_Gun** (300c) | HP cao nhất, Damage burst cao, slow fire rate |
| **Riddler** (500c) | Crit chance/damage tốt, tốc độ di chuyển nhanh |
| **Ion_Blaster** (700c) | Tốc độ bắn tốt, cân bằng |
| **Utensil_Poker** (900c) | HP + Armor cao nhất nhóm giữa, tank |
| **Lightning_Fryer** (1000c) | Crit damage + Damage mạnh, Mana cao |
| **Plasma_Rifle** (1200c) | Tốc độ bắn cao nhất, tốc độ di chuyển tốt |
| **Laser_Cannon** (1500c) | Damage/phát cao nhất, CritDmg cao nhất |

---

## Mục 4: Stage 1 – `data/stage1.json`

**Kỳ vọng player:** Lv1 → Lv3-4 sau Stage 1.
**Player Lv1 Hypergun (mới):** HP=800, Dmg=14, Armor=10.

Chỉ thay đổi giá trị trong các block `"stats": {...}` — **không thay đổi layout, movement, count, visual_id**.

| Wave | Batch | Role | HP | Damage | Armor | Egg Rate | Score |
|---|---|---|---|---|---|---|---|
| W1 | B1 (×15) | NORMAL | 120 | 22 | 0 | 2.5 | 10 |
| W1 | B2 (×15) | NORMAL | 120 | 22 | 0 | 2.5 | 10 |
| W1 | B3 (×8) | NORMAL | 130 | 22 | 0 | 2.5 | 12 |
| W2 | B1 (×11) | NORMAL | 130 | 25 | 0 | 2.5 | 12 |
| W2 | B2 (×10) | SWARM | 65 | 12 | 0 | 8.0 | 15 |
| W3 | B1 (×30) | ASTEROID | 600 | 55 | 0 | 0 | 5 |
| W3 | B2 (×30) | ASTEROID | 600 | 55 | 0 | 0 | 5 |
| W4 | B1 (×15) | NORMAL | 150 | 25 | 0 | 2.0 | 12 |
| W4 | B2 (×10) | TANK | 1200 | 55 | 12 | 1.2 | 120 |
| W4 | B3/4/5 (×1) | ASTEROID | 600 | 55 | 0 | 0 | 5 |
| **W5** | **B1 (×1)** | **BOSS** | **7000** | **45** | **18** | **0** | **1000** |

---

## Mục 5: Stage 2 – `data/stage2.json`

**Kỳ vọng player:** Lv3 → Lv6-7 sau Stage 2.
**Player Lv3-4 Hypergun:** HP≈960-1040, Dmg≈16-18.

Chỉ thay đổi giá trị trong `"stats": {...}`.

| Wave | Batch | Role | HP | Damage | Armor | Egg Rate | Score |
|---|---|---|---|---|---|---|---|
| W1 | B1 (×10) | NORMAL | 160 | 22 | 0 | 2.5 | 15 |
| W1 | B2 (×15) | NORMAL | 160 | 22 | 0 | 2.0 | 15 |
| W1 | B3 (×10) | SWARM | 80 | 14 | 0 | 8.0 | 10 |
| W2 | B1 (×16) | NORMAL | 190 | 26 | 0 | 2.0 | 20 |
| W2 | B2 (×8) | NORMAL | 200 | 26 | 0 | 1.8 | 20 |
| W2 | B3 (×10) | NORMAL | 180 | 26 | 0 | 2.0 | 20 |
| W3 | B1 (×25) | ASTEROID | 600 | 55 | 0 | 0 | 5 |
| W3 | B1b (×25) | ASTEROID | 600 | 60 | 0 | 0 | 5 |
| W3 | B2 (×20) | NORMAL | 210 | 28 | 0 | 2.0 | 22 |
| W4 | B1 (×12) | NORMAL | 260 | 32 | 5 | 1.5 | 30 |
| W4 | B2 (×12) | TANK | 1400 | 55 | 12 | 0.8 | 140 |
| W4 | B2 ast×12 ×3 | ASTEROID | 600 | 60 | 0 | 0 | 5 |
| W4 | B3 (×10) | NORMAL | 210 | 28 | 0 | 2.0 | 25 |
| **W5** | **B1 (×1)** | **BOSS** | **45000** | **55** | **18** | **0** | **5000** |
| W6 | B1 (×11) | NORMAL | 200 | 28 | 0 | 2.0 | 20 |
| W6 | B2 (×10) | NORMAL | 200 | 32 | 0 | 1.5 | 25 |
| W6 | B3 (×8) | NORMAL | 180 | 32 | 0 | 1.0 | 25 |
| W7 | B1 (×9) | NORMAL | 210 | 32 | 0 | 2.0 | 25 |
| W7 | B2 (×21) | NORMAL | 190 | 32 | 0 | 2.0 | 25 |
| W8 | B1 (×12) | NORMAL | 260 | 32 | 0 | 2.5 | 30 |
| W8 | B2 (×11) | NORMAL | 220 | 36 | 0 | 2.5 | 30 |
| W9 | B1 (×15) | NORMAL | 110 | 22 | 0 | 1.0 | 15 |
| W9 | B2 (×12) | NORMAL | 230 | 36 | 0 | 3.5 | 40 |
| **W10** | **B1 (×2)** | **BOSS** | **35000** | **55** | **18** | **0** | **5000** |

> **Ghi chú W5 Boss:** HP tăng từ 40000 → 45000 (boss wave 5 của stage 2, đây là boss thứ nhất player gặp có lv cao).
> **W10 Boss ×2:** Mỗi con HP=35000 (từ 30000), phù hợp player Lv6-7.

---

## Mục 6: Stage 3 – `data/stage3.json`

**Theo yêu cầu:** Phase 4 multiplier = **3.2** (không phải 3.5).
**Kỳ vọng player:** Lv4-6. Đây là stage đặc biệt (Waterfall), enemy rất yếu ở phase đầu, mạnh dần.

Thay đổi hoàn toàn block `stats` và `phases` trong wave_id 1:

```json
"stats": {
    "hp": 80,
    "damage": 22,
    "armor": 0,
    "speed": 100,
    "egg_rate": 4.0,
    "score": 12
},
"layout": {
    "type": "CONTINUOUS_STREAM",
    "target_kills": 200,
    "spawn_interval": 0.6,
    "phases": [
        { "spawn_count_threshold": 0,   "hp_multiplier": 1.0, "speed_multiplier": 1.0 },
        { "spawn_count_threshold": 50,  "hp_multiplier": 1.5, "speed_multiplier": 1.15 },
        { "spawn_count_threshold": 100, "hp_multiplier": 2.2, "speed_multiplier": 1.3 },
        { "spawn_count_threshold": 150, "hp_multiplier": 3.2, "speed_multiplier": 1.5 }
    ]
}
```

---

## Mục 7: Stage 4 – `data/stage4.json`

**Kỳ vọng player:** Lv5-8. Stage 4 có 10 wave với nhiều enemy type phức tạp.

Chỉ thay đổi `"stats": {...}`, không thay đổi cấu trúc layout/movement/count/visual_id.

**Quy tắc áp dụng:** Tăng HP tất cả enemy NORMAL ~20%, TANK ~15%, Boss nhân 1.65×.

| Wave | Batch | Role | HP (cũ → mới) | Damage (cũ → mới) | Armor | Score |
|---|---|---|---|---|---|---|
| W1 | B1 NORMAL×15 | NORMAL | 250→300 | 20→24 | 0 | 10 |
| W1 | B2 NORMAL×14 | NORMAL | 200→240 | 10→14 | 0 | 10 |
| W1 | B3 NORMAL×7 | NORMAL | 300→360 | 20→24 | 0 | 15 |
| W2 | B1 ASTEROID×10 | ASTEROID | 800 (giữ) | 40→45 | 0 | 25 |
| W2 | B1b NORMAL×8 | NORMAL | 300→360 | 20→24 | 0 | 15 |
| W2 | B2 NORMAL×5 | NORMAL | 350→420 | 20→24 | 0 | 20 |
| W3 | B1 NORMAL×12 | NORMAL | 300→360 | 20→24 | 0 | 15 |
| W3 | B2 NORMAL×7 | NORMAL | *giữ nguyên các wave 3-7 tương tự, +20% HP* | | | |
| ... | ... | ... | *Tất cả NORMAL +20% HP, damage +4-5* | | | |
| W8 | TANK×2 scale=2.0 | NORMAL | 1500→1800 | 40→48 | 0 | 300 |
| W8 | NORMAL×12 | NORMAL | 250→300 | 20→24 | 0 | 30 |
| W9 | NORMAL×15×3 batches | NORMAL | 300-400→360-480 | 20-30→24-36 | 0 | 30-50 |
| W9 | TANK×6×2 scale=1.5 | NORMAL | 800→960 | 50→58 | 0 | 100 |
| **W10** | **BOSS visual_id=8** | **BOSS** | **15000→25000** | **50→60** | **25→30** | **5000** |

---

## Mục 8: Stage 5 – `data/stage5.json` (**Quan Trọng – Sửa Lỗi**)

### Vấn Đề Hiện Tại
- Wave 10 Boss (visual_id=7): HP=**5000**, damage=40 → **Đây là boss đầu tiên của stage 5**
- Wave 11-14: Enemy thường HP=50-300 → **yếu hơn boss wave 10!** ❌
- Wave 15 Boss cuối (visual_id=14): HP=**2000**, damage=50 → **Boss cuối yếu hơn boss giữa!** ❌

### Cấu Trúc Đúng (Wave Scaling)
Wave 1-9: Enemy thường, tăng dần.
Wave 10: Boss giữa (mid-boss) → **Phải mạnh nhất trong giai đoạn 1**.
Wave 11-14: Enemy thường tiếp tục, **MẠNH HƠN wave 1-9** (đây là giai đoạn 2 sau khi đánh mid-boss).
Wave 15: Boss cuối → **Mạnh nhất cả stage**, luôn mạnh hơn boss wave 10.

### Bảng Thay Đổi Đầy Đủ (chỉ sửa `"stats": {...}`)

**Wave 1-9 (Enemy thường):**
| Wave | Batch | Role | HP mới | Damage mới | Armor mới | Egg Rate | Score |
|---|---|---|---|---|---|---|---|
| W1 | B1 NORMAL×8 | NORMAL | 280 | 32 | 5 | 2.5 | 30 |
| W1 | B2 NORMAL×8 | NORMAL | 280 | 32 | 5 | 2.5 | 30 |
| W1 | B3 NORMAL×8 | NORMAL | 160 | 22 | 0 | 3.0 | 12 |
| W2 | B1 NORMAL×16 | NORMAL | 200 | 28 | 0 | 2.5 | 22 |
| W2 | B2 NORMAL×6 | NORMAL | 280 | 32 | 5 | 2.5 | 32 |
| W2 | B3 NORMAL×8 | NORMAL | 280 | 32 | 5 | 2.5 | 32 |
| W3 | B1 ASTEROID×60 | ASTEROID | 600 | 55 | 0 | 0 | 5 |
| W3 | B2 NORMAL×10 | NORMAL | 280 | 32 | 5 | 2.5 | 32 |
| W4 | B1 NORMAL×10 | NORMAL | 260 | 28 | 0 | 2.8 | 28 |
| W4 | B1b NORMAL×5 | NORMAL | 280 | 32 | 5 | 2.5 | 32 |
| W4 | B2 TANK×4 | TANK | 1500 | 45 | 12 | 2.0 | 120 |
| W4 | B3 NORMAL×15 | NORMAL | 280 | 32 | 5 | 2.5 | 32 |
| W5 | B1 NORMAL×10 | NORMAL | 260 | 28 | 0 | 2.8 | 28 |
| W5 | B2 NORMAL×16 | NORMAL | 280 | 32 | 5 | 2.5 | 32 |
| W6 | B1 TANK×8 | TANK | 1800 | 48 | 12 | 2.0 | 120 |
| W6 | B1b NORMAL×8 | NORMAL | 200 | 28 | 0 | 2.5 | 22 |
| W6 | B2 NORMAL×12 | NORMAL | 300 | 34 | 5 | 2.5 | 35 |
| W7 | B1 NORMAL×5 | NORMAL | 350 | 38 | 8 | 2.5 | 45 |
| W7 | B2 NORMAL×15 | NORMAL | 170 | 24 | 0 | 3.0 | 12 |
| W8 | B1 NORMAL×4 | NORMAL | 300 | 34 | 5 | 2.5 | 35 |
| W8 | B1b NORMAL×4 | NORMAL | 360 | 40 | 8 | 2.5 | 48 |
| W8 | B2 ASTEROID×15 | ASTEROID | 600 | 55 | 0 | 0 | 5 |
| W9 | B1 NORMAL×16 | NORMAL | 360 | 40 | 8 | 2.5 | 45 |
| W9 | B2 TANK×10 | TANK | 2000 | 50 | 15 | 1.8 | 150 |

**Wave 10 (Mid-Boss):**
| Wave | Batch | Role | HP mới | Damage mới | Armor | Score |
|---|---|---|---|---|---|---|
| **W10** | **B1 BOSS×1 (visual_id=7)** | **BOSS** | **18000** | **55** | **20** | **3000** |

**Wave 11-14 (Giai Đoạn 2 – PHẢI mạnh hơn wave 1-9):**
| Wave | Batch | Role | HP mới | Damage mới | Armor mới | Egg Rate | Score |
|---|---|---|---|---|---|---|---|
| W11 | B1 NORMAL×12 | NORMAL | 380 | 42 | 8 | 2.5 | 45 |
| W11 | B2 SWARM×5 | SWARM | 100 | 30 | 0 | 8.0 | 25 |
| W12 | B1 TANK×10 | TANK | 2200 | 55 | 18 | 1.5 | 150 |
| W12 | B2 NORMAL×16 | NORMAL | 380 | 42 | 8 | 2.5 | 45 |
| W13 | B1 NORMAL×12 | NORMAL | 400 | 45 | 10 | 2.5 | 50 |
| W13 | B2 TANK×3 | TANK | 2200 | 55 | 18 | 1.5 | 150 |
| W14 | B1 SWARM×12 | SWARM | 100 | 30 | 0 | 8.0 | 22 |
| W14 | B2 TANK×10 | TANK | 2200 | 55 | 18 | 1.5 | 150 |

**Wave 15 (Final Boss) – Phải mạnh hơn mid-boss wave 10 (18000 HP):**
| Wave | Batch | Role | HP mới | Damage mới | Armor | Egg Rate | Score |
|---|---|---|---|---|---|---|---|
| **W15** | **B1 BOSS×1 (visual_id=14)** | **BOSS** | **35000** | **70** | **35** | **0** | **5000** |

> **Kiểm tra logic:** W15 Boss (35000 HP, armor 35) > W10 Mid-Boss (18000 HP, armor 20). ✅ Đúng thứ tự!

---

## Mục 9: Stage 6 – `data/stage6.json`

**Player bắt đầu Level 7** (theo yêu cầu). Hypergun Lv7: HP=1280, Dmg=21, Armor=17.

| Wave | Boss (visual_id) | HP cũ → mới | Damage cũ → mới | Armor cũ → mới | Speed | Score |
|---|---|---|---|---|---|---|
| **W1** | Boss (visual_id=10) | 5000 → **9000** | 50 | 22 → **24** | 130 | 1000 |
| **W2** | Boss (visual_id=11) | 4000 → **13000** | 40 → **48** | 25 → **28** | 130 | 1000 |
| **W3** | Boss (visual_id=8) | 10000 → **18000** | 60 → **68** | 20 → **24** | 140 | 1500 |
| **W4** | Boss (visual_id=7) | 12000 → **24000** | 60 → **72** | 30 → **34** | 110 | 2000 |
| **W5** | Boss (visual_id=14) | 15000 → **32000** | 80 → **88** | 35 → **40** | 140 | 3000 |

---

## Mục 10: Verification Plan

### Build & Chạy
```bash
make && ./game.exe
```

### Kiểm Tra Chức Năng

| Kiểm tra | Cách kiểm tra | Kết quả kỳ vọng |
|---|---|---|
| EXP lên nhanh hơn | Chơi Stage 1 W1 | Lên Lv2 trong W1-2 |
| Meat EXP đúng | Nhặt thịt rơi ra | Thanh EXP tăng rõ hơn trước |
| Stage 6 bắt đầu Lv7 | Vào Stage 6 → nhìn Stats Panel (Tab) | Level = 7 |
| Stage 7 bắt đầu Lv10 | Vào Stage 7 → nhìn Stats Panel (Tab) | Level = 10 |
| Weapon ordering | So sánh Laser_Cannon(1500c) vs Neutron_Gun(300c) | Laser mạnh hơn rõ ràng |
| Stage 5 W15 boss | Đánh tới wave 15 | Boss khó hơn mid-boss W10 |

### Không Kiểm Tra (Ngoài Phạm Vi)
- State machine, sound, rendering → không bị ảnh hưởng.

Bấm **Proceed** để thực hiện!


---

## M?c 11: Nghi�n C?u R?i Ro & Kh? Nang G�y Bug/Crash (Ph�n T�ch B? Sung)

Tru?c khi ti?n h�nh code, t�i d� d?c v� ki?m tra k? m� ngu?n hi?n t?i c?a game d? d? do�n nh?ng v? tr� c� kh? nang g�y l?i (bug/crash) n?u th?c hi?n sai l?ch plan tr�n. Du?i d�y l� c�c r?i ro c� th? g?p v� c�ch ph�ng tr�nh:

### 1. R?i ro Crash Game do Parse JSON (Nguy co R?T CAO)
- **V?n d?**: File \data/stageX.json\ du?c load th�ng qua thu vi?n nlohmann/json. N?u trong qu� tr�nh ch?nh s?a \stats\ m� b? qu�n d?u ph?y \,\, thi?u ngo?c \}\, ho?c nh?p nh?m ki?u d? li?u (vd: nh?p \\u0022300\u0022\ thay v� \300\), game s? vang (crash) ngay l?p t?c khi kh?i t?o stage d� do \json::parse_error\.
- **C�ch ph�ng tr�nh**: Khi s?a JSON, tu�n th? tuy?t d?i c?u tr�c ban d?u, ch? thay d?i gi� tr? s? (numbers), kh�ng th�m/b?t c�c tru?ng b?t bu?c.

### 2. R?i ro l?i d? li?u CSV \spaceship.csv\ (Nguy co TRUNG B�NH)
- **V?n d?**: �� ki?m tra h�m \SpaceshipDataManager::LoadCSV()\. H�m n�y s? d?ng \std::stof\ v� \std::stoi\. N?u c� k� t? l? (nhu kho?ng tr?ng du th?a, k� t? ch? l?n v�o c?t s?), ho?c thi?u c?t, n� s? n�m exception. Code hi?n t?i c� b?c \	ry...catch\ v� in ra console, b? qua d�ng l?i. Tu\u0079 nhi�n, h�m \GetStats()\ n?u kh�ng t�m th?y level s? tr? v? **Fallback Stats** (HP=100, Damage=10). N?u x?y ra, ngu?i choi s? t? nhi�n b? y?u di d?t ng?t m� game kh�ng crash.
- **C�ch ph�ng tr�nh**: Khi ch�p d� CSV, d?m b?o d�ng 88 d�ng d? li?u (8 vu kh� x 11 c?p d?), d�ng th? t? c?t, ngan c�ch b?ng d?u ph?y, kh�ng c� space th?a.

### 3. R?i ro li�n quan d?n \SetLevel()\ ? Stage 6 \u0026 7 (Nguy co TH?P)
- **V?n d?**: 
    - �� ki?m tra \Spaceship::SetLevel()\: H�m n�y c� co ch? an to�n \if (level \u003C 1) level = 1; if (level \u003E 11) level = 11;\ n�n vi?c set Level 7 hay Level 10 l� an to�n.
    - �� ki?m tra \GameManager::StartStage()\: C� bi?n \player2\ d?ng con tr?. N?u g?i \player2-\u003eSetLevel()\ trong ch? d? Single Player (player2 = nullptr) th� game s? **crash (Access Violation)**.
- **C�ch ph�ng tr�nh**: �?m b?o b?c ki?m tra null pointer: \if (player2) { player2-\u003eSetLevel(...); }\. Trong plan d� ghi r� di?u n�y.

### 4. R?i ro li�n quan d?n ch? s? sau khi SetLevel (Nguy co BUG LOGIC)
- **V?n d?**: H�m \SetLevel()\ ch? g�n bi?n \level\. N?u kh�ng g?i \ReloadStatsFromCSV()\, HP/Damage c?a player v?n gi? nguy�n ? m?c Level cu, g�y l?i m?t c�n b?ng game (d� d� set level 10 nhung HP v?n l� level 1).
- **C�ch ph�ng tr�nh**: Trong plan d� d? c?p: b?t bu?c ph?i g?i \player-\u003eReloadStatsFromCSV()\ ngay sau \SetLevel()\ d? d?ng b? data t? C++ map v�o instance c?a Player.

### 5. R?i ro Infinite Loop (Freeze game) khi GainExp (Nguy co B?NG 0)
- **V?n d?**: H�m \Spaceship::GainExp()\ s? d?ng v�ng l?p \while (currentExp \u003E= maxExp) { LevelUp(); }\. N?u \maxExp\ v� t�nh b? set b?ng 0 ho?c s? �m, v�ng l?p n�y s? ch?y m�i m�i, l�m d?ng game.
- **Kh?ng d?nh an to�n**: Plan s?a \maxExp = 60\ v� \maxExp *= 1.15f\. Gi� tr? n�y s? lu�n tang d?n v� duong (\u003E0). V�ng l?p EXP l� ho�n to�n an to�n.

