# Plan 13: End-to-End Daily Summary (Bug Fixes, Integrations, UI & Balancing)

This document summarizes the full scope of work performed today, starting from debugging teammate integration issues, reviewing UI code, to implementing core game balancing and global font systems.

## 0. Git Pull Debugging: Object File Conflicts & Build Pipeline
**The Problem:** After pulling the latest commit from a teammate, the game failed to run properly. This was not a code error, but a Git tracking issue: compiled object files (`.o` and the `obj/` folder) were accidentally tracked and pushed to the repository, causing the local compiler (`make`) to use incompatible binaries or skip recompiling entirely.
**The Fix:** 
1. **Git Cleanup:** We updated `.gitignore` to ignore `*.o` and the `obj/` directory, and ran `git rm --cached` to permanently purge them from Git tracking. This guarantees that only source code (`.cpp`, `.h`) is shared across machines.
2. **VSCode Task (The JSON Fix):** We modified `.vscode/tasks.json` to add a new task called `"Clean & Rebuild Game"`. If the build system ever gets confused by branch switching or timestamp changes, the player can trigger this task from VSCode to run `make clean && make game`, ensuring a flawless fresh build.


## 2. Weapon Fire Rate & Animation Balancing
**Goal:** The global attack speed scaling made late-game firing animations (especially for Beam Weapons and the Utensil Poker) too erratic and overpowered. We needed a clean way to apply a 15% global nerf and smooth out animations without breaking the base logic.

**Implementation Details:**
- **Global Attack Speed Nerf:** Modified `Spaceship::GetAttackSpeed()` to multiply the final returned attack speed by `0.85f` (a 15% global reduction). We also ensured `Spaceship::Fire()` exclusively calls `GetAttackSpeed()` instead of directly reading the `attackSpeed` variable so the nerf applies uniformly.
- **Beam Damage Balancing:** For laser/beam weapons (e.g., Plasma Rifle, Absolver Beam), we applied the same `0.85f` multiplier to their `damageRate` (DPS) in `GameManager.cpp` to ensure their DPS scales fairly alongside projectile weapons.
- **Animation Tweaks (Súng 6 & Súng 8):**
  - **Lightning Fryer (Súng 6):** Removed the 60Hz high-frequency jitter and implemented a localized pseudo-random tick system (updating the lightning path 50 times per second). This makes the electric arc look powerful but less chaotic.
  - **Utensil Poker (Súng 8):** Reduced the frequency of the `OscillatingTrajectory` from `3.0f` to `2.5f`, making the forks wobble more predictably.

---

## 3. Floating Combat Text (Damage Numbers)
**Goal:** Enhance the visual feedback when dealing damage by displaying floating numbers (White for normal hits, Red and larger for critical hits) that fade out smoothly. 

**Implementation Details (OOP & Performance):**
- **Data Structure (`DamageText`):** Created a lightweight `DamageText` struct in `GameManager.h`.
- **Memory Management:** Added `std::vector<DamageText> activeDamageTexts` to `GameManager` to manage the lifecycle of these texts independently from Enemies or Bullets (Single Responsibility Principle).
- **Update & Draw Logic:** 
  - Every frame, each damage text floats upwards (`position.y -= 40.0f * deltaTime`) and its alpha is reduced toward the end of its lifetime.
  - Normal hits last `0.45s` (size 20). Critical hits last `0.7s` (size 26) and are colored Red.
- **Critical Hit Calculation (Projectile vs. Beam):**
  - **Projectiles:** A dice roll triggers a crit. The damage is multiplied by the crit modifier, and a single floating text pops up.
  - **Beam Weapons:** Beams deal damage continuously. To prevent text spam, we introduced `beamTextTimer`. The damage text for beams only pops up **once every 0.25 seconds**. It accumulates the DPS dealt over that quarter-second and displays it as a single chunk. Crit calculations are still evaluated correctly behind the scenes!

---

## 4. Global Custom Font System (Vietnamese Support)
**Goal:** Raylib's default font is rigid and lacks support for Vietnamese accented characters (e.g., "Lõi", "Chỉ số"). Updating every single `DrawText` call across multiple files to `DrawTextEx` would be extremely tedious and error-prone. 

**Implementation Details (Macro Magic):**
- **Loading the Font:** In `GameManager::Init()`, we loaded `assets/FONT.ttf` using `LoadFontEx`. To support Vietnamese, we manually allocated an array of over 600 Unicode Codepoints (spanning Basic Latin, Latin-1, and Latin Extended Additional) and passed it to the font loader.
- **The Wrapper Functions:** Created `GameManager::DrawTextCustom` and `GameManager::MeasureTextCustom` which natively handle the custom font and correct text scaling.
- **C++ Variadic Macros (The Hack):**
  Instead of replacing 40+ lines of code, we injected a global macro into `GameManager.h`:
  ```cpp
  #define DrawText(text, x, y, size, ...) GameManager::GetInstance()->DrawTextCustom(text, x, y, size, __VA_ARGS__)
  #define MeasureText(text, size) GameManager::GetInstance()->MeasureTextCustom(text, size)
  ```
  - *Why `__VA_ARGS__`?* Inline struct initializers for colors (like `{255, 255, 255, 255}`) contain commas. The C-preprocessor mistakes these commas for separate macro arguments. By using variadic macros (`...`), we force the preprocessor to capture the entire color struct as a single argument and safely pass it to our custom wrapper.
  - *Result:* Every single file that includes `GameManager.h` (such as `UIManager.cpp`) automatically compiles using our custom font, perfectly measured and perfectly centered, without changing any of their internal code!

## 5. Polishing & Commit Details
- Re-centered the Main Menu title dynamically using the new `MeasureText` logic.
- Renamed the game title to exactly **"CHICKEN INVADERS"** (Removed "OOP" from the window bar and the main screen).
- **Enemy Asset Update:** Switched the default enemy sprite from `chicken03.png` (unused/deleted) back to `chicken01.png` for consistency.

### Recommended Git Commits
To keep the history clean, here are the suggested English commit messages for today's work:

1. `fix: Purge tracked .o files from git and add Clean & Rebuild VSCode task`
2. `feat: Implement RPG floating damage text and critical hit logic`
3. `balance: Apply global 15% fire rate nerf and smooth out beam weapon animations`
4. `feat: Integrate global custom font with Vietnamese support via C++ macros`
5. `fix: Update default enemy texture reference to chicken01.png`
