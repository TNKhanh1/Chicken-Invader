#include "Spaceship.h"
#include "AllWeaponBehaviors.h"
#include "SpaceshipDataManager.h"
#include "GameManager.h"
#include <iostream>
#include <cmath>

Spaceship::Spaceship(std::string n, Vector2 pos, float hp, float dmg, float arm, float spd, 
                     float critC, float critD, float mana, float atkSpd)
    : Character(pos, hp, dmg, arm, spd), 
      critChance(critC), critDamage(critD), maxMana(mana), currentMana(0), attackSpeed(atkSpd),
      level(1), currentExp(0), maxExp(100), name(n) {
    prevPosition = pos;
}

void Spaceship::AddObserver(IObserver* observer) {
    observers.push_back(observer);
}

void Spaceship::RemoveObserver(IObserver* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Spaceship::Notify(EventType event, const std::string& data) {
    for (auto observer : observers) {
        observer->OnNotify(event, data);
    }
}

float Spaceship::GetCritChance() const { return critChance; }
float Spaceship::GetCritDamage() const { return critDamage; }
float Spaceship::GetMaxMana() const { return maxMana; }
float Spaceship::GetCurrentMana() const { return currentMana; }
float Spaceship::GetAttackSpeed() const { 
    // Giảm tốc bắn 15% toàn cục theo yêu cầu (Global Fire Rate Nerf)
    return attackSpeed * 0.85f; 
}
int Spaceship::GetLevel() const { return isManaActive ? 11 : level; }
void Spaceship::SetLevel(int newLevel) {
    level = newLevel;
    if (level < 1) level = 1;
    if (level > 11) level = 11;
}
float Spaceship::GetCurrentExp() const { return currentExp; }
float Spaceship::GetMaxExp() const { return maxExp; }
std::string Spaceship::GetName() const { return name; }

Rectangle Spaceship::GetHitbox() const {
    return {position.x - 20, position.y - 20, 40, 40};
}

void Spaceship::Init() {}

void Spaceship::Update(float deltaTime) {
    if (fireTimer > 0.0f) {
        fireTimer -= deltaTime;
    }
    
    // Cập nhật animation giật lùi nảy súng (Weapon Recoil Animation)
    if (recoilTimer > 0.0f) {
        recoilTimer -= deltaTime;
        if (recoilTimer <= 0.0f) {
            recoilTimer = 0.0f;
            recoilOffset = 0.0f;
        } else {
            float progress = 1.0f - (recoilTimer / recoilDuration);
            if (progress <= 0.15f) {
                // Giai đoạn 1: giật nảy chớp nhoáng siêu tốc (15% chu kỳ đầu)
                recoilOffset = maxRecoilDistance * (progress / 0.15f);
            } else {
                // Giai đoạn 2: lướt nhanh phục hồi về vị trí cũ (85% chu kỳ sau)
                float recovery = (progress - 0.15f) / 0.85f;
                recoilOffset = maxRecoilDistance * (1.0f - recovery);
            }
        }
    } else {
        recoilOffset = 0.0f;
    }
    
    // Cập nhật hệ thống đuôi năng lượng và động cơ phản lực (Thruster Energy Trail)
    float dx = position.x - prevPosition.x;
    float dy = position.y - prevPosition.y;
    float distMoved = sqrt(dx * dx + dy * dy);

    if (deltaTime > 0.0f && distMoved > 0.1f) {
        // Tàu đang di chuyển: gia tăng cường độ sáng lửa đuôi dựa trên tốc độ và quãng đường
        float currentSpeed = distMoved / deltaTime;
        float moveBoost = (currentSpeed / (moveSpeed > 0 ? moveSpeed : 300.0f)) * 1.5f;
        thrusterIntensity += moveBoost * 4.0f * deltaTime;
        if (thrusterIntensity > 2.5f) thrusterIntensity = 2.5f;

        // Lực quán tính: lái sang ngang khiến ngọn lửa đuôi nghiêng sang hướng ngược lại
        float targetTilt = -dx * 1.8f;
        if (targetTilt > 28.0f) targetTilt = 28.0f;
        if (targetTilt < -28.0f) targetTilt = -28.0f;
        thrusterTiltX = thrusterTiltX + (targetTilt - thrusterTiltX) * (12.0f * deltaTime);

        // Tiến lùi theo trục Y: phóng tới (dy < 0) làm lửa đuôi vươn dài, lùi sau (dy > 0) lửa thu ngắn
        if (dy < -0.5f) {
            thrusterLengthMult = 1.0f + std::min(0.8f, (-dy / (moveSpeed * 0.016f)) * 0.4f);
        } else if (dy > 0.5f) {
            thrusterLengthMult = 0.7f;
        } else {
            thrusterLengthMult = 1.0f;
        }
    } else {
        // Tàu đứng yên: lửa lui về trạng thái chờ êm ái (Idle glow), độ nghiêng phục hồi về chính tâm
        thrusterTiltX *= (1.0f - 8.0f * deltaTime);
        thrusterLengthMult = 1.0f;
    }

    // Khấu hao (Decay) cường độ sáng dần lui về mức nền 0.3f khi ngừng bứt tốc hoặc xả đạn
    if (thrusterIntensity > 0.3f) {
        thrusterIntensity -= 1.6f * deltaTime;
        if (thrusterIntensity < 0.3f) thrusterIntensity = 0.3f;
    }

    // Cập nhật trạng thái Mana
    if (isManaActive) {
        currentMana -= (maxMana / 5.0f) * deltaTime; // Tiêu hao hết trong 5 giây
        if (currentMana <= 0.0f) {
            currentMana = 0.0f;
            DeactivateMana();
        }
        Notify(EventType::PLAYER_MANA_CHANGED, std::to_string(currentMana));
    }

    prevPosition = position;
}

void Spaceship::Die() {}

void Spaceship::SetShootingBehavior(std::unique_ptr<IShootingBehavior> behavior) {
    shootingBehavior = std::move(behavior);
}

void Spaceship::ReloadStatsFromCSV() {
    SpaceshipStats stats = SpaceshipDataManager::GetInstance()->GetStats(name, level);
    maxHp = stats.hp;
    currentHp = maxHp; 
    damage = stats.damage;
    armor = stats.armor;
    moveSpeed = stats.moveSpeed;
    critChance = stats.critChance;
    critDamage = stats.critDamage;
    maxMana = stats.maxMana;
    currentMana = maxMana;
    attackSpeed = stats.attackSpeed;

    std::cout << "[STAT SCALING] Reloaded CSV Stats -> Ship: " << name 
              << " | Level: " << level 
              << " | HP: " << maxHp 
              << " | DMG: " << damage 
              << " | AS: " << attackSpeed 
              << " | MoveSpd: " << moveSpeed 
              << " | Crit: " << critChance << "%" << std::endl;
}

void Spaceship::SetWeapon(const std::string& weaponName) {
    name = weaponName;
    currentWeapon = weaponName;
    SetShootingBehavior(CreateWeaponBehavior(weaponName));
    ReloadStatsFromCSV();
}

bool Spaceship::CanFire() const {
    return fireTimer <= 0.0f;
}

void Spaceship::TriggerRecoil() {
    float atkSpd = GetAttackSpeed();
    float fireInterval = (atkSpd > 0.0f) ? (1.0f / atkSpd) : 0.25f;
    
    // Thời gian animation rút ngắn siêu tốc để giật nhanh và gắt hơn (tối thiểu 30ms, tối đa 120ms)
    recoilDuration = std::min(fireInterval * 0.5f, 0.12f);
    if (recoilDuration < 0.03f) recoilDuration = 0.03f;
    
    // Tăng cường độ giật cực đại: tối thiểu 12px (kể cả súng tia Laser bắn siêu tốc) và tối đa 26px với súng đại bác bắn chậm
    maxRecoilDistance = std::min(26.0f, std::max(12.0f, 50.0f / (atkSpd > 0.0f ? atkSpd : 5.0f)));
    
    recoilTimer = recoilDuration;

    // Xả tải buồng đốt khi bóp cò nã súng: cường độ sáng đuôi gia tăng mạnh phụ thuộc tốc độ bắn (Attack Speed)
    float shootBoost = std::min(1.0f, 0.3f + (atkSpd * 0.06f));
    thrusterIntensity = std::min(2.5f, thrusterIntensity + shootBoost);
}

float Spaceship::GetRecoilOffset() const {
    return recoilOffset;
}

float Spaceship::GetThrusterIntensity() const { return thrusterIntensity; }
float Spaceship::GetThrusterTiltX() const { return thrusterTiltX; }
float Spaceship::GetThrusterLengthMult() const { return thrusterLengthMult; }

void Spaceship::Fire() {
    if (shootingBehavior) {
        shootingBehavior->Shoot(this);
        TriggerRecoil(); // Kích hoạt hiệu ứng phản lực nảy giật súng (Juice VFX)
        
        if (GetAttackSpeed() > 0) {
            fireTimer = 1.0f / GetAttackSpeed();
        }
        GainMana(10.0f);
        if (HasArgument(7)) { // 7: Energy Flow
            GainMana(2.0f);
        }
    }
}

void Spaceship::GainExp(float amount) {
    if (level >= 10) return; // Level tối đa là 10, không nhận thêm exp
    if (HasArgument(0)) { // 0: EXP Amplifier
        amount *= 1.5f;
    }
    currentExp += amount;
    Notify(EventType::PLAYER_EXP_GAINED, std::to_string(currentExp));
    if (currentExp >= maxExp) {
        LevelUp();
    }
}

void Spaceship::GainMana(float amount) {
    if (isManaActive) return; // Không hồi mana khi đang dùng

    if (currentMana < maxMana) {
        currentMana += amount;
        if (currentMana > maxMana) currentMana = maxMana;
        Notify(EventType::PLAYER_MANA_CHANGED, std::to_string(currentMana));
    }
}

void Spaceship::ActivateMana() {
    if (currentMana >= maxMana && !isManaActive) {
        isManaActive = true;
        // Tùy chỉnh thêm các buff nếu cần
    }
}

void Spaceship::DeactivateMana() {
    isManaActive = false;
}

void Spaceship::LevelUp() {
    if (level >= 10) return; // Đã đạt max level
    level++;
    currentExp -= maxExp;
    maxExp *= 1.2f;
    
    // Tái đồng bộ chỉ số từ CSV theo nguyên tắc OOP Data-Driven và DRY
    ReloadStatsFromCSV();
    
    if (level >= 10) {
        currentExp = 0; // Đặt exp về 0 khi max level
    }

    Notify(EventType::PLAYER_LEVEL_UP, std::to_string(level));
}

void Spaceship::Heal(float amount) {
    if (currentHp < maxHp) {
        currentHp += amount;
        if (currentHp > maxHp) currentHp = maxHp;
    }
}

void Spaceship::AddArgument(int argId) {
    if (std::find(activeArguments.begin(), activeArguments.end(), argId) == activeArguments.end()) {
        activeArguments.push_back(argId);
    }
}

bool Spaceship::HasArgument(int argId) const {
    return std::find(activeArguments.begin(), activeArguments.end(), argId) != activeArguments.end();
}

