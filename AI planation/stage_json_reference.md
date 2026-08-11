# Stage JSON Reference Guide

> This document is the definitive, authoritative reference for creating `data/stageX.json` files.  
> Every field listed here is **verified against the actual C++ code** in `WaveManager.cpp`, `FormationBuilder.cpp`, `Enemy.h`, and `EnemyFactory.cpp`.

---

## 1. Top-Level Structure

```json
{
    "stage_id": 1,
    "name": "Stage 1",
    "waves": [ ... ]
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `stage_id` | `int` | ✅ | Stage number (1, 2, 3, ...) |
| `name` | `string` | ❌ | Human-readable name (unused by engine) |
| `waves` | `array` | ✅ | Array of Wave objects |

---

## 2. Wave Object

```json
{
    "wave_id": 1,
    "batches": [ ... ]
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `wave_id` | `int` | ✅ | Wave number within this stage (1, 2, 3, ...) |
| `batches` | `array` | ✅ | Array of Batch objects |

**Rules:**
- Waves are played sequentially: Wave 1 → Wave 2 → Wave 3 → ...
- Between each wave, the player enters a **Stat Selection** screen.
- A wave can have **multiple batches** (sub-waves). Batches with the same `batch_id` spawn **simultaneously**.

---

## 3. Batch Object (The Core Unit)

```json
{
    "batch_id": 1,
    "visual_id": 1,
    "role": "NORMAL",
    "count": 15,
    "layout": { ... },
    "movement": { ... },
    "stats": { ... }
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `batch_id` | `int` | ✅ | Batch number within the wave. Batches are spawned sequentially (1 → 2 → 3). **Multiple entries with the same `batch_id` spawn simultaneously.** |
| `visual_id` | `int` | ✅ | Which chicken texture to use (1–10). Maps to `chicken01_anim.png` through `chicken10_anim.png`. |
| `role` | `string` | ✅ | Enemy role. **Must be a string**, not a number. |
| `count` | `int` | ✅ | Total number of enemies to spawn. Used by `V_SHAPE`, `INTERSECTING_V`, `RANDOM_RAIN`. For `GRID` and `SWEEP_TO_GRID`, the count is derived from `rows * cols`. |
| `layout` | `object` | ✅ | Formation/positioning configuration. |
| `movement` | `object` | ✅ | Movement behavior configuration. |
| `stats` | `object` | ✅ | Enemy stat block. |

### 3.1 `role` (String Values)

| Value | Description |
|---|---|
| `"NORMAL"` | Standard chicken. Default size. |
| `"SWARM"` | Small, fast, low-HP chicken. |
| `"TANK"` | Large, high-HP chicken. |
| `"BOSS"` | Boss chicken. |
| `"ASTEROID"` | Not a chicken — uses asteroid texture instead. **Must set `canShoot = false` (handled automatically by code).** |

---

## 4. `stats` Object (ALL FIELDS MANDATORY)

```json
"stats": {
    "hp": 100,
    "damage": 20,
    "armor": 0,
    "speed": 100,
    "egg_rate": 3.0,
    "score": 10
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `hp` | `float` | ✅ | Hit points. Chicken dies when HP reaches 0. |
| `damage` | `float` | ✅ | Contact damage dealt to the player. |
| `armor` | `float` | ✅ | Damage reduction. Set to `0` for no armor. |
| `speed` | `float` | ✅ | Base movement speed (pixels/second). Used by `RANDOM_RAIN` to calculate spread distance. For other formations, speed is defined by the movement behavior. |
| `egg_rate` | `float` | ✅ | Seconds between egg drops. Set to `0.0` for enemies that never shoot (e.g., asteroids). |
| `score` | `int` | ✅ | Points awarded to the player upon killing this enemy. |

> ⚠️ **WARNING:** If ANY of these 6 fields is missing, the game will **crash instantly** with a `json::out_of_range` exception!

---

## 5. `layout` Object (Formation Types)

The `layout.type` field determines which `FormationBuilder` function is called.  
Each type requires **different fields**. Using the wrong fields will crash the game.

---

### 5.1 `GRID` — Rectangular Grid

Spawns enemies in a rectangular grid dropping from above.

```json
"layout": {
    "type": "GRID",
    "rows": 3,
    "cols": 5,
    "spacing_x": 150.0,
    "spacing_y": 100.0,
    "start_y": -300.0,
    "target_base_y": 100.0
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `type` | `"GRID"` | ✅ | — |
| `rows` | `int` | ✅ | Number of rows. |
| `cols` | `int` | ✅ | Number of columns. |
| `spacing_x` | `float` | ✅ | Horizontal distance between columns (pixels). |
| `spacing_y` | `float` | ✅ | Vertical distance between rows (pixels). |
| `start_y` | `float` | ✅ | Initial Y position (use negative values, e.g., `-300.0`, to spawn above screen). |
| `target_base_y` | `float` | ✅ | Final Y position for the **first row**. Subsequent rows are placed at `target_base_y + row * spacing_y`. |

**How it works (FormationBuilder::BuildGrid):**
- Grid is **centered horizontally** on the screen automatically.
- Total enemies spawned = `rows × cols`.
- Each enemy drops from `(x, start_y + row * spacing_y)` to `(x, target_base_y + row * spacing_y)`.

---

### 5.2 `V_SHAPE` — Inverted V Formation

Spawns enemies in one or more stacked V-shapes.

```json
"layout": {
    "type": "V_SHAPE",
    "layers": 2,
    "layer_spacing": 120.0,
    "spacing_x": 120.0,
    "spacing_y": 80.0,
    "start_y": -100.0,
    "target_base_y": 80.0
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `type` | `"V_SHAPE"` | ✅ | — |
| `layers` | `int` | ✅ | Number of V-layers stacked behind each other (1, 2, or 3). |
| `layer_spacing` | `float` | ✅ | Vertical distance between each V-layer. Set to `0.0` if `layers` = 1. |
| `spacing_x` | `float` | ✅ | Horizontal distance between each enemy in the V. |
| `spacing_y` | `float` | ✅ | Vertical offset per step from center (controls the V "steepness"). |
| `start_y` | `float` | ✅ | Starting Y above screen (negative value). |
| `target_base_y` | `float` | ✅ | Final Y position for the center of the V. |

**How it works (FormationBuilder::BuildVShape):**
- The V is **centered horizontally** on screen.
- `count` (from batch level) determines the number of enemies **per layer**.
- Wing tips are higher than the center: `targetY = target_base_y + |distance_from_center| * spacing_y`.
- Each subsequent layer is pushed further back by `layer_spacing`.
- Total enemies = `count × layers`.

---

### 5.3 `SWEEP_TO_GRID` — Sweep from Sides

Two groups of enemies fly in from the left and right edges, cross in the center, then rearrange into a rectangular grid.

```json
"layout": {
    "type": "SWEEP_TO_GRID",
    "rows": 2,
    "cols_per_side": 4,
    "spacing_x": 150.0,
    "spacing_y": 120.0,
    "target_base_y": 50.0
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `type` | `"SWEEP_TO_GRID"` | ✅ | — |
| `rows` | `int` | ✅ | Number of rows in the final grid. |
| `cols_per_side` | `int` | ✅ | Number of columns **per side** (left and right). Total columns = `cols_per_side × 2`. |
| `spacing_x` | `float` | ✅ | Horizontal spacing in the final grid. |
| `spacing_y` | `float` | ✅ | Vertical spacing in the final grid. |
| `target_base_y` | `float` | ✅ | Final Y position for the top row. |

**How it works (FormationBuilder::BuildSweepToGrid):**
- Left group starts at `x = -100`, right group at `x = screenWidth + 100`.
- They fly inward to the center, then use `WaypointMovement` to navigate to final grid positions.
- Total enemies = `rows × cols_per_side × 2`.
- **Movement MUST be `"WAYPOINT"`** for this to work correctly!

---

### 5.4 `INTERSECTING_V` — Two V-shapes Crossing

A top V-shape descends and a bottom V-shape ascends; they cross at the screen center, then rearrange into a rectangular grid.

```json
"layout": {
    "type": "INTERSECTING_V",
    "spacing_x": 120.0,
    "spacing_y": 100.0,
    "target_base_y": 100.0
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `type` | `"INTERSECTING_V"` | ✅ | — |
| `spacing_x` | `float` | ✅ | Horizontal spacing. |
| `spacing_y` | `float` | ✅ | Vertical spacing. |
| `target_base_y` | `float` | ✅ | Final Y for the top row of the resulting rectangle. |

**How it works (FormationBuilder::BuildIntersectingV):**
- Uses `count` from the batch level (not in layout).
- Produces `count × 2` enemies total (top V + bottom V).
- Final grid is hardcoded to 5 columns.
- **Movement MUST be `"WAYPOINT"`!**

---

### 5.5 `RANDOM_RAIN` — Meteor/Asteroid Rain

Spawns enemies at random X positions above the screen, spread over a time duration.

```json
"layout": {
    "type": "RANDOM_RAIN",
    "duration": 15.0,
    "start_y": -100.0,
    "asteroid_variant": 1
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `type` | `"RANDOM_RAIN"` | ✅ | — |
| `duration` | `float` | ✅ | Total duration of the rain in seconds. Enemies are spread vertically so they arrive over this time window. |
| `start_y` | `float` | ✅ | Starting Y position (negative, above screen). |
| `asteroid_variant` | `int` | ❌ | `1` = Normal asteroid, `2` = Flame asteroid. If omitted, random 1 or 2. |

**How it works (FormationBuilder::BuildRandomRain):**
- Uses `count` from the batch level.
- `stats.speed` is used to calculate how far apart to spread enemies vertically.
- `maxOffset = duration × speed`. Each enemy starts at a random Y between `start_y` and `start_y - maxOffset`.
- **Role MUST be `"ASTEROID"`.** This enables asteroid rendering and disables egg shooting.
- **Movement should be `"METEOR_DIVE"`.** This makes asteroids fall straight down with rotation.

---

### 5.6 `TARGETED_PLAYER` — Spawns on Player's X Position

Spawns a single enemy directly above the player's current position. Used for targeted asteroids.

```json
"layout": {
    "type": "TARGETED_PLAYER",
    "start_y": -100.0,
    "asteroid_variant": 2,
    "spawn_delay": 3.0
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `type` | `"TARGETED_PLAYER"` | ✅ | — |
| `start_y` | `float` | ✅ | Starting Y position. |
| `asteroid_variant` | `int` | ❌ | Which asteroid texture to use. |
| `spawn_delay` | `float` | ❌ | Delay in seconds before this enemy actually spawns. **If > 0, the spawn is deferred** and handled by `WaveManager::Update()`. This is how you create timed spawns mid-wave. |

**How it works:**
- Only spawns **1 enemy** per entry.
- The enemy's X position = the player's current X at the moment of spawn.
- **Role MUST be `"ASTEROID"`.** Movement MUST be `"METEOR_DIVE"`.
- To spawn 3 targeted asteroids at 3s, 6s, and 9s, create **3 separate batch entries** with the same `batch_id` but different `spawn_delay` values.

---

## 6. `movement` Object (Movement Types)

The `movement.type` field determines which `IMovementBehavior` subclass is assigned.

---

### 6.1 `HORIZONTAL_BOUNCE`

Enemy drops to target Y, then bounces left/right indefinitely.

```json
"movement": {
    "type": "HORIZONTAL_BOUNCE",
    "drift": 0.0
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `type` | `"HORIZONTAL_BOUNCE"` | ✅ | — |
| `drift` | `float` | ✅ | Horizontal oscillation amplitude in pixels. `0.0` = pure hover (no side movement). `300.0` = wide sweeping. |

---

### 6.2 `WAYPOINT`

Enemy follows a sequence of waypoints generated by `FormationBuilder`. **No additional fields needed.**

```json
"movement": {
    "type": "WAYPOINT"
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `type` | `"WAYPOINT"` | ✅ | — |

**Usage:** Required for `SWEEP_TO_GRID` and `INTERSECTING_V` layout types.

---

### 6.3 `METEOR_DIVE`

Enemy falls straight down with visual rotation. **No additional fields needed.**

```json
"movement": {
    "type": "METEOR_DIVE"
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `type` | `"METEOR_DIVE"` | ✅ | — |

**Usage:** Required for `RANDOM_RAIN` and `TARGETED_PLAYER` layout types.

---

### 6.4 `STRAIGHT`

Enemy moves in a fixed direction at constant speed.

```json
"movement": {
    "type": "STRAIGHT"
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `type` | `"STRAIGHT"` | ✅ | — |

---

## 7. Valid Layout + Movement Combinations

Not all combinations work. Use this table:

| Layout Type | Valid Movement | Why |
|---|---|---|
| `GRID` | `HORIZONTAL_BOUNCE` | Drops to target, then bounces |
| `V_SHAPE` | `HORIZONTAL_BOUNCE` | Drops to target V position, then bounces |
| `SWEEP_TO_GRID` | `WAYPOINT` | Follows sweep → intersect → grid waypoints |
| `INTERSECTING_V` | `WAYPOINT` | Follows V → intersect → grid waypoints |
| `RANDOM_RAIN` | `METEOR_DIVE` | Falls straight down |
| `TARGETED_PLAYER` | `METEOR_DIVE` | Falls straight down onto player |

> ⚠️ Using an invalid combination (e.g., `GRID` + `WAYPOINT`) will cause enemies to spawn but **not move correctly** (they will be stuck or float away).

---

## 8. GameManager Validation Rules

In `GameManager.cpp`, the test menu validates which stages/waves are playable:

```cpp
bool isValidSelection = (testConfig.stage == 1 && testConfig.wave <= 10)
                      || (testConfig.stage == 2 && testConfig.wave <= 4);
```

**When adding a new stage**, you MUST update this line to include the new stage and its wave count. Otherwise the test menu will show "WARNING: WAVE NOT YET IMPLEMENTED" and refuse to start.

---

## 9. Stage Transition Logic

When the player clears the **last wave** of a stage:
1. `GameManager` calls `WaveManager::GetTotalWaves()` to check if there are more waves.
2. If `currentWave >= GetTotalWaves()`, it increments `currentStage`, resets `currentWave = 1`, and loads the next `data/stageX.json`.
3. A `stage3.json` file must exist in `data/` for the transition to work.

---

## 10. Complete Example (Stage 1, Wave 1, Batch 1)

```json
{
    "batch_id": 1,
    "visual_id": 1,
    "role": "NORMAL",
    "count": 15,
    "layout": {
        "type": "GRID",
        "rows": 3,
        "cols": 5,
        "spacing_x": 150.0,
        "spacing_y": 100.0,
        "start_y": -300.0,
        "target_base_y": 100.0
    },
    "movement": {
        "type": "HORIZONTAL_BOUNCE",
        "drift": 0.0
    },
    "stats": {
        "hp": 100,
        "damage": 20,
        "armor": 0,
        "speed": 100,
        "egg_rate": 3.0,
        "score": 10
    }
}
```

---

## 11. Checklist Before Creating a New Stage

- [ ] All 6 `stats` fields present in every batch (`hp`, `damage`, `armor`, `speed`, `egg_rate`, `score`)
- [ ] `role` is a **string** (e.g., `"NORMAL"`), not a number
- [ ] `layout.type` matches a valid formation name exactly
- [ ] All required layout fields for the chosen type are present
- [ ] `movement.type` is compatible with the layout type (see Section 7)
- [ ] `movement.drift` is present when using `HORIZONTAL_BOUNCE`
- [ ] `layout.target_base_y` is present for `GRID`, `V_SHAPE`, `SWEEP_TO_GRID`, `INTERSECTING_V`
- [ ] `layout.duration` is present for `RANDOM_RAIN`
- [ ] `count` is present at the **batch level** (not inside `layout`)
- [ ] `batch_id` numbering is sequential (1, 2, 3, ...)
- [ ] `wave_id` numbering is sequential (1, 2, 3, ...)
- [ ] Asteroids: `role` = `"ASTEROID"`, `movement` = `"METEOR_DIVE"`, `egg_rate` = `0.0`
- [ ] Update `isValidSelection` in `GameManager.cpp` for the test menu

---

## 12. Known Bugs & Fixes (Reference)

### 12.1. Nlohmann JSON `type_error.302` Crash
- **Symptom:** Game crashes instantly upon loading a stage or spawning a batch with the exception `[json.exception.type_error.302] type must be number, but is null`.
- **Cause:** Attempting to access an undeclared JSON key directly (e.g., `s["armor"]`) when the field is missing in `stage.json` returns a `null` object. When C++ tries to cast this `null` to `float` or `int`, it throws an exception.
- **Fix:** Used the safe `.value()` accessor in `WaveManager.cpp` to provide a fallback default instead of crashing (e.g., `s.value("armor", 0.0f)`).

### 12.2. `METEOR_SHOWER` Fails to Spawn
- **Symptom:** `METEOR_SHOWER` batches do absolutely nothing after their `spawn_delay` expires.
- **Cause:** The delayed spawn logic in `WaveManager::Update()` did not generate any `spawnPoints` for layout type `METEOR_SHOWER`, resulting in an empty list, so the enemies were never spawned.
- **Fix:** Implemented specific `spawnPoints` generation logic for `METEOR_SHOWER` inside `WaveManager::Update()`, randomizing X and Y coordinates above the screen based on `b["count"]`.

### 12.3. Lỗi Đội hình đè lên nhau (Overlap) khi dùng HORIZONTAL_BOUNCE
- **Symptom:** Nhiều đàn gà bay ra và xếp chồng lên nhau ở điểm dừng cuối cùng, dù `start_y` được cấu hình cách xa nhau.
- **Cause:** Thuật toán tính Y đích cho toàn bộ một cụm là `targetY = targetBaseY + r * spacingY`. Nếu 2 Batch khác nhau được định nghĩa `target_base_y` không cách đủ xa (Ví dụ: Đàn 2 hàng dừng ở 100 => chiếm mốc 100 và 200, Đàn 1 hàng dừng ở 150), thì Y đích của chúng sẽ giao nhau (Y=200 và Y=150 sẽ đâm xuyên nhau vì mỗi con gà cao 100px). 
- **Fix:** Phải tính toán kỹ số hàng (`rows`) và `spacing_y` của từng Batch, đảm bảo `target_base_y` của Batch sau phải cộng dồn ít nhất bằng tổng chiều cao của Batch trước. (Ví dụ đàn trên dừng ở 100, 2 hàng khoảng cách 100 => chiếm không gian đến 200. Đàn tiếp theo phải dừng ở mức 300). Đồng thời, khoảng cách của `start_y` (khi spawn) phải y hệt như khoảng cách của `target_base_y` để tránh va chạm khi đang rơi.

### 12.4. Lỗi Gà bay giật cục, co giật trái phải liên tục
- **Symptom:** Gà di chuyển kiểu `HORIZONTAL_BOUNCE` bị kẹt ở một chỗ, rung lắc dữ dội sang hai bên.
- **Cause:** Thuộc tính `drift` (Biên độ trượt ngang) quá nhỏ (Ví dụ: `drift = 10.0`). Với tốc độ di chuyển cao (Speed = 100), gà đạt đến giới hạn `drift` chỉ trong chưa tới `0.1s` và đảo chiều liên tục.
- **Fix:** Tăng `drift` lên tối thiểu `100.0` - `150.0` để gà có đủ khoảng trống trượt mượt mà sang hai bên trước khi đảo chiều quay lại.
