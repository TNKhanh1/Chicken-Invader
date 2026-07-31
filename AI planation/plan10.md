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
