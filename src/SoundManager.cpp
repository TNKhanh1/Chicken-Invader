#include "../include/SoundManager.h"
#include <iostream>

SoundManager* SoundManager::instance = nullptr;

SoundManager::SoundManager() : soundOn(true), musicOn(true), isBeamPlaying(false), currentBeamWeapon("") {
}

SoundManager::~SoundManager() {
    // CleanUp() is called explicitly by GameManager::CleanUp()
}

SoundManager* SoundManager::GetInstance() {
    if (instance == nullptr) {
        instance = new SoundManager();
    }
    return instance;
}

void SoundManager::DestroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

void SoundManager::Init() {
    InitAudioDevice();

    backgroundMusic = LoadMusicStream("assets/sound/backgroundmusic.mp3"); 
    backgroundMusic.looping = true;
    SetMusicVolume(backgroundMusic, 0.6f);
    
    beepSound = LoadSound("assets/sound/beep.ogg"); 
    coinSound = LoadSound("assets/sound/coin.ogg");
    bossHitSound = LoadSound("assets/sound/boss.ogg");
    chickenHitSound = LoadSound("assets/sound/chicken.ogg");

    // Single shot sounds
    gunSounds["Hypergun"] = LoadSound("assets/sound/gun/hypergun.ogg");
    gunSounds["Neutron_Gun"] = LoadSound("assets/sound/gun/neutrongun.ogg");
    gunSounds["Riddler"] = LoadSound("assets/sound/gun/riddlergun.ogg");
    gunSounds["Ion_Blaster"] = LoadSound("assets/sound/gun/ionblaster.ogg");
    gunSounds["Utensil_Poker"] = LoadSound("assets/sound/gun/untensilpoker.ogg");

    // Beam sounds
    beamSounds["Lightning_Fryer"] = LoadSound("assets/sound/gun/lightningfryer.ogg");
    beamSounds["Plasma_Rifle"] = LoadSound("assets/sound/gun/plasmarifle.ogg");
    beamSounds["Laser_Cannon"] = LoadSound("assets/sound/gun/lazercannon.ogg");

    if (musicOn) {
        PlayMusicStream(backgroundMusic);
    }
}

void SoundManager::CleanUp() {
    UnloadMusicStream(backgroundMusic);
    UnloadSound(beepSound);
    UnloadSound(coinSound);
    UnloadSound(bossHitSound);
    UnloadSound(chickenHitSound);

    for (auto& pair : gunSounds) {
        UnloadSound(pair.second);
    }
    gunSounds.clear();

    for (auto& pair : beamSounds) {
        UnloadSound(pair.second);
    }
    beamSounds.clear();

    CloseAudioDevice();
}

void SoundManager::ToggleSound(bool on) {
    soundOn = on;
    if (!soundOn && isBeamPlaying) {
        if (beamSounds.find(currentBeamWeapon) != beamSounds.end()) {
            StopSound(beamSounds[currentBeamWeapon]);
        }
        isBeamPlaying = false;
    }
}

void SoundManager::ToggleMusic(bool on) {
    musicOn = on;
    if (musicOn) {
        PlayMusicStream(backgroundMusic);
    } else {
        StopMusicStream(backgroundMusic);
    }
}

void SoundManager::UpdateMusic() {
    if (musicOn) {
        UpdateMusicStream(backgroundMusic);
    }
}

void SoundManager::PlayBeep() {
    if (soundOn) {
        PlaySound(beepSound);
    }
}

void SoundManager::PlayCoin() {
    if (soundOn) {
        PlaySound(coinSound);
    }
}

void SoundManager::PlayBossHit() {
    if (soundOn) {
        if (!IsSoundPlaying(bossHitSound)) {
            PlaySound(bossHitSound);
        }
    }
}

void SoundManager::PlayChickenHit() {
    if (soundOn) {
        if (!IsSoundPlaying(chickenHitSound)) {
            PlaySound(chickenHitSound);
        }
    }
}

void SoundManager::PlayGunShot(const std::string& weaponName) {
    if (soundOn && gunSounds.find(weaponName) != gunSounds.end()) {
        PlaySound(gunSounds[weaponName]);
    }
}

void SoundManager::UpdateBeamSound(const std::string& weaponName, bool isFiring) {
    if (!soundOn) return;

    if (isFiring) {
        if (!isBeamPlaying || currentBeamWeapon != weaponName) {
            // Stop previous if changing weapon while firing
            if (isBeamPlaying && beamSounds.find(currentBeamWeapon) != beamSounds.end()) {
                StopSound(beamSounds[currentBeamWeapon]);
            }
            // Start new beam
            if (beamSounds.find(weaponName) != beamSounds.end()) {
                PlaySound(beamSounds[weaponName]);
            }
            currentBeamWeapon = weaponName;
            isBeamPlaying = true;
        } else {
            // Ensure looping
            if (beamSounds.find(currentBeamWeapon) != beamSounds.end() && 
                !IsSoundPlaying(beamSounds[currentBeamWeapon])) {
                PlaySound(beamSounds[currentBeamWeapon]);
            }
        }
    } else {
        // Stop firing
        if (isBeamPlaying) {
            if (beamSounds.find(currentBeamWeapon) != beamSounds.end()) {
                StopSound(beamSounds[currentBeamWeapon]);
            }
            isBeamPlaying = false;
        }
    }
}
