# Plan 18: Stage 2 Implementation & Animation Synchronization

## 1. Goal
Implement the first 4 waves of **Stage 2** using the `chicken02` variant. This involved:
- Refactoring the texture loading system to support all 10 chicken variants dynamically.
- Fixing animation synchronization and size consistency across all chicken types.
- Creating a fully data-driven stage transition system.
- Designing unique, multi-batch wave choreography for Stage 2.
- Writing a comprehensive [Stage JSON Reference Guide](file:///e:/ChickenInvader_local/ChickenInvader/AI%20planation/stage_json_reference.md) for future stage creation.

---

## 2. Implemented Features

### 2.1 Multi-Texture Array (OOP Refactoring)
**Problem:** `GameManager` only loaded a single `Texture2D texEnemyAnim` (chicken01). Stage 2 requires chicken02.

**Solution (Following Plan 16 Step 2):**
- `GameManager.h`: Replaced `Texture2D texEnemyAnim;` with `Texture2D texEnemyAnims[10];`.
- `GameManager.h`: Added `Texture2D& GetTexEnemyAnim(int variantIndex)` with bounds-checking and fallback to index 0.
- `GameManager.cpp`: Init loads all 10 sprite sheets in a `for` loop using `snprintf` to construct file paths dynamically.
- `GameManager.cpp`: Destructor unloads all 10 textures in a loop.

### 2.2 Dynamic Frame Slicing (Enemy::Draw)
**Problem:** `Enemy::Draw()` hardcoded `100.0f` as the frame size, which would break for sprite sheets with different resolutions.

**Solution:**
- `Enemy.cpp`: Changed `gm->GetTexEnemyAnim()` to `gm->GetTexEnemyAnim(visualId - 1)`.
- Replaced hardcoded `100.0f` with `float frameSize = (float)tex.height;` — this dynamically reads the actual texture height to determine frame dimensions.
- Updated `srcRec` to use `frameSize` for both width and height slicing.

### 2.3 Animation Synchronization Fix
**Problem:** `chicken02_anim.png` through `chicken10_anim.png` were generated at their native resolution (150x150 for chicken02), causing:
- Visually larger/different animation amplitudes compared to chicken01 (100x100).
- Inconsistent wing flapping speed and body bobbing between chicken types.

**Solution:**
- Rewrote the batch animation script (`batch_warp_v3.py`).
- **Key change:** All source images (chicken02-10) are first **resized to 100x100** using `Image.LANCZOS` before applying the mesh warp. This ensures:
  - All output sprite sheets are **2400x100** (24 frames × 100x100), identical to chicken01.
  - The warp uses the **exact same absolute pixel values** as chicken01's script (`left_pivot=30`, `right_pivot=70`, `body_dy=4.0`, wing amplitude `15.0`, leg amplitude `6.0`).
  - All chickens now flap, bob, and spread legs with identical timing and amplitude.

### 2.4 Data-Driven Wave/Batch Transitions
**Problem:** `GameManager.cpp` hardcoded `int maxBatch = 3;` and had no logic for transitioning between stages.

**Solution:**
- `WaveManager.h/cpp`: Added `int GetMaxBatchForWave(int waveId) const` — queries the JSON for the highest `batch_id` in a given wave.
- `WaveManager.h/cpp`: Added `int GetTotalWaves() const` — returns the total number of waves in the loaded JSON.
- `GameManager.cpp`: Replaced hardcoded `maxBatch = 3` with `WaveManager::GetInstance()->GetMaxBatchForWave(currentWave)`.
- `GameManager.cpp`: Added stage transition: when `currentWave >= GetTotalWaves()`, increments `currentStage`, resets `currentWave = 1`, and loads `data/stageX.json`.
- `GameManager.cpp`: Updated `isValidSelection` to allow testing Stage 2 (waves 1–4).

### 2.5 JSON Crash Fix
**Problem:** The initial `stage2.json` was written with incorrect field names and types, causing instant crashes:
- `role` was an integer (`1`) instead of a string (`"NORMAL"`).
- Missing mandatory `stats` fields: `damage`, `armor`, `speed`, `egg_rate`.
- Missing `movement.drift` for `HORIZONTAL_BOUNCE`.
- Missing `layout.target_base_y`, `layout.duration`, and batch-level `count`.

**Solution:** Completely rewrote `stage2.json` to match the exact schema of `stage1.json`. Created `stage_json_reference.md` to prevent this class of error in the future.

### 2.6 Stage 2 Wave Design (Creative, Multi-Batch)

#### Wave 1 (3 Batches)
- **Batch 1:** 10 chicken02 in a single V-shape, drifting left/right with `drift: 150`.
- **Batch 2:** 15 chicken02 in a 3×5 grid with wider sweeping (`drift: 200`).
- **Batch 3:** 10 chicken02 SWARM in an intersecting V, fast and low-HP.

#### Wave 2 (3 Batches)
- **Batch 1:** 16 chicken02 sweeping from both sides into a 4×4 grid (`SWEEP_TO_GRID`).
- **Batch 2:** 8 chicken02 in a double-layer V-shape, hovering in place (`drift: 0`).
- **Batch 3:** 10 chicken02 in a 2×5 grid with aggressive sweeping (`drift: 250`).

#### Wave 3 (2 Batches — Meteor + Ambush)
- **Batch 1:** Mixed meteor rain — 25 normal asteroids + 25 flame asteroids simultaneously for 10 seconds.
- **Batch 2:** 20 chicken02 in a 4×5 grid drop down after the meteor rain clears, catching the player off guard.

#### Wave 4 (3 Batches — Mini-Boss Fight)
- **Batch 1:** 12 chicken02 in a double V-shape as the opening formation.
- **Batch 2:** 3 TANK chicken02 in a 1×3 row with 3 targeted flame asteroids at 3s/6s/9s intervals.
- **Batch 3:** 10 chicken02 in an intersecting V as reinforcements.

### 2.7 Miscellaneous Fix
- Fixed a stray `ss` typo in `src/AllWeaponBehaviors.cpp` line 11 (`{ss` → `{`) that was preventing compilation.

---

## 3. Files Modified

| File | Change |
|---|---|
| `include/GameManager.h` | `texEnemyAnim` → `texEnemyAnims[10]`, new getter |
| `src/GameManager.cpp` | Loop-load 10 textures, loop-unload, dynamic maxBatch, stage transitions, isValidSelection |
| `src/Enemy.cpp` | Dynamic `frameSize = tex.height`, `GetTexEnemyAnim(visualId - 1)` |
| `include/WaveManager.h` | Added `GetTotalWaves()`, `GetMaxBatchForWave()` |
| `src/WaveManager.cpp` | Implemented `GetTotalWaves()`, `GetMaxBatchForWave()` |
| `src/AllWeaponBehaviors.cpp` | Removed stray `ss` typo |
| `data/stage2.json` | New file: 4 waves, 11 batches total |
| `assets/enemy/chicken02-10_anim.png` | Regenerated: all now 2400x100 (synchronized with chicken01) |

---

## 4. Reference Documents Created
- [stage_json_reference.md](file:///e:/ChickenInvader_local/ChickenInvader/AI%20planation/stage_json_reference.md) — Complete field-by-field guide for writing `stageX.json` files, verified against C++ source.
- [chicken_animation_reference.md](file:///e:/ChickenInvader_local/ChickenInvader/AI%20planation/chicken_animation_reference.md) — Reference for animation standardization (3-tier system).
- [planboss01.md](file:///e:/ChickenInvader_local/ChickenInvader/AI%20planation/planboss01.md) — Detailed implementation plan for adding Boss 1 (chicken09) to Stage 1, Wave 5.

---

## 5. Argument (Lõi) System Fixes
- **Analyzed all 10 Arguments** to ensure they match their in-game descriptions.
- **Fixed Boss Hunter (Arg 3)**: It was incorrectly applying +80% bonus damage to `ASTEROID`. Changed the condition to `EnemyRole::BOSS` in `GameManager.cpp` (both normal bullets and beam weapons).
- **Fixed Stat Windfall (Arg 1)**: Description promised "3 extra Stat Selections" but code gave flat HP/Mana/Levels to bypass loop complexity. 
  - Refactored `GameManager.h` to add `extraStatSelectionsPending`.
  - Updated `GameManager.cpp` to correctly chain 3 consecutive `STAT_SELECTION` states when this Argument is chosen.
- Compiled successfully and verified the fixes.

---

## 6. Fire Phoenix Boss (Stage 1, Wave 5) Implementation
- **Completed Boss implementation** based on `planboss01.md`.
- Added `FireBullet` class for procedural fire projectiles with glowing and pulsing effects (no external texture required).
- Added `FirePhoenixBoss` class implementing a complex 2-phase lifecycle:
  - **Phase 1** has a normal attack cycle and wing spark particles (fewer, lower alpha).
  - Overrode `Die()` to prevent death at the end of Phase 1. Instead, triggers a 2-second transition state (invulnerable, flashes white/orange, bursts of sparks).
  - **Phase 2** features faster attacks, denser spark particles (brighter, more frequent), and a subtle red tint on the sprite.
  - Integrated 2 alternating skills into the attack cycle: Fireball (large glowing projectiles) and Fire Rain (radial burst of small projectiles).
  - Implemented a custom Boss HP bar with Phase indicators drawn directly above the boss.
- Updated `EnemyFactory.cpp` and `stage1.json` to cleanly construct `FirePhoenixBoss` using `visualId == 10` (the new chicken10 variant).
- Appended Wave 5 configuration to `data/stage1.json` defining the boss encounter.

### 6.1 Visual & Bullet Upgrades (Follow-up)
- **Advanced Boss Image Warping:** 
  - Wrote a custom Python script (`create_boss_anim.py`) to process `chicken10.png` (531x470).
  - Used Connected Components (`skimage.measure.label`) to automatically isolate the main body from detached fire feathers.
  - Applied mesh warping (flapping) ONLY to the main body.
  - Implemented a 2-stream falling and fading logic for the detached fire feathers to create a seamless infinite looping idle animation (cháy rụi/rụng lông) without stretching them.
  - Automatically detected and filtered out an extraneous Gemini watermark logo in the bottom right corner before processing.
- **Procedural + Texture Bullet Blending:**
  - Loaded `firebullet.png` into `GameManager`.
  - Refactored `FireBullet` to use an `enum class Type { NORMAL, LARGE, EXPLOSIVE }` for robust OOP scalability instead of a boolean flag.
  - Scaled up normal bullets by 1.5x.
  - Implemented the `EXPLOSIVE` bullet type: it dynamically aims at the player's predicted position (`player->GetHitbox()`), travels for 0.8 seconds, and automatically detonates into a 360-degree spread of 16 `NORMAL` bullets.
  - Removed `firebullet.png` to rely entirely on procedural generation (`DrawCircleGradient`) for boss projectiles to save resources and maintain a consistent aesthetic.
- Scaled up the procedural bullets significantly to match the visual weight of the previous sprite.
- Successfully compiled the game without errors. The implementation strictly adheres to OOP principles (Inheritance, Strategy, Factory) and encapsulates bullet explosion logic directly within the `FireBullet` class to avoid logic leaks.

### 6.2 Boss Polish & Critical Bug Fixes
- **Iterator Invalidation Crash Fix:** 
  - **Issue:** The game crashed via Segmentation Fault during the Phase 2 boss fight when the boss spawned a burst of bullets. This was due to modifying `GameManager::activeBullets` while iterating over it in `GameManager::Update()`.
  - **Solution:** Implemented an entity pending buffer (`pendingBullets`, `pendingEnemies`, `pendingItems`). All entities added during the `Update` loop are staged here and flushed to the active lists at the very end of the loop, preventing iterator invalidation.
- **Stage Auto-Transition Refactor:** 
  - Instead of automatically loading the JSON for the next stage upon completion of the final wave, `GameManager` now transitions back to `GameState::WAVE_SELECTION` (Main Menu). The player's HP is also fully healed.
- **Removed Boss Drumstick Drop:** Removed the logic in `Boss::Update` that spawned `Meat` every 5 seconds to keep the boss fight challenging and focused.
- **Intense Phase 2 Transition:**
  - **Duration:** Increased transition duration to 2.6 seconds.
  - **Flashing:** The boss now blinks aggressively (every 0.08s) between blinding white (`Opacity: 100%`) and a highly transparent red-orange (`Opacity: ~30%`), simulating a powerful resurrection/transformation.
  - **Stand Still:** Overrode movement by locking `targetPos = position` so the boss hovers menacingly in place.
  - **Bullet Bursts:** The boss detonates a 360-degree burst of 24 large fireballs at the exact start *and* the exact end of the transition.
  - **Aggression:** Decreased `attackCooldown` from 1.8f to 1.1f for Phase 2, resulting in vastly more bullets being fired once the boss revives.
