# Plan 06: Gameplay Systems, Rendering Architecture, and Wave Management

This document outlines the architectural design and technical implementation for the new gameplay features and systems.

## 1. Test Gameplay Menu (Level / Wave / Batch Selection)
**Goal:** Provide a testing interface to jump directly into specific waves or batches without playing through the entire game.

### Architecture & Implementation
- **UI State Management:** Introduced a new UI state `TEST_MENU` in `UIManager`. This menu is accessed via a dedicated "Test Gameplay" button on the Main Menu.
- **Selection Logic:** Created interactive UI buttons to increment or decrement `selectedLevel`, `selectedWave`, and `selectedBatch` variables.
- **Integration with GameManager:** Upon initiating the test, the `UIManager` passes the selected parameters to `GameManager::InitGame()`. The `GameManager` overrides its internal progression counters and immediately spawns the specified scenario using `StartWave()`.

## 2. Rectangle Formation (Wave 1, Batch 3)
**Goal:** Spawn enemies in a solid rectangular formation that moves as a single cohesive block across the screen.

### Architecture & Implementation
- **Grid Layout Calculation:** Enemies are spawned using a strict nested loop (Rows x Columns) to assign exact `(x, y)` coordinates for each enemy relative to a central anchor point.
- **Synchronized Movement:** All enemies within the formation share the same `HorizontalBounceMovement` strategy with identical parameters (speed, boundaries). By ensuring their initial relative offsets are mathematically precise, the uniform velocity updates keep the entire block perfectly structured as it bounces between the screen edges.

## 3. Continuous Firing System (Spaceship)
**Goal:** Enhance the shooting mechanics by implementing a hold-to-shoot system regulated by the spaceship's attack speed.

### Architecture & Implementation
- **Input Handling:** Utilizes continuous key state polling (`IsKeyDown(KEY_SPACE)`) to detect sustained fire commands.
- **Cooldown Regulation:** Integrated a `fireTimer` mechanism within the `Spaceship` class. The timer decreases based on `deltaTime` and restricts bullet instantiation until it reaches zero.
- **Execution:** When the cooldown is met, the spaceship delegates bullet spawning to its `WeaponStrategy`. The timer is subsequently reset to `1.0f / attackSpeed`, ensuring the fire rate remains strictly tied to the spaceship's attributes.
- **Combat Tuning:** Player bullet speed and base spaceship damage are balanced to maintain an optimal combat pace against incoming enemy waves.

## 4. Asteroid Rendering System
**Goal:** Render complex asteroid sprites featuring massive dust trails and non-centered physics.

### Architecture & Implementation
- **Grid Parsing:** The `asteroidNormal.png` sprite sheet is parsed as a **15x2 grid** (15 columns, 2 rows), yielding a `512x1024` resolution per frame (a 1:2 aspect ratio).
- **Rendering & Scaling:** The `512x1024` frame is scaled to a `100x200` rendering rectangle via Raylib's `DrawTexturePro`. This sizing maintains the physical rock at an optimal visual dimension (~100x100) while accurately projecting the massive dust trail upwards.
- **Hitbox Alignment (Origin Offset):** The physical rock resides at the bottom of the texture. To align the game's positional hitbox with the rock, the `origin` vector for rotation and drawing is offset to `(50.0f, 160.0f)`. This shifts the rendering pivot downwards, allowing the visual dust trail to overlap other entities naturally without triggering false collisions.
- **Animation Sequence:** The animation advances frame-by-frame sequentially from left to right (row-major order), completing a smooth 30-frame loop to simulate a continuous falling trajectory.

## 5. Scalable Wave & Batch Architecture
**Goal:** Establish a robust architectural foundation for managing complex levels containing multiple waves, batches, and enemy types.

### Architecture & Implementation
To handle a game with numerous levels, each containing up to 15 waves broken down into multiple batches (đợt), the system utilizes a **Data-Driven Architecture** combined with the **Factory and Strategy Patterns**.

#### Data Structure
Levels are defined in external configuration files (e.g., JSON). A dedicated parser reads these files to construct the level dynamically:
- **Level:** Represents a stage, containing a queue of `Wave` objects.
- **Wave:** Represents a distinct phase. Contains a queue of `Batch` objects and is considered cleared when all batches are defeated.
- **Batch (Đợt):** The smallest spawn unit. Contains configurations for:
  - `EnemyType` (e.g., Normal, Tank, Asteroid, Boss)
  - `Count` (Number of enemies)
  - `SpawnDelay` (Time between individual enemy spawns)
  - `MovementBehavior` (e.g., Dive, Zigzag, Bounce)
  - `WeaponStrategy` (e.g., Single, Spread)

#### Code Implementation Outline
```cpp
struct BatchConfig {
    EnemyFactory::EnemyType enemyType;
    int count;
    float spawnDelay;
    MovementType moveType;
    WeaponType weaponType;
};

struct WaveConfig {
    std::vector<BatchConfig> batches;
    float delayBeforeWave;
};

class LevelManager {
private:
    std::queue<WaveConfig> upcomingWaves;
    WaveConfig currentWave;
    float waveTimer;
    
public:
    void LoadLevel(const std::string& filepath); 
    void Update(float deltaTime); 
};
```
- **LevelManager:** Reads the configuration and acts as the director, passing `BatchConfig` instructions to the `GameManager` and `EnemyFactory`.
- **EnemyFactory & Strategies:** Instantiates enemies dynamically based on the requested `EnemyType`, attaching the designated `IMovementBehavior` and `IWeaponStrategy`. This completely decouples level design from core game logic.
