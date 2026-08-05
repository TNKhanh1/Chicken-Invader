# Plan 15: Wave Transition Animation System, Item Cleanup & Upcoming Procedural Chicken Animation

---

## Part 1: Wave Transition Animation System & End-of-Wave Cleanup (COMPLETED)

### Overview
Prior to this plan, the game had no transition sequence between waves — after completing the stat/argument selection screen, the game jumped directly into the next wave. This plan created a brand new cinematic `WAVE_INTRO` state and implemented critical end-of-wave cleanup mechanics.

---

### Change 1: 3-Phase Cinematic Wave Transition (`WAVE_INTRO`)

**Context:** No wave transition existed. The game went straight from `ARGUMENT_SELECTION` → `TEST_GAMEPLAY`.

**How it works:**

Three constant timing values are defined in `GameManager.h`:
```cpp
const float WARP_DURATION       = 3.4f;  // seconds
const float TEXT_SHOW_DURATION  = 2.0f;  // seconds
const float TEXT_FADE_DURATION  = 0.5f;  // seconds
// Total = 5.9 seconds
```

The timer `waveIntroTimer` is initialized to `WARP_DURATION + TEXT_SHOW_DURATION + TEXT_FADE_DURATION = 5.9f` when `StartWave()` is called and counts **down** to 0.

**Elapsed time tracking:**
```cpp
float totalDur = WARP_DURATION + TEXT_SHOW_DURATION + TEXT_FADE_DURATION; // 5.9f
float elapsed  = totalDur - waveIntroTimer;  // time since intro started
```

**Phase 1 — Warp Speed (elapsed: 0.0s → 3.4s):**
- Background scroll speed ramps from `30.0f` px/s to `3000.0f` px/s using a linear ease-in over 1 second:
  ```cpp
  float t = elapsed / 1.0f;  // t clamps to [0, 1]
  bgScrollSpeed = 30.0f + (3000.0f - 30.0f) * t;  // = 30 + 2970*t
  ```
  After `t >= 1.0f` (i.e., elapsed > 1.0s), `bgScrollSpeed` stays at `3000.0f`.
- `waveTextAlpha = 0.0f` — no text displayed.

**Phase 2 — Text Show (elapsed: 3.4s → 5.4s):**
- Background speed decelerates back to normal at rate `2000.0f` px/s²:
  ```cpp
  bgScrollSpeed = std::max(30.0f, bgScrollSpeed - 2000.0f * deltaTime);
  ```
  At 60 FPS (deltaTime ≈ 0.0167s), speed drops by ~33 px/s per frame. From 3000 to 30 takes ~(3000-30)/2000 ≈ 1.49 seconds — within the 2.0s window, so it's always back to normal before Phase 3.
- "WAVE X" text fades in: `waveTextAlpha` increases at `500.0f` units/s:
  ```cpp
  waveTextAlpha = std::min(255.0f, waveTextAlpha + 500.0f * deltaTime);
  ```
  Time to reach full opacity: 255 / 500 = 0.51 seconds. Text is fully visible after 0.51s into Phase 2 and stays at full alpha for the remaining ~1.49s.

**Phase 3 — Text Fade (elapsed: 5.4s → 5.9s):**
- `bgScrollSpeed` is snapped directly to `30.0f` (already there from Phase 2).
- `waveTextAlpha` decreases at `500.0f` units/s:
  ```cpp
  waveTextAlpha = std::max(0.0f, waveTextAlpha - 500.0f * deltaTime);
  ```
  At 0.5s duration, the alpha goes from 255 → 255 - 500*0.5 = 5 ≈ 0. Text fully disappears.

**Transition to gameplay:**
When `waveIntroTimer <= 0.0f`, the state changes to `TEST_GAMEPLAY` and `SpawnWaveBatch(currentWave, currentBatch)` is called immediately. Enemies appear right as the text finishes fading.

---

### Change 2: Player Movement During Transition

**Context:** In the original code, WASD input was only processed in `TEST_GAMEPLAY`. The player was completely frozen during `WAVE_INTRO`.

**Fix:** Duplicated the WASD polling block directly into the `WAVE_INTRO` case in `Update()`:
```cpp
if (IsKeyDown(KEY_W)) pPos.y -= player->GetMoveSpeed() * deltaTime;
if (IsKeyDown(KEY_S)) pPos.y += player->GetMoveSpeed() * deltaTime;
if (IsKeyDown(KEY_A)) pPos.x -= player->GetMoveSpeed() * deltaTime;
if (IsKeyDown(KEY_D)) pPos.x += player->GetMoveSpeed() * deltaTime;
// + boundary clamp to [0..screenWidth] x [0..screenHeight]
player->Update(deltaTime);  // recoil, animation, etc.
```
Shooting (SPACE key handling) is intentionally **not** added — the player can move but cannot fire during the transition.

---

### Change 3: Item Collection Delay at Wave End

**Context:** When the last enemy of the final batch died, `EnterStatSelection()` was called instantly on the same frame. Any items (drumsticks) still falling were immediately lost.

**Fix:** In the `TEST_GAMEPLAY` case of `Update()`, the final-batch check now polls `activeItems`:
```cpp
// currentBatch == maxBatch (3) AND activeEnemies is empty
if (activeItems.empty()) {
    EnterStatSelection(currentWave + 1);
}
```
The game loops here every frame, updating and rendering items, until all items have either been collected by the player or fallen off-screen (at which point their `isActive` flag is set to false and they are erased from `activeItems` by the cleanup loop). Only then does the game proceed.

> **Note on item fall speed:** Items fall at `100.0f * deltaTime` px/s (set in `Item::Update()`). For a screen height of ~900px, an item at the top takes ~9 seconds to fall off-screen. The delay is bounded by the slowest-falling item on screen.

---

### Change 4: Complete Board Wipe on New Wave

**Context:** If the wave transition happened while bullets (enemy eggs) or leftover items were active, they could persist into the new wave.

**Fix:** `GameManager::StartWave()` now calls `.clear()` on all entity vectors before setting up the `WAVE_INTRO` state:
```cpp
void GameManager::StartWave(int waveNumber) {
    activeBullets.clear();
    activeItems.clear();
    activeEnemies.clear();
    activeDamageTexts.clear();
    // ... set waveIntroTimer, currentWave, etc.
    ChangeState(GameState::WAVE_INTRO);
}
```
This is a hard reset — no entity from the previous wave can survive into the next.

---


