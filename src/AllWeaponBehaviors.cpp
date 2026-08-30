#include "../include/AllWeaponBehaviors.h"
#include "../include/HypergunShootingBehavior.h"
#include "../include/BeamWeapon.h"
#include "../include/GameManager.h"
#include "../include/Bullet.h"
#include "../include/BulletTrajectory.h"
#include "../include/SoundManager.h"
#include <algorithm>
#include <iostream>
#include <cmath>

void NeutronGunBehavior::Shoot(Spaceship* ship) {
    if (!ship) return;
    GameManager* gm = GameManager::GetInstance();
    Vector2 pos = ship->GetPosition();
    float dmg = ship->GetDamage();
    int lv = ship->GetLevel();

    int bulletType = (lv <= 3) ? 3 : ((lv <= 6) ? 4 : 5); // 3: Weak, 4: Medium, 5: Strong
    float radius = (lv <= 3) ? 12.0f : ((lv <= 6) ? 15.0f : 18.0f);
    float speed = (lv <= 3) ? 850.0f : ((lv <= 6) ? 920.0f : 1000.0f);

    // Cơ chế xòe quạt theo Level: từ 1 viên (Level 1) đến tối đa 11 viên xòe rộng rực rỡ (Level 11)
    int numShots = std::min(11, std::max(1, lv));
    float step = 8.5f; // Góc mở giãn cách giữa các viên đạn (độ)
    float startAngle = -((numShots - 1) * step) / 2.0f;

    for (int i = 0; i < numShots; ++i) {
        float angle = startAngle + i * step;
        // Dịch chuyển nhẹ theo chiều ngang khớp với vị trí nòng / cánh tương ứng góc bắn
        float offsetX = (numShots > 1) ? ((float)i - (numShots - 1) / 2.0f) * 6.0f : 0.0f;
        Vector2 spawnPos = { pos.x + offsetX, pos.y - 20.0f };
        
        auto bullet = std::make_shared<Bullet>(spawnPos, dmg, speed, true, bulletType, radius);
        // Áp dụng SpreadTrajectory để từng viên đạn tung ra theo góc độ hình quạt uy lực
        bullet->SetTrajectory(std::make_shared<SpreadTrajectory>(angle));
        bullet->SetShooter(ship);
        gm->AddBullet(bullet);
    }
    SoundManager::GetInstance()->PlayGunShot("Neutron_Gun");
}

void RiddlerBehavior::Shoot(Spaceship* ship) {
    if (!ship) return;
    GameManager* gm = GameManager::GetInstance();
    Vector2 pos = ship->GetPosition();
    float dmg = ship->GetDamage() * 0.8f;
    int lv = ship->GetLevel();

    int numBullets = std::min(13, 3 + lv);
    float maxAngle = 18.0f - std::min(10.0f, (float)lv * 0.8f); // Chụm dần ở cấp độ cao
    float step = (numBullets > 1) ? (maxAngle * 2.0f / (numBullets - 1)) : 0.0f;
    float startAngle = -maxAngle;

    for (int i = 0; i < numBullets; ++i) {
        float angle = startAngle + i * step;
        auto bullet = std::make_shared<Bullet>(pos, dmg, 650.0f, true, 6, 12.0f); // Type 6: Riddler
        bullet->SetTrajectory(std::make_shared<SpreadTrajectory>(angle));
        bullet->SetShooter(ship);
        gm->AddBullet(bullet);
    }
    SoundManager::GetInstance()->PlayGunShot("Riddler");
}

void IonBlasterBehavior::Shoot(Spaceship* ship) {
    if (!ship) return;
    GameManager* gm = GameManager::GetInstance();
    Vector2 pos = ship->GetPosition();
    float dmg = ship->GetDamage();
    int lv = ship->GetLevel();

    int bulletType = (lv < 5) ? 7 : 8; // 7: Ion Single, 8: Ion Double
    std::vector<float> angles = {0.0f};
    if (lv >= 2) { angles = {-15.0f, 15.0f}; }
    if (lv >= 4) { angles = {-20.0f, 0.0f, 20.0f}; }
    if (lv >= 6) { angles = {-30.0f, -15.0f, 15.0f, 30.0f}; }
    if (lv >= 8) { angles = {-35.0f, -18.0f, 0.0f, 18.0f, 35.0f}; }

    for (float ang : angles) {
        auto bullet = std::make_shared<Bullet>(pos, dmg, 550.0f, true, bulletType, 16.0f);
        bullet->SetTrajectory(std::make_shared<SpreadTrajectory>(ang));
        bullet->SetShooter(ship);
        gm->AddBullet(bullet);
    }
    SoundManager::GetInstance()->PlayGunShot("Ion_Blaster");
}

void UtensilPokerBehavior::Shoot(Spaceship* ship) {
    if (!ship) return;
    GameManager* gm = GameManager::GetInstance();
    Vector2 pos = ship->GetPosition();
    float dmg = ship->GetDamage() + 15.0f;
    int lv = ship->GetLevel();

    // Type 9: Fork, Type 10: Carving Knife (xen kẽ hoặc nâng cấp)
    int bulletType = (lv % 2 != 0) ? 9 : 10;
    int numUtensils = std::min(5, 1 + lv / 2);
    float offsets[] = {0.0f, -25.0f, 25.0f, -50.0f, 50.0f};

    for (int i = 0; i < numUtensils; ++i) {
        Vector2 spawnPos = { pos.x + offsets[i], pos.y };
        auto bullet = std::make_shared<Bullet>(spawnPos, dmg, 480.0f, true, bulletType, 15.0f);
        // Giảm tốc độ lắc lượn của dĩa (từ 3.0f xuống 2.5f)
        bullet->SetTrajectory(std::make_shared<OscillatingTrajectory>((i % 2 == 0 ? 0.0f : 5.0f), 2.5f, 14.0f));
        bullet->SetShooter(ship);
        gm->AddBullet(bullet);
    }
    SoundManager::GetInstance()->PlayGunShot("Utensil_Poker");
}

void PlasmaRifleBehavior::Shoot(Spaceship* ship) {
    // Sát thương tia Plasma liên tục được tính trực tiếp ở vòng lặp update beam trong GameManager
}

void LaserCannonBehavior::Shoot(Spaceship* ship) {
    // Sát thương tia Laser liên tục được tính trực tiếp ở vòng lặp update beam trong GameManager
}

std::unique_ptr<IShootingBehavior> CreateWeaponBehavior(const std::string& weaponName) {
    if (weaponName == "Hypergun")        return std::make_unique<HypergunShootingBehavior>();
    if (weaponName == "Plasma_Rifle")    return std::make_unique<PlasmaRifleBehavior>();
    if (weaponName == "Neutron_Gun")     return std::make_unique<NeutronGunBehavior>();
    if (weaponName == "Riddler")         return std::make_unique<RiddlerBehavior>();
    if (weaponName == "Lightning_Fryer") return std::make_unique<LightningFryerBehavior>();
    if (weaponName == "Ion_Blaster")     return std::make_unique<IonBlasterBehavior>();
    if (weaponName == "Utensil_Poker")   return std::make_unique<UtensilPokerBehavior>();
    if (weaponName == "Laser_Cannon")    return std::make_unique<LaserCannonBehavior>();
    
    // Mặc định trả về Hypergun
    return std::make_unique<HypergunShootingBehavior>();
}
