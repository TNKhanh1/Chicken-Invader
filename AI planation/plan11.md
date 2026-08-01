# Core Mechanics: Argument System

## Overview

The **Argument** system is a Roguelike mechanic that allows players to choose **1 out of 3 random Arguments** at the end of Waves 5, 10, and 15 (before fighting a Boss).
Each Argument provides a special passive effect that shapes the playstyle for the rest of the run.

## Balancing Principles

- Each Argument is **strong in some situations, but weak in others** — no Argument is absolutely overpowered (OP).
- The player can only pick **1 Argument each time** (max 3 Arguments in a full 15-Wave run).
- Arguments **do not stack** their effects on top of each other (unless explicitly stated).

---

## List of 10 Arguments

### 1. EXP Amplifier
- **Effect:** Gain **+50% EXP** from all monsters for the rest of the run.
- **Balance:** No immediate power spike — benefits accumulate over time. Extremely strong if picked early (Wave 5), but very weak if picked late.

### 2. Stat Windfall
- **Effect:** Instantly gain **3 consecutive Stat selections**.
- **Balance:** Huge immediate power spike, but lacks any long-term passive scaling.

### 3. Abundant Gifts
- **Effect:** Starting from the next Stat Selection, the player will be offered **4 choices** instead of 3.
- **Balance:** Increases the probability of getting desired stats. Value scales with the number of remaining Stat selections.

### 4. Boss Hunter
- **Effect:** Deal **+80% Damage** to all Bosses.
- **Balance:** Only valuable during the 3 Boss fights. Useless against normal monsters.

### 5. Armor Crusher
- **Effect:** Every attack deals bonus damage equal to **3% of the target's current HP**.
- **Balance:** Extremely effective against high-HP monsters and Bosses, but weak against low-HP swarm enemies. Effectiveness diminishes as the target's HP drops.

### 6. Blood Fury
- **Effect:** Every time you kill a monster, gain **+2 permanent Damage** (uncapped stacking).
- **Balance:** Weak early on, but scales infinitely the more enemies you kill. Best picked early. No immediate combat advantage.

### 7. Bloodthirst
- **Effect:** Every time you kill a monster, **restore 15 HP**.
- **Balance:** Highly effective during swarm waves (Waves 1-4), but holds little value in Boss fights where there are few targets. Does not increase damage output.

### 8. Energy Flow
- **Effect:** Every time you fire your weapon, **restore 2 Mana**.
- **Balance:** Depends entirely on attack speed and mana usage. Synergizes perfectly with high-cost active skills.

### 9. Round Recovery
- **Effect:** **Restore 80 HP** at the start of every new Wave.
- **Balance:** Most effective when sustaining consistent damage. Strong early pick, weak late pick. Does not provide in-combat healing.

### 10. Fast Track
- **Effect:** Instantly **level up 3 times**.
- **Balance:** Very strong at early levels to unlock weapons and stats quickly. Weak at higher levels (Level cap is 10, meaning this is wasted if already close to the cap).

---

## Technical Implementation Details (What has been done)

### 1. Data Structure and Definitions
- Defined an array of `CardDef` for `ALL_ARGUMENTS` in `GameManager.cpp` matching the 10 designs above.
- Added `ArgumentType` enum and tracked selected arguments inside the `Spaceship` class (`std::vector<int> activeArguments`).
- Exposed `HasArgument(int id)` in `Spaceship` to quickly query active effects.

### 2. Game State Transition Logic
- Modifed the wave transition logic in `UpdateTestGameplay()`. 
- At the end of every wave, the game automatically enters `STAT_SELECTION`.
- If the finished wave is **5, 10, or 15**, a flag `pendingArgumentAfterStat` is set to `true`.
- After the player selects a Stat, the game checks `pendingArgumentAfterStat`. If true, it immediately transitions to `ARGUMENT_SELECTION` and resets the flag. If false, it proceeds to the next wave normally.
- This creates the flow: **Wave -> Stat Selection -> (If Wave 5/10/15) Argument Selection -> Next Wave**.

### 3. UI Rendering & Scalability
- We unified the UI layout logic for both Stat and Argument selection screens in `Draw()`.
- The background frame asset (`chiso.png` / `loi.png`) had a native bounding box of 186x287. We explicitly defined `srcRect = { 244, 29, 186, 287 }` to accurately extract the metallic frame from the 667x374 raw image.
- We scaled the cards up to `320x490` (`CARD_W = 320`, `CARD_H = 490`) to properly utilize the 1600x900 screen space and accommodate longer text strings.
- Implemented a dedicated `textArea` bounding box nested inside the `cardRect`. This `textArea` has explicitly calibrated top, bottom, and side paddings (`+40x`, `+110y`) to ensure that all text (Name, Divider line, Description) fits entirely inside the glass frame, dodging the deep, overhanging metal ornaments at the top border.
- The Card number indicator (1, 2, 3) is rendered perfectly over the top-left metal frame by accounting for the frame's cut-out corners.

### 4. Basic Stat Replacements
- Replaced the less impactful "Move Speed" and "Bullet Speed" with "Crit Chance" and "Crit Damage" in `ALL_STATS` to ensure all stat upgrades provide highly desirable, combat-defining benefits.
