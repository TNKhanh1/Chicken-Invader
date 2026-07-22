# Plan & Progress #05: Visual Effects and System UI Improvements

## Completed Objectives (Post Plan #04)

In this phase, we focused on making the game more dynamic and providing the player with more control through a settings system.

### 1. Item Drop Effects (Meat Drop)
- **Feature:** When chickens are destroyed, they now drop a meat item.
- **Trajectory Physics:** Instead of dropping straight down, the dropping items have been programmed with a combination of two physics mechanics:
  - **Parabolic gravity:** The item falls freely with increasing acceleration (`velocity.y += gravity * dt`).
  - **Sinusoidal movement:** The item sways left and right forming a sine wave trajectory (`sin(time) * amplitude`), simulating the natural bounce and scattering effect when the chicken explodes.

### 2. Seamless Scrolling Space Background
- **Feature:** Added a continuous vertically scrolling space background to simulate the spaceship flying forward.
- **Mirrored Seamless Technique:** To completely resolve the "seam" issue when tiling images (due to top and bottom edges not matching), the rendering system uses 3 texture tiles. The middle tile is **flipped vertically**, which perfectly aligns the pixels of the connecting edges, creating a flawless, infinitely scrolling space illusion.
- **Background Speed:** The scrolling speed was fine-tuned to 20 for a smoother and more comfortable visual experience.
- **New Asset:** Added `background.jpg` as the new default background.

### 3. Settings Menu System
- **Feature:** Introduced a new `SETTINGS` state into the `GameManager`'s `GameState` enum.
- **User Interface:** 
  - A Settings button (gear icon) is now placed at the top-right corner, globally accessible across `MAIN_MENU`, `TEST_MENU`, and even during `TEST_GAMEPLAY`.
  - Inside the Settings menu, players can seamlessly switch between 4 different backgrounds (1 default and 3 variations).
- **State Routing:** Implemented a `previousState` tracking variable. This ensures that when the player presses "BACK" in the Settings menu, they are returned to the exact screen they left (e.g., resuming an active gameplay test) without being forced back to the Main Menu.

### 4. UI Adjustments
- **Gameplay HUD Optimization:** Relocated the Score counter slightly lower and further left in the `TEST_GAMEPLAY` state to prevent it from overlapping with the Settings button.
- **Text Visibility:** Changed the Score text color to white to ensure it stands out against all dark space backgrounds.

---

