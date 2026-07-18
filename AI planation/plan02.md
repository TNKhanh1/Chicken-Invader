# Phase 2 Implementation Plan (Minimal Core)

This document outlines an extremely focused and minimal set of tasks for Phase 2. To keep the workload light, manageable, and ensure zero bugs, we will implement only the absolute bare minimum required to demonstrate the OOP architecture (Strategy Pattern) and a playable core loop. 

All UI states (Menus), complex weapons, and advanced wave systems are completely deferred.

---

## 1. Minimal Weapon System (Strategy Pattern)
We will implement the `IShootingBehavior` interface with only the most basic weapon to prove the architecture works perfectly.

* **`SingleShot`**
  * **Logic**: Fires one bullet moving straight up.
  * **OOP Benefit**: Directly implements the Strategy interface without any complex math, serving as a clean proof-of-concept.

---

## 2. Minimal Enemy Movement (Strategy Pattern)
We will implement the `IMovementBehavior` interface with a single, predictable path.

* **`StraightMovement`**
  * **Logic**: The enemy moves straight down the Y-axis at a constant speed (`y += speed * deltaTime`).
  * **OOP Benefit**: The simplest possible implementation of the Strategy interface, ensuring stable and predictable enemy behavior.

---

## 3. Basic Spawner & Collision (Test Loop)
To make the game testable and playable, we need a simple way to spawn enemies and detect when bullets hit them.

* **Basic Spawner**: A simple countdown timer. When it hits zero, it spawns a standard enemy at a random X coordinate from the top of the screen.
* **Basic Collision**: Use Raylib's built-in `CheckCollisionRecs()` to check if a bullet's rectangle intersects an enemy's rectangle. If true, both objects are destroyed.
