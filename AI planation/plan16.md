# Plan 16: Procedural Sprite Sheet Animation & OOP Refactoring

## 1. Overview
The objective of this plan is to modernize the enemy rendering system by introducing lifelike 2D sprite animations and enforcing strict Object-Oriented Programming (OOP) principles. The previous static image approach has been replaced with a fluid, procedural mesh-warped animation system, and rendering logic has been properly encapsulated within the respective entity classes.

---

## 2. Implemented Features

### 2.1 Python-Based Mesh Warping Animation System
To create a high-quality 12-frame animation loop from a single static image (`chicken01.png`), a procedural mesh-warping pipeline was developed using Python's `scikit-image` (`PiecewiseAffineTransform`).

**Warping Logic & Mathematics:**
- **Grid Generation:** A 9x9 control grid is overlaid on the 100x100 source image, dividing it into discrete transformable cells.
- **Whole-Wing Flapping:** Pivot points are established at the wing roots (`x = 30` and `x = 70`). The vertical displacement (`dy`) of the grid points increases linearly from the root to the wing tip based on a sine wave function `sin(phase)`. This ensures the entire wing acts as a cohesive, hinged structure rather than just distorting the edges.
- **Symmetrical Leg Spreading:** The grid points in the lower quadrant (where the legs are located, `y > 65`) are subjected to a horizontal displacement (`dx`). Left leg points move negatively, and right leg points move positively, synchronized symmetrically via a cosine envelope.
- **Momentum-Based Body Bobbing:** To simulate realistic physical momentum, the central body grid points dip downwards (`dy` is positive) precisely when the legs are fully extended outward and the wings are at the bottom of their stroke. This body bobbing is baked directly into the sprite sheet frames.
- **Output:** The pipeline produces a seamless 1200x100 sprite sheet (`chicken01_anim.png`) that runs smoothly at 12 FPS.

### 2.2 OOP & Architectural Refactoring
The game's rendering architecture has been modernized to adhere to strict OOP paradigms. Previously, entity-specific logic was tightly coupled and centralized within the `GameManager` class.

**Refactoring Steps:**
- **Encapsulation of Enemy Logic:** The `Enemy::Update(float deltaTime)` and `Enemy::Draw()` methods were entirely extracted from `GameManager.cpp` and implemented inside `Enemy.cpp`. 
- **Animation State Management:** The `Enemy` class is now fully responsible for managing its own animation lifecycle. It maintains the internal state variables `animTimer` (to track delta time accumulation) and `currentAnimFrame` (to dictate the current sprite sheet slice).
- **Encapsulation of Bullet Logic:** Similarly, `Bullet::Draw()` was extracted from `GameManager.cpp` and relocated to `Bullet.cpp`, granting the `Bullet` class full control over its rendering instructions.
- **Flyweight Pattern:** `GameManager` has been strictly relegated to the role of an asset manager. It loads the `Texture2D texEnemyAnim` into memory exactly once during initialization and provides it to the `Enemy` instances by reference, minimizing memory overhead.

---

## 3. Future Architecture for Remaining Chicken Variants

With the procedural mesh-warping system successfully deployed for `chicken01.png`, the architecture is now prepared to scale this solution across all 10 chicken variants (`chicken02.png` through `chicken10.png`).

### Step 1: Batch Asset Generation
- The existing `warp_anim_v2.py` script will be expanded into a batch-processing loop.
- The loop will iterate through the base images `chicken01.png` to `chicken10.png`.
- **Dynamic Pivot Calculation:** Because the variants possess different aspect ratios and dimensions (e.g., `chicken08.png` is wider, `chicken10.png` is taller), the script will dynamically read each image's width and height to calculate the proportional wing pivots and leg regions before applying the mathematical deformation.

### Step 2: C++ Asset Management Refactoring
- **Array Storage:** In `GameManager.h`, the single texture reference will be upgraded to an array: `Texture2D texEnemyAnims[10];`.
- **Batch Loading:** In `GameManager::Init()`, all 10 generated sprite sheets will be loaded into the array using a dynamic file path constructor (`snprintf`).
- **Resource Access:** A getter method `Texture2D& GetTexEnemyAnim(int index)` will be provided.

### Step 3: Enemy Variant Assignment
- **State Addition:** An `int chickenVariant;` property will be added to the `Enemy` class.
- **Initialization:** Upon instantiation, the `Enemy` constructor will assign a specific variant index (0-9) based on the wave configuration or RNG.
- **Dynamic Rendering:** The `Enemy::Draw()` method will fetch the texture matching its `chickenVariant` from the `GameManager` and render the appropriate frame. This strategy ensures O(1) rendering complexity while providing rich visual diversity across different game waves.
