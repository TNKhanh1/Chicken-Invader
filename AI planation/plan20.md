# Plan 20: Core Gameplay Balancing & Difficulty Adjustment

This document outlines the balancing adjustments made to increase the difficulty and engagement of the early game (Stage 1), specifically targeting the boss fight and enemy wave density.

## 1. Fire Phoenix Boss Buffs (Stage 1)
To make the Stage 1 Boss a more challenging "Bullet Hell" experience, the following mechanics were overhauled:

- **Auto-Aim Targeting**: Rewrote the targeting logic for the *Normal Attack* and *Large Fireball*. The boss now uses `atan2` to calculate the exact angle to the player's spaceship, firing directly at the player instead of dropping bullets blindly downwards.
- **Increased Bullet Density**:
  - *Normal Attack*: Bullet count increased from 3 to 5 (Phase 1) and from 5 to 9 (Phase 2) with wider spread angles.
  - *Large Fireball*: Projectile count increased from 1 to 3 (Phase 1) and from 2 to 5 (Phase 2).
  - *Fire Rain*: 360-degree burst density doubled, from 12 to 24 bullets (Phase 1) and from 20 to 40 bullets (Phase 2).
- **Faster Attack Rate**: Reduced the boss's `attackCooldown` from 2.5 seconds to 2.0 seconds, forcing the player to dodge more frequently.

## 2. Wave 4 Batch 2 Adjustments (Data-Driven)
Utilizing the data-driven architecture, the difficulty of Wave 4 Batch 2 was increased directly via the JSON configuration file (`data/stage1.json`) without recompiling the C++ code:

- **Increased Enemy Count**: Doubled the number of `TANK` chickens in this batch from 5 to 10.
- **Formation Expansion**: Expanded the `GRID` layout from a single row (`1x5`) to a double row (`2x5`).
- **Spacing Adjustments**: Set `spacing_y` to `120.0` pixels to create a deeply layered defensive wall of high-HP enemies, resulting in a significantly denser egg-drop rate for the player to survive.

---

# Chi tiết Boss Màn 2 (Wave 5) & Cấu trúc Stage 2

## Mục tiêu
Thiết kế và lập trình Boss cho Wave 5 của Stage 2 sử dụng hình ảnh `chicken11.png` (kích thước 500x500). Stage 2 sẽ được mở rộng ra 10 Wave. Boss ở Wave 5 sẽ đóng vai trò là Mid-Boss (Boss giữa màn).

## 3. Cơ sở Tính toán Chỉ số (Player DPS vs Boss HP)
Dựa theo `spaceship.csv`, một phi thuyền trung bình khi đến Stage 2 Wave 5 sẽ có vũ khí khoảng **Level 5 - Level 6**.
- **Hypergun Level 5**: Damage = 23, AttackSpeed = 5.4 đạn/giây. 
- Giả định phi thuyền bắn tỏa 3 nòng trúng 2 nòng vào Boss -> DPS thực tế = `23 * 5.4 * 2 = 248.4 sát thương/giây`. 
- Nếu cộng thêm Tỷ lệ chí mạng (9% crit, 170% crit damage), DPS thực tế rơi vào khoảng **~280 sát thương/giây**.

**Tính toán Máu Boss (Chicken11)**: 
Để một trận đánh Mid-Boss kéo dài khoảng **2.5 phút (150 giây)** với người chơi trình độ trung bình (đạn trượt 20%):
- Tổng sát thương người chơi gây ra trong 150s: `280 * 150 * 80% = 33,600`.
- **Quyết định HP Boss**: Set `40,000 HP` (Chỉ 1 Phase duy nhất). 
- **Giáp (Armor)**: 15. (Giảm 15 sát thương mỗi viên đạn, ép người chơi nâng cấp vũ khí lên ít nhất level 4 để xuyên giáp).

## 4. Cấu trúc Stage 2 (10 Waves) với Chỉ số Cụ thể
Hệ số sức mạnh (Power Creep) so với Stage 1 là **x1.5 lần**.
- **Wave 1 - 2 (Khởi động)**: 
  - Gà thường (Normal): `HP 150`, `Damage 30`, `Speed 120`, Spawn 25 con (Hình ZigZag).
- **Wave 3 - 4 (Thử thách)**:
  - Gà Tank: `HP 1500`, `Armor 15`, `Damage 75`, Spawn 15 con (Lưới 3x5).
- **Wave 5 (Mid-Boss)**: Chi tiết sẽ được viết riêng ở `planboss02.md`.
- **Wave 6 - 9 (Cao trào Bullet Hell)**:
  - Gà Laze (Sniper): `HP 800`, Bắn tia Laze tốc độ `800 px/s`, sát thương `50`. (Đòi hỏi kỹ năng né).
- **Wave 10 (Final Boss)**: Sẽ thiết kế ở Kế hoạch sau.
