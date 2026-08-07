# Chicken Animation Reference Guide

> This document is the definitive reference for creating and managing animated sprite sheets for all chicken enemy types.
> It covers the animation pipeline, the mathematical warp model, the C++ rendering path, and the design spec for 3 chicken tiers.

---

## 1. Animation Pipeline Overview

```
chicken01.png (static image)
        ↓
  [Python: batch_warp_v3.py]
        ↓
chicken01_anim.png (sprite sheet: 2400×100, 24 frames × 100×100)
        ↓
  [C++: GameManager loads into texEnemyAnims[0]]
        ↓
  [C++: Enemy::Draw() slices frame by currentAnimFrame]
```

### Tools Required
- Python 3.10+
- Libraries: `Pillow`, `scikit-image`, `numpy`
- Install: `pip install Pillow scikit-image numpy`

### Script Location
- `scratch/batch_warp_v3.py` — Current production script for all chickens (02–10)
- `scratch/warp_anim_v2.py` — Original script for chicken01 only (12 frames, legacy)

---

## 2. Input Requirements

### Source Image Specs
| Property | Requirement |
|---|---|
| Format | `.png` with RGBA transparency |
| Background | Fully transparent (alpha = 0) |
| Orientation | Facing forward (symmetrical left-right) |
| Content | Centered in the canvas, no padding |
| Naming | `chickenXX.png` where XX = 01 to 10 (zero-padded) |
| Location | `assets/enemy/` |

### Current Source Image Sizes
| File | Native Size | Notes |
|---|---|---|
| `chicken01.png` | 100×100 | Baseline reference |
| `chicken02.png` – `chicken07.png` | 150×150 | Standard variants |
| `chicken08.png` | 186×139 | Non-square |
| `chicken09.png` | 220×195 | Non-square |
| `chicken10.png` | 212×224 | Non-square |

> **IMPORTANT:** Native size does NOT matter. The animation script resizes ALL images to **100×100** before warping. This is what ensures visual consistency across all chicken types.

---

## 3. Animation Math Model

The animation is produced by applying a **PiecewiseAffineTransform** (mesh warp) to the source image 24 times with varying parameters.

### 3.1 Control Grid
- An **11×11 control grid** (121 points) is overlaid on the 100×100 image.
- Grid points are evenly spaced at ~10px intervals.
- Each frame displaces these grid points according to a sine wave phase.

### 3.2 Phase Calculation
```
phase = frame_index × (2π / 24)    // 0 to 2π over 24 frames
dip_factor = sin(phase)             // Ranges from -1.0 to +1.0
```

### 3.3 Wing Flapping
Two pivot points divide the chicken into 3 zones: left wing, body, right wing.

| Pivot | X Position | Zone |
|---|---|---|
| Left pivot | `x = 30` | Left wing: `x ∈ [0, 30]` |
| Body | — | Body: `x ∈ (30, 70)` |
| Right pivot | `x = 70` | Right wing: `x ∈ [70, 100]` |

**Displacement formula (vertical):**
```
For left wing:  dy = ((30 - x) / 30) × 15.0 × dip_factor
For right wing: dy = ((x - 70) / 30) × 15.0 × dip_factor
```
- Wing tips move ±15px vertically at maximum amplitude.
- Movement decreases linearly toward the pivot (hinge effect).

### 3.4 Body Bobbing
All grid points receive a global vertical displacement:
```
body_dy = 4.0 × dip_factor
```
- The body dips 4px downward when wings are at their lowest stroke.

### 3.5 Leg Spreading
Grid points below `y = 65` (bottom 35% of image) spread horizontally:
```
leg_y_weight = (y - 65) / (100 - 65)    // 0.0 at y=65, 1.0 at y=100

For left leg (x < 50):  dx = -6.0 × dip_factor × leg_y_weight
For right leg (x > 50): dx = +6.0 × dip_factor × leg_y_weight
```
- Maximum leg spread: ±6px at the bottom edge.

### 3.6 Summary of Constants

| Parameter | Value | Effect |
|---|---|---|
| `num_frames` | 24 | Animation loop length |
| `left_pivot` | 30.0 | Left wing hinge point |
| `right_pivot` | 70.0 | Right wing hinge point |
| `wing_amplitude` | 15.0 | Max vertical wing displacement (px) |
| `body_dy` | 4.0 | Max body bobbing (px) |
| `leg_amplitude` | 6.0 | Max horizontal leg spread (px) |
| `leg_threshold` | 65.0 | Y coordinate where leg region begins |
| `grid_size` | 11×11 | Control point density |

> These constants are calibrated for 100×100 images. **Do not change them** unless you want a different animation feel for ALL chickens.

---

## 4. Output Sprite Sheet Format

| Property | Value |
|---|---|
| Dimensions | `2400 × 100` (24 frames × 100px wide × 100px tall) |
| Layout | Horizontal strip (all frames side by side) |
| Format | RGBA PNG |
| Naming | `chickenXX_anim.png` |
| Location | `assets/enemy/` |

### Frame Indexing
```
Frame 0: srcRect = { 0, 0, 100, 100 }
Frame 1: srcRect = { 100, 0, 100, 100 }
...
Frame 23: srcRect = { 2300, 0, 100, 100 }
```

---

## 5. C++ Rendering Path

### 5.1 Loading (GameManager.cpp)
```cpp
// Init: load all 10 sprite sheets
for (int i = 0; i < 10; i++) {
    char path[100];
    snprintf(path, sizeof(path), "assets/enemy/chicken%02d_anim.png", i + 1);
    texEnemyAnims[i] = LoadTexture(path);
}
```

### 5.2 Frame Advancement (Enemy.cpp)
```cpp
// 30 FPS animation (Update method)
animTimer += deltaTime;
if (animTimer >= 1.0f / 30.0f) {
    animTimer -= 1.0f / 30.0f;
    currentAnimFrame = (currentAnimFrame + 1) % 24;
}
```

### 5.3 Rendering (Enemy::Draw)
```cpp
Texture2D tex = gm->GetTexEnemyAnim(visualId - 1);
float frameSize = (float)tex.height;  // = 100.0f for all standardized chickens
Rectangle srcRec = { currentAnimFrame * frameSize, 0, frameSize, frameSize };

float base = baseSizeForType();  // Determines display size based on role
Rectangle destRec = { position.x, position.y, base, base };
DrawTexturePro(tex, srcRec, destRec, origin, tiltAngle, tintColor);
```

### 5.4 Size by Role (baseSizeForType)
Currently returns `100.0f` for all roles. This needs to be updated for the 3-tier system (see Section 6).

---

## 6. Three-Tier Chicken Design Spec

### 6.1 Tier Overview

| Tier | Role in JSON | Display Size | Hitbox | Example Chicken |
|---|---|---|---|---|
| **Normal** | `"NORMAL"` or `"SWARM"` | 100×100 | 50×50 | chicken01–chicken07 |
| **Large** | `"TANK"` | 150×150 | 75×75 | chicken08–chicken09 |
| **Boss** | `"BOSS"` | 200×200 | 100×100 | chicken10 |

### 6.2 Implementation: baseSizeForType()

To enable 3-tier sizing, update `Enemy::baseSizeForType()` in `Enemy.cpp`:

```cpp
float Enemy::baseSizeForType() const {
    switch (role) {
        case EnemyRole::NORMAL:
        case EnemyRole::SWARM:
        case EnemyRole::ASTEROID:
            return 100.0f;
        case EnemyRole::TANK:
            return 150.0f;
        case EnemyRole::BOSS:
            return 200.0f;
        default:
            return 100.0f;
    }
}
```

And update hitbox accordingly:

```cpp
Rectangle Enemy::GetHitbox() const {
    float base = baseSizeForType();
    float hitSize = base * 0.5f;  // hitbox is 50% of display size
    return { position.x - hitSize/2, position.y - hitSize/2, hitSize, hitSize };
}
```

### 6.3 Visual Effects by Tier

| Effect | Normal | Large | Boss |
|---|---|---|---|
| Tilt on movement | ±12° | ±8° (slower, heavier) | ±5° (massive, steady) |
| Hit flash color | `{255, 80, 80}` | `{255, 120, 40}` (orange) | `{255, 255, 0}` (yellow) |
| Hit shake amplitude | ±3px | ±5px | ±8px |
| HP bar | On hover only | Always visible | Always visible + thicker |
| Death effect | Simple fade | Explosion particles | Large explosion + screen shake |

> These effects are **not yet implemented** — they are design targets for future work. Currently all tiers use the same visual effects.

### 6.4 Animation Considerations

The animation sprite sheets are all standardized to 100×100 per frame. Since `baseSizeForType()` controls the **destination rectangle** size, the same 100×100 sprite sheet is simply **scaled up** by `DrawTexturePro`:

- Normal (100×100 dest) → 1:1 pixel-perfect rendering
- Large (150×150 dest) → 1.5× upscale (slightly softer but acceptable)
- Boss (200×200 dest) → 2× upscale (may need higher-res source)

### 6.5 High-Resolution Boss Sprites (Future)

If the 2× upscale for bosses looks too blurry, you can create a separate high-res animation:

1. Set `target_size=200` in `batch_warp_v3.py` for the boss chicken only.
2. Output will be `4800×200` (24 frames × 200×200).
3. In `Enemy::Draw()`, `frameSize = tex.height` will automatically be `200.0f`.
4. `DrawTexturePro` will render 200×200 source to 200×200 dest → pixel-perfect.

No C++ code changes needed — the dynamic `frameSize = tex.height` already handles this!

---

## 7. How to Add a New Chicken Variant (Step by Step)

### Step 1: Create the Source Image
- Draw or obtain a forward-facing chicken PNG with transparent background.
- Save as `assets/enemy/chickenXX.png` (any resolution is fine).

### Step 2: Generate the Animation
```bash
cd e:\ChickenInvader_local\ChickenInvader
python scratch/batch_warp_v3.py
```
This regenerates ALL chicken02–10 animations. To generate only one:
```python
generate_warped_sprite_sheet("assets/enemy/chicken11.png", "assets/enemy/chicken11_anim.png")
```

### Step 3: Update C++ (if adding chicken11+)
- `GameManager.h`: Increase array size from `[10]` to `[11]` (or higher).
- `GameManager.cpp`: Update the `for` loop upper bound from `10` to `11`.
- `GameManager.h`: Update bounds check in `GetTexEnemyAnim()`.

### Step 4: Use in JSON
```json
{
    "visual_id": 11,
    "role": "NORMAL",
    ...
}
```

### Step 5: Test
- Compile: `mingw32-make`
- Open test menu → Select the stage/wave using the new chicken
- Verify animation plays smoothly at 30 FPS
- Verify size matches the intended tier
