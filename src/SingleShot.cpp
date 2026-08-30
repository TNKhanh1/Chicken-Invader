#include "SingleShot.h"
#include "Spaceship.h"
#include "Bullet.h"
#include "GameManager.h"
#include "SoundManager.h"
#include <memory>

void SingleShot::Shoot(Spaceship* ship) {
    if (!ship) return;
    auto bullet = std::make_shared<Bullet>(ship->GetPosition(), ship->GetDamage(), 480.0f, true);
    bullet->SetShooter(ship);
    GameManager::GetInstance()->AddBullet(bullet);
    SoundManager::GetInstance()->PlayGunShot("Hypergun");
}
