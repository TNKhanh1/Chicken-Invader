#include "Bosses.h"
#include "Item.h"
#include "Meat.h"
#include "Spaceship.h"
#include <cmath>

// --- Custom Boss Bullets ---

GrenadeBullet::GrenadeBullet(Vector2 startPos, Vector2 velocity) 
    : Bullet(startPos, 100.0f, 0.0f, false) {
    SetVelocity(velocity);
}

void GrenadeBullet::Draw() {
    if (!isActive) return;
    GameManager* gm = GameManager::GetInstance();
    Texture2D tex = gm->texGrenade;
    if (tex.id != 0) {
        DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, 
                       {position.x, position.y, 30.0f, 40.0f}, {15.0f, 20.0f}, 0.0f, WHITE);
    } else {
        DrawCircle(position.x, position.y, 20, GREEN);
    }
}

KnifeBullet::KnifeBullet(Vector2 startPos, Vector2 velocity) 
    : Bullet(startPos, 150.0f, 0.0f, false) {
    SetVelocity(velocity);
}

void KnifeBullet::Draw() {
    if (!isActive) return;
    GameManager* gm = GameManager::GetInstance();
    Texture2D tex = gm->texKnife;
    Vector2 vel = GetVelocity();
    float angle = atan2(vel.y, vel.x) * (180.0f / PI) + 90.0f;
    if (tex.id != 0) {
        DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, 
                       {position.x, position.y, 32.0f, 100.0f}, {16.0f, 50.0f}, angle, WHITE);
    } else {
        DrawCircle(position.x, position.y, 10, GRAY);
    }
}

EggBullet::EggBullet(Vector2 startPos, Vector2 velocity) 
    : Bullet(startPos, 50.0f, 0.0f, false) {
    SetVelocity(velocity);
}

void EggBullet::Draw() {
    if (!isActive) return;
    Texture2D tex = GameManager::GetInstance()->GetTexEnemyBullet();
    if (tex.id != 0) {
        DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, 
                       {position.x, position.y, 24.0f, 30.0f}, {12.0f, 15.0f}, 0.0f, WHITE);
    } else {
        DrawCircle(position.x, position.y, 12, YELLOW);
    }
}

// --- Boss Base Class ---

Boss::Boss(int visualId, const EnemyStats& stats, Vector2 pos) 
    : Enemy(visualId, EnemyRole::BOSS, stats, pos), 
      drumstickDropTimer(0.0f), wobbleAngle(0.0f), wobbleTimer(0.0f),
      battleTime(0.0f), targetPos(pos) {
}

void Boss::Update(float deltaTime) {
    Enemy::Update(deltaTime);
    
    battleTime += deltaTime;
    
    // Drumstick drop logic (mỗi 5 giây)
    drumstickDropTimer += deltaTime;
    if (drumstickDropTimer >= 5.0f) {
        drumstickDropTimer = 0.0f;
        auto gm = GameManager::GetInstance();
        gm->AddItem(std::make_shared<Meat>(position, Vector2{0.0f, -200.0f}));
    }
    
    // Random movement logic
    float dx = targetPos.x - position.x;
    float dy = targetPos.y - position.y;
    float dist = sqrt(dx*dx + dy*dy);
    if (dist < 10.0f) {
        auto gm = GameManager::GetInstance();
        targetPos.x = (float)GetRandomValue(150, gm->GetScreenWidth() - 150);
        targetPos.y = (float)GetRandomValue(100, gm->GetScreenHeight() / 2 - 100);
    } else {
        float moveS = 150.0f * deltaTime; // Tốc độ di chuyển
        position.x += (dx / dist) * moveS;
        position.y += (dy / dist) * moveS;
    }
    
    // Wobble effect
    wobbleTimer += deltaTime * 3.0f; 
    wobbleAngle = sin(wobbleTimer) * 20.0f; 
}

void Boss::Draw() {
    if (!isActive) return;

    auto gm = GameManager::GetInstance();
    Texture2D tex = gm->GetTexEnemyAnim(visualId - 1);
    
    float destW = 350.0f;
    float destH = 350.0f;
    Vector2 origin = { destW / 2.0f, destH / 2.0f };
    
    float frameSize = (float)tex.height;
    Rectangle srcRec = { (float)currentAnimFrame * frameSize, 0, frameSize, frameSize };
    
    Color tintColor = WHITE;
    if (hitFlashTimer > 0.0f) tintColor = { 255, 80, 80, 255 };

    Rectangle destRec = { position.x, position.y, destW, destH };
    DrawTexturePro(tex, srcRec, destRec, origin, wobbleAngle, tintColor);
}

Rectangle Boss::GetHitbox() const {
    return {position.x - 100, position.y - 100, 200, 200};
}

// --- Military Chicken Boss ---

MilitaryChickenBoss::MilitaryChickenBoss(int visualId, const EnemyStats& stats, Vector2 pos) 
    : Boss(visualId, stats, pos), grenadeTimer(0.0f), knifeTimer(0.0f), knifePhase(0), knifeDelay(0.0f) {
}

void MilitaryChickenBoss::DropGrenades() {
    for (int i = 0; i < 5; ++i) {
        float angle = GetRandomValue(45, 135) * (PI / 180.0f);
        float speed = (float)GetRandomValue(150, 300);
        Vector2 vel = { cos(angle) * speed, sin(angle) * speed };
        GameManager::GetInstance()->AddBullet(std::make_shared<GrenadeBullet>(position, vel));
    }
}

void MilitaryChickenBoss::ThrowKnives(Spaceship* player) {
    if (!player) return;
    
    Vector2 pPos = player->GetPosition();
    Vector2 diff = { pPos.x - position.x, pPos.y - position.y };
    float len = sqrt(diff.x * diff.x + diff.y * diff.y);
    if (len == 0.0f) len = 1.0f;
    
    float speed = 500.0f;
    Vector2 vel = { (diff.x / len) * speed, (diff.y / len) * speed };
    
    // Ném 1 dao trực tiếp hướng về player
    GameManager::GetInstance()->AddBullet(std::make_shared<KnifeBullet>(position, vel));
}

void MilitaryChickenBoss::Update(float deltaTime) {
    Boss::Update(deltaTime);
    
    auto gm = GameManager::GetInstance();
    auto player = gm->GetPlayer();
    
    // Cứ 2 phút (120s), giảm cooldown xuống 80%
    float cooldownMult = std::max(0.2f, (float)pow(0.8, floor(battleTime / 120.0f)));
    
    grenadeTimer += deltaTime;
    if (grenadeTimer >= (GetRandomValue(600, 800) / 100.0f) * cooldownMult) {
        grenadeTimer = 0.0f;
        DropGrenades();
    }
    
    knifeTimer += deltaTime;
    if (knifeTimer >= (GetRandomValue(400, 500) / 100.0f) * cooldownMult) {
        knifeTimer = 0.0f;
        if (player && player->IsActive()) {
            ThrowKnives(player.get());
        }
    }
}

// --- Super Chick Boss ---

SuperChickBoss::SuperChickBoss(int visualId, const EnemyStats& stats, Vector2 pos) 
    : Boss(visualId, stats, pos), eggBurstTimer(0.0f) {
}

void SuperChickBoss::FireEggBurst() {
    int numEggs = 16;
    float speed = 150.0f;
    for (int i = 0; i < numEggs; ++i) {
        float angle = (i * (360.0f / numEggs)) * (PI / 180.0f);
        Vector2 vel = { cos(angle) * speed, sin(angle) * speed };
        GameManager::GetInstance()->AddBullet(std::make_shared<EggBullet>(position, vel));
    }
}

void SuperChickBoss::Update(float deltaTime) {
    Boss::Update(deltaTime);
    
    // Cứ 2 phút (120s), giảm cooldown xuống 80%
    float cooldownMult = std::max(0.2f, (float)pow(0.8, floor(battleTime / 120.0f)));
    
    eggBurstTimer += deltaTime;
    if (eggBurstTimer >= (GetRandomValue(300, 400) / 100.0f) * cooldownMult) {
        eggBurstTimer = 0.0f;
        FireEggBurst();
    }
}
