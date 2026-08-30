#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include "raylib.h"
#include <map>
#include <string>

class SoundManager {
private:
    static SoundManager* instance;

    // Toggles
    bool soundOn;
    bool musicOn;

    // Assets
    Music backgroundMusic;
    Sound beepSound;
    Sound coinSound;
    Sound bossHitSound;
    Sound chickenHitSound;

    // Maps to store gun sounds
    std::map<std::string, Sound> gunSounds;
    std::map<std::string, Sound> beamSounds;

    // Variables for beam looping
    bool isBeamPlaying;
    std::string currentBeamWeapon;

    // Private constructor/destructor for Singleton
    SoundManager();
    ~SoundManager();

public:
    static SoundManager* GetInstance();
    static void DestroyInstance();

    void Init();
    void CleanUp();

    // Configuration
    bool IsSoundOn() const { return soundOn; }
    bool IsMusicOn() const { return musicOn; }
    void ToggleSound(bool on);
    void ToggleMusic(bool on);

    // Playback
    void UpdateMusic(); // Call every frame
    
    void PlayBeep();
    void PlayCoin();
    void PlayBossHit();
    void PlayChickenHit();
    void PlayGunShot(const std::string& weaponName);
    
    // For continuous beam firing
    void UpdateBeamSound(const std::string& weaponName, bool isFiring);
};

#endif // SOUND_MANAGER_H
