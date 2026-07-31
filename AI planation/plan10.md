# Implementation Plan 10

## Fixes and Improvements

1. **Build System Fix (Windows Compatibility):**
   - **File:** `Makefile`
   - **Details:** The previous `mkdir -p obj` command caused compilation errors on Windows `cmd.exe` (resulting in the error "A subdirectory or file obj already exists"). It was replaced with the Windows-compatible `if not exist $(OBJ_DIR)\ mkdir $(OBJ_DIR)`. This ensures `mingw32-make` compiles successfully on Windows without freezing VSCode's debugger by running outdated versions of the executable.

2. **Game Crash/Freeze Fix:**
   - **File:** `src/GameManager.cpp`
   - **Details:** Addressed a critical crash (`SIGSEGV`) occurring during game exit or initialization. Some textures (e.g., `texSpaceship`, `texBulletPlayer`) were defined and subsequently unloaded in `GameManager::CleanUp()`, despite never being initialized via `LoadTexture()`. Removed these invalid `UnloadTexture()` calls to prevent OpenGL from deleting uninitialized memory addresses.

3. **Level and EXP Cap Implementation:**
   - **File:** `src/Spaceship.cpp`
   - **Details:** Enforced a maximum level cap of 10. Once the spaceship reaches Level 10, it can no longer gain additional EXP. The EXP bar is explicitly reset to 0, and the `GainExp()` function immediately returns if the maximum level is reached, exactly as requested.

4. **Background Scrolling Speed Increase:**
   - **File:** `src/GameManager.cpp`
   - **Details:** Verified and finalized the 15% increase in background scrolling speed in the gameplay test state (background speed increased from baseline up to a multiplier of 23.0f).

5. **Fix Game Crash (ODR Violation):**
   - **Files:** `include/Enemy.h` and `src/GameManager.cpp`
   - **Details:** Fixed a segmentation fault crash caused by an ODR (One Definition Rule) violation in C++14. The `static constexpr float` variables (`TRAIL_SPAWN_INTERVAL`, `TRAIL_FADE_SPEED`) were declared in `Enemy.h` but used with non-constexpr variables (`deltaTime`), causing undefined behavior. Replaced them with local constants and literals in `GameManager.cpp`.

6. **Asteroid Visual Improvement:**
   - **File:** `src/GameManager.cpp`
   - **Details:** Redesigned the asteroid rendering logic to look more realistic. Instead of drawing large semi-transparent ghost sprites at previous positions (which looked like afterimages), the new logic draws the full combined sprite (trail + body) at the current position. It replaces the ghost sprites with small, colored particle circles (orange/fire for `asteroidFlame` and gray/stone for `asteroidNormal`) that fade out, creating a clean motion-trail effect.

7. **Fix Wave 3 Batch 3 Progression Bug:**
   - **File:** `src/GameManager.cpp`
   - **Details:** Fixed an issue where Wave 3 Batch 3 would never end, causing the game to get stuck. Tank Chickens were spawning too far off-screen (`y = -1900`) with a slow `SpiralMovement` (20px/s), taking nearly 100 seconds to appear on screen. Fixed this by spawning all enemies in Batch 3 within a reasonable off-screen range (`y = -100` to `-800`) and using `StraightMovement` for the tanks so they fall faster and appear quickly.

8. **Wave System Redesign (Batches & Formations):**
   - **File:** `src/GameManager.cpp`
   - **Details:** Completely overhauled the wave system. Each wave is now divided into 3 distinct "batches" (đợt). The game waits for the player to clear the current batch before transitioning to the next one. Implemented unique enemy formations and compositions up to Wave 4 (e.g., zig-zag formations, meteor showers, tank chicken drops, and swarms from both sides).

9. **Enemy Stat Scaling:**
   - **File:** `src/EnemyFactory.cpp`
   - **Details:** Implemented dynamic enemy scaling. Enemies now spawn with increased HP and Damage based on the current wave number to ensure the game remains balanced and challenging as the player upgrades their spaceship.

10. **Spaceship Speed & Bullet Boundary Adjustments:**
    - **Files:** `assets/spaceship/spaceship.csv`, `src/GameManager.cpp`
    - **Details:** Increased the base movement speed of the spaceship by 10-15% for smoother control. Additionally, fixed a boundary issue where the spaceship could not fire when positioned at the very bottom of the screen by extending the bullet deletion boundary (`delete threshold` increased to `1100`).

