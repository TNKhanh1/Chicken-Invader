# Plan 17: Data-Driven Stage System & Formation Fixes

## 1. Goal
Transition the hardcoded Wave and Batch enemy spawning logic into a JSON-based **Data-Driven Level System**. Resolve spawning coordinates, missing enemies, and delayed-spawn issues that arose from the transition to ensure all formations precisely match the original gameplay intent.

## 2. Implementation Details

### 2.1 Data-Driven Level System (JSON)
- Created `data/stage1.json` to act as the single source of truth for Stage 1's wave logic.
- Rewrote `WaveManager` to parse `nlohmann::json` files instead of relying on hardcoded arrays.
- Structured the JSON schema to define `visual_id`, `role`, `stats`, `layout`, and `movement` for each enemy batch.
- Integrated `GameManager` to automatically load `data/stageX.json` upon entering a stage.

### 2.2 Wave 3: Asteroid Variations
- Separated the Wave 3 asteroid rain into two simultaneous batches within the JSON.
- Batch 1 spawns 30 `asteroid_variant: 1`.
- Batch 2 spawns 30 `asteroid_variant: 2`.
- Both batches share the same duration and fall logic, creating a mixed meteor shower.

### 2.3 Simultaneous Batch Spawning (Wave 4)
- Fixed a bug in `WaveManager::SpawnBatch` where only the first matching `batch_id` was processed, causing the Targeted Asteroids to not spawn alongside the Tank Chickens in Wave 4.
- Modified `WaveManager::SpawnBatch` to process **all** JSON entries that share the requested `batch_id` before returning, allowing concurrent enemy patterns.
- Hooked `WaveManager::GetInstance()->Update(deltaTime)` into `GameManager::UpdatePlay` (the `TEST_GAMEPLAY` state) to ensure time-delayed spawns (like the 3-second interval targeted asteroids) are properly executed mid-wave.

### 2.4 Formation Positioning Fixes (Wave 1 & 2)

#### Wave 1.3 (`SWEEP_TO_GRID`)
- **Issue:** Enemies overlapped vertically while sweeping in, and the final grid was too tightly packed.
- **Fix:** Increased the vertical starting gap (`startY = 50.0f + i * 120.0f`) in `FormationBuilder::BuildSweepToGrid` to prevent texture overlap during the fly-in. Increased `spacing_x` to `150.0` and `spacing_y` to `120.0` in the JSON config.

#### Wave 2.1 (`V_SHAPE`)
- **Issue:** The double V-shape was too compact.
- **Fix:** Increased JSON layout parameters (`spacing_x: 120.0`, `spacing_y: 80.0`, `layer_spacing: 120.0`) to give the formation breathing room.

#### Wave 2.2 (`INTERSECTING_V`)
- **Issue:** The bottom V-shape was missing chickens, and the final grid did not form a perfect rectangle.
- **Fix:** 
  - The missing chickens were caused by `Enemy::Update`'s off-screen culling limit (`screenHeight + 300.0f`). The bottom V-shape was starting too deep (e.g., `y = 1260`), triggering instant deletion. Fixed by clamping the starting Y-coordinate of the bottom V to `screenHeight + 50.0f` in `FormationBuilder`.
  - Removed hardcoded spacing values (e.g., `100.0f`) in `FormationBuilder::BuildIntersectingV` and replaced them with dynamic grid generation based on `spacing_x` and `spacing_y`. This ensures the two halves interlock seamlessly into a perfect 4x5 rectangle.

## 3. Potential Bugs / Architectural Review
- **OOP & Patterns:** Maintained the Singleton pattern for `WaveManager` and `GameManager`. The Strategy pattern for `IMovementBehavior` dynamically receives the correct pathing based on JSON string mapping.
- **Safety:** The JSON implementation uses proper boundary checks (`contains()`) for optional fields (like `spawn_delay` and `asteroid_variant`) to prevent crashes.

## 4. Verification
- Compiled successfully with Raylib `mingw32-make`.
- Manual testing verified all spacing, overlapping, and missing entity issues are fully resolved. Targeted asteroids drop perfectly on schedule during the Tank fight.
