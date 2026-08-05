# Plan 15: Wave Transition Animation System & Item Cleanup

## Overview
This plan details the creation of a brand new Wave Transition Animation System (the `WAVE_INTRO` state) and the implementation of crucial end-of-wave cleanup mechanics to ensure a smooth, bug-free, and visually stunning progression between waves in Chicken Invaders.

## Changes Implemented

### 1. Brand New 3-Phase Transition Animation (`WAVE_INTRO`)
* **Context:** Previously, the game lacked a transition sequence, immediately jumping into the next wave after stat selection.
* **Implementation:** Designed a cinematic 3-phase transition animation that lasts exactly 5.9 seconds:
  * **Phase 1 (Warp Speed - 3.4s):** The background rapidly accelerates to a hyper-speed scroll (`3000.0f` px/s) to simulate the spaceship warping to a new sector.
  * **Phase 2 (Text Show - 2.0s):** The background decelerates back to normal speed (`30.0f` px/s) while the "WAVE X" text fades in and remains displayed on the screen.
  * **Phase 3 (Text Fade - 0.5s):** The "WAVE X" text smoothly fades out, after which enemies immediately spawn.

### 2. Player Freedom During Transition
* **Implementation:** Added direct WASD polling and boundary clamping logic into the new `WAVE_INTRO` state within `GameManager::Update()`. This ensures the player isn't frozen; they can freely fly around the screen while the background is warping (shooting is disabled).

### 3. Item Collection Delay (End of Wave)
* **Issue:** When the last enemy of a wave was destroyed, the game immediately transitioned to the Stat Selection screen, causing players to lose out on uncollected items (drumsticks) that hadn't finished falling.
* **Fix:** Modified the `TEST_GAMEPLAY` logic. After clearing the final batch of enemies in a wave, the game now waits until `activeItems.empty()` is true. This ensures the player has ample time to collect dropping items (or for them to fall off-screen) before `EnterStatSelection()` is triggered.

### 4. Complete Board Wipe on New Wave
* **Issue:** Eggs (bullets), items, and floating damage texts from the previous wave could occasionally carry over into the new wave if skipped or spawned at the last millisecond.
* **Fix:** Added aggressive cleanup in the `GameManager::StartWave()` method. `activeBullets.clear()`, `activeItems.clear()`, `activeEnemies.clear()`, and `activeDamageTexts.clear()` are executed to ensure the new wave starts on a completely clean slate.
