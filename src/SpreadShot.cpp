#include "SpreadShot.h"
#include "Spaceship.h"
#include "Bullet.h"
#include "GameManager.h"
#include <memory>

void SpreadShot::Shoot(Spaceship* ship) {
    if (!ship) return;
    float dmg = ship->GetDamage();
    Vector2 pos = ship->GetPosition();
    
    auto b1 = std::make_shared<Bullet>(pos, dmg, 480.0f, true);
    auto b2 = std::make_shared<Bullet>(pos, dmg, 480.0f, true);
    auto b3 = std::make_shared<Bullet>(pos, dmg, 480.0f, true);
    
    b1->SetVelocity(Vector2{0.0f, -480.0f});
    b2->SetVelocity(Vector2{-100.0f, -480.0f});
    b3->SetVelocity(Vector2{100.0f, -480.0f});
    
    auto gm = GameManager::GetInstance();
    gm->AddBullet(b1);
    gm->AddBullet(b2);
    gm->AddBullet(b3);
    gm->PlayShootSound();
}
