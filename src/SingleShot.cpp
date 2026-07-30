#include "SingleShot.h"
#include "Spaceship.h"
#include "Bullet.h"
#include "GameManager.h"
#include <memory>

void SingleShot::Shoot(Spaceship* ship) {
    if (!ship) return;
    auto bullet = std::make_shared<Bullet>(ship->GetPosition(), ship->GetDamage(), 480.0f, true);
    GameManager::GetInstance()->AddBullet(bullet);
    GameManager::GetInstance()->PlayShootSound();
}
