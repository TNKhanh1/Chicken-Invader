# Implementation Plan: Audio & Sound System Integration

## Goal Description
Implement a comprehensive audio system using the `SoundManager` (Singleton Pattern). This handles UI sounds (beep, coin), enemy hit sounds, background music, and weapon-specific sounds (single shots and continuous beam sounds). We will add ON/OFF toggles for Sound Effects and Music in the Settings menu. The provided assets in `assets/sound/` and `assets/sound/gun/` will be used.

## Proposed Changes

### Sound Manager Component
This component strictly follows the Singleton design pattern to encapsulate all audio-related logic.

#### [MODIFY] include/SoundManager.h (Update logic)
- Update paths and variables to use the `.ogg` formats and exact filenames from `assets/sound/`.

#### [MODIFY] src/SoundManager.cpp (Update logic)
- Load audio files:
  - `assets/sound/backgroundmusic.mp3`
  - `assets/sound/beep.ogg`
  - `assets/sound/coin.ogg`
  - `assets/sound/boss.ogg`
  - `assets/sound/chicken.ogg`
- Load gun sounds:
  - `"Hypergun"` -> `assets/sound/gun/hypergun.ogg`
  - `"Neutron_Gun"` -> `assets/sound/gun/neutrongun.ogg`
  - `"Riddler"` -> `assets/sound/gun/riddlergun.ogg`
  - `"Ion_Blaster"` -> `assets/sound/gun/ionblaster.ogg`
  - `"Utensil_Poker"` -> `assets/sound/gun/untensilpoker.ogg`
- Load beam sounds:
  - `"Lightning_Fryer"` -> `assets/sound/gun/lightningfryer.ogg`
  - `"Plasma_Rifle"` -> `assets/sound/gun/plasmarifle.ogg`
  - `"Laser_Cannon"` -> `assets/sound/gun/lazercannon.ogg`
- `UpdateBeamSound()` handles continuous sounds when the player fires a beam weapon, ensuring the sound loops correctly and stops instantly when the mouse/key is released.

### Settings Menu & GameManager Updates
#### [MODIFY] src/GameManager.cpp
- **Settings Menu**: Add "Sound: ON/OFF" and "Music: ON/OFF" buttons.
- **Update loop**: Call `SoundManager::GetInstance()->UpdateMusic()`.
- **Initialization**: Call `SoundManager::GetInstance()->Init()`.
- **Cleanup**: Call `SoundManager::GetInstance()->CleanUp()`.
- **Beam Logic**: Hook `ProcessBeamWeapon` execution logic to dynamically check if a beam weapon is firing and call `SoundManager::GetInstance()->UpdateBeamSound()`.

### UI Audio Triggers
#### [MODIFY] src/TitleScreen.cpp
#### [MODIFY] src/SummaryScreen.cpp
#### [MODIFY] src/MenuManager.cpp
- Inject `SoundManager::GetInstance()->PlayBeep()` inside all standard button click handlers.

#### [MODIFY] src/ShopUI.cpp
#### [MODIFY] src/RuneSelectionUI.cpp
- For navigation/tab buttons: Inject `PlayBeep()`.
- For purchasing logic: Play `PlayCoin()` if the item is bought successfully. Play `PlayBeep()` if the player already owns the item, fails to buy, or just clicks the tab. 

### Enemy Hit Sounds
#### [MODIFY] src/Enemy.cpp
- In `Enemy::TakeDamage(float)`: Check the `role` enum.
- Call `PlayBossHit()` if `role == EnemyRole::BOSS`, otherwise `PlayChickenHit()`.

### Weapon Sounds
#### [MODIFY] src/AllWeaponBehaviors.cpp
#### [MODIFY] src/HypergunShootingBehavior.cpp
#### [MODIFY] src/SingleShot.cpp
#### [MODIFY] src/SpreadShot.cpp
- Inside `Shoot()` methods, replace the obsolete `GameManager::PlayShootSound()` calls with specific `SoundManager::GetInstance()->PlayGunShot(weaponName)`.
