#include "Bosses.h"
#include "Item.h"
#include "Meat.h"
#include "Spaceship.h"
#include "FontManager.h"
#include <cmath>
#include <raymath.h>

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
      drumstickDropTimer(5.0f), wobbleAngle(0.0f), wobbleTimer(0.0f), battleTime(0.0f) {
    canShoot = false;
    drawScale = 1.0f;
    targetPos = pos;
}

void Boss::Update(float deltaTime) {
    Enemy::Update(deltaTime);
    
    auto gm = GameManager::GetInstance();
    if (gm->IsBossCutscene()) {
        float cTimer = gm->GetCutsceneTimer();
        // Fly down from -200 to 300 during the first 2 seconds
        if (cTimer <= 2.0f) {
            float startY = -200.0f;
            float endY = 300.0f;
            float t = cTimer / 2.0f;
            // Ease out cubic
            t = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
            position.y = startY + t * (endY - startY);
        } else {
            position.y = 300.0f;
        }
        targetPos = position; // keep target pos in sync
        return; // Skip normal movement and attacks
    }
    
    battleTime += deltaTime;
    
    // Drumstick drop logic removed as requested
    
    // Random movement logic
    float dx = targetPos.x - position.x;
    float dy = targetPos.y - position.y;
    float dist = sqrt(dx*dx + dy*dy);
    if (dist < 10.0f) {
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
    
    float destW = 350.0f * drawScale;
    float destH = 350.0f * drawScale;
    Vector2 origin = { destW / 2.0f, destH / 2.0f };
    
    float frameSize = (float)tex.height;
    Rectangle srcRec = { (float)currentAnimFrame * frameSize, 0, frameSize, frameSize };
    
    Color tintColor = WHITE;
    if (hitFlashTimer > 0.0f) tintColor = { 255, 80, 80, 255 };

    Rectangle destRec = { position.x, position.y, destW, destH };
    DrawTexturePro(tex, srcRec, destRec, origin, wobbleAngle, tintColor);

    // Hiển thị bong bóng thoại trong Cutscene
    if (gm->IsBossCutscene()) {
        float cTimer = gm->GetCutsceneTimer();
        if (cTimer > 2.0f && cTimer <= 8.0f) {
            std::string line1, line2;
            if (visualId == 12) { // Military Chicken
                line1 = "You've come far, pilot...";
                line2 = "but this is your end!";
            } else if (visualId == 13) { // Super Chick
                line1 = "Prepare to be";
                line2 = "scrambled!";
            }
            
            if (!line1.empty()) {
                float typeDuration = 2.0f; // Typewriter takes 2 seconds
                float typeTimer = cTimer - 2.0f;
                int totalChars = line1.length() + line2.length();
                int charsToShow = (int)((typeTimer / typeDuration) * totalChars);
                if (charsToShow > totalChars) charsToShow = totalChars;
                
                std::string disp1 = line1.substr(0, std::min(charsToShow, (int)line1.length()));
                int rem = charsToShow - (int)line1.length();
                std::string disp2 = rem > 0 ? line2.substr(0, rem) : "";
                
                int w1 = FontManager::GetInstance()->MeasureGameText(line1, 20, "Modern").x;
                int w2 = FontManager::GetInstance()->MeasureGameText(line2, 20, "Modern").x;
                int maxW = std::max(w1, w2);
                int totalH = line2.empty() ? 20 : 45;
                
                // Bubble position
                float bX = position.x + (visualId == 12 ? -maxW - 40.0f : 40.0f); 
                float bY = position.y - 120.0f;
                
                Rectangle bubbleRec = { bX - 10, bY - 10, (float)maxW + 20, (float)totalH + 20 };
                DrawRectangleRounded(bubbleRec, 0.2f, 10, Fade(WHITE, 0.9f));
                DrawRectangleRoundedLinesEx(bubbleRec, 0.2f, 10, 2.0f, BLACK);
                
                // Bubble tail
                Vector2 p1, p2, p3;
                if (visualId == 12) { // Tail points right
                    p1 = { bX + maxW - 20, bY + totalH + 10 };
                    p3 = { bX + maxW + 10, bY + totalH + 40 };
                    p2 = { bX + maxW, bY + totalH + 10 };
                } else { // Tail points left
                    p1 = { bX + 10, bY + totalH + 10 };
                    p3 = { bX - 10, bY + totalH + 40 };
                    p2 = { bX + 30, bY + totalH + 10 };
                }
                DrawTriangle(p1, p3, p2, Fade(WHITE, 0.9f));
                DrawLineV(p1, p3, BLACK);
                DrawLineV(p3, p2, BLACK);
                
                // Draw text
                FontManager::GetInstance()->DrawGameText(disp1, bX, bY, 20, BLACK, "Modern");
                if (!disp2.empty()) {
                    FontManager::GetInstance()->DrawGameText(disp2, bX, bY + 25, 20, BLACK, "Modern");
                }
            }
        }
    }
}

Rectangle Boss::GetHitbox() const {
    float hw = 100.0f * drawScale;
    float hh = 100.0f * drawScale;
    return { position.x - hw, position.y - hh, hw * 2.0f, hh * 2.0f };
}

// --- Military Chicken Boss ---

MilitaryChickenBoss::MilitaryChickenBoss(int visualId, const EnemyStats& stats, Vector2 pos) 
    : Boss(visualId, stats, pos), grenadeTimer(0.0f), knifeTimer(0.0f), knifePhase(0), knifeDelay(0.0f) {
}

void MilitaryChickenBoss::DropGrenades() {
    for (int i = 0; i < 5; ++i) {
        float angle = GetRandomValue(45, 135) * (PI / 180.0f);
        float speed = (float)GetRandomValue(150, 300);
        Vector2 vel = { cosf(angle) * speed, sinf(angle) * speed };
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
    if (gm->IsBossCutscene()) return; // Không tấn công trong Cutscene
    
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
        Vector2 vel = { cosf(angle) * speed, sinf(angle) * speed };
        GameManager::GetInstance()->AddBullet(std::make_shared<EggBullet>(position, vel));
    }
}

void SuperChickBoss::Update(float deltaTime) {
    Boss::Update(deltaTime);
    
    if (GameManager::GetInstance()->IsBossCutscene()) return; // Không tấn công trong Cutscene
    
    // Cứ 2 phút (120s), giảm cooldown xuống 80%
    float cooldownMult = std::max(0.2f, (float)pow(0.8, floor(battleTime / 120.0f)));
    
    eggBurstTimer += deltaTime;
    if (eggBurstTimer >= (GetRandomValue(300, 400) / 100.0f) * cooldownMult) {
        eggBurstTimer = 0.0f;
        FireEggBurst();
    }
}

// ============================================================
// --- FireBullet Implementation ---
// ============================================================

FireBullet::FireBullet(Vector2 startPos, Vector2 velocity, float dmg, Type type)
    : Bullet(startPos, dmg, 0.0f, false, 0, type == Type::NORMAL ? 15.0f : 40.0f),
      type(type), glowPulse(0.0f), lifeTimer(0.0f), initialVelocity(velocity) {
    SetVelocity(velocity);
}

void FireBullet::Update(float deltaTime) {
    Bullet::Update(deltaTime);
    glowPulse += deltaTime * 8.0f;
    
    if (type == Type::EXPLOSIVE && isActive) {
        lifeTimer += deltaTime;
        if (lifeTimer >= 0.8f) { // Explodes after 0.8 seconds
            isActive = false;
            // Spawn 16 normal bullets
            int numBullets = 16;
            float speed = 250.0f;
            auto gm = GameManager::GetInstance();
            for (int i = 0; i < numBullets; ++i) {
                float angle = (i * (360.0f / numBullets)) * (PI / 180.0f);
                Vector2 vel = { cosf(angle) * speed, sinf(angle) * speed };
                gm->AddBullet(std::make_shared<FireBullet>(position, vel, GetDamage() * 0.5f, Type::NORMAL));
            }
        }
    }
}

void FireBullet::Draw() {
    if (!isActive) return;

    float p = 0.8f + 0.2f * sinf(glowPulse);
    if (type == Type::NORMAL) {
        float outerR = 25.0f * p;
        float innerR = 12.0f;
        DrawCircleGradient((int)position.x, (int)position.y, outerR, {255, 120, 20, 180}, {255, 60, 0, 0});
        DrawCircleGradient((int)position.x, (int)position.y, innerR, {255, 255, 180, 255}, {255, 200, 50, 200});
    } else {
        float outerR = 60.0f * p;
        float midR = 35.0f;
        float innerR = 18.0f;
        DrawCircleGradient((int)position.x, (int)position.y, outerR, {255, 100, 0, 180}, {255, 60, 0, 0});
        DrawCircleGradient((int)position.x, (int)position.y, midR, {255, 160, 30, 200}, {255, 80, 0, 60});
        DrawCircleGradient((int)position.x, (int)position.y, innerR, {255, 255, 180, 255}, {255, 200, 50, 200});
    }
}

// ============================================================
// --- FirePhoenixBoss Implementation ---
// ============================================================

FirePhoenixBoss::FirePhoenixBoss(int visualId, const EnemyStats& stats, Vector2 pos,
                                 float p1Hp, float p2Hp)
    : Boss(visualId, stats, pos),
      currentPhase(Phase::PHASE_1),
      phase1MaxHp(p1Hp),
      phase2MaxHp(p2Hp),
      transitionTimer(0.0f),
      isInvulnerable(false),
      transitionFlashCount(0),
      transitionFlashTimer(0.0f),
      attackTimer(0.0f),
      normalAttackCount(0),
      normalAttacksBeforeSkill(4),
      attackCooldown(2.0f),
      nextSkillType(0),
      sparkSpawnTimer(0.0f) {
    // Boss tự quản lý tấn công, không dùng egg system
    canShoot = false;
}

// --- Phase Transition ---

void FirePhoenixBoss::StartTransition() {
    currentPhase = Phase::TRANSITIONING;
    isInvulnerable = true;
    transitionTimer = 2.6f; // Tăng thời gian chuyển phase thêm 30%
    transitionFlashCount = 0;
    transitionFlashTimer = 0.0f;
    
    // Đứng bất động
    targetPos = position;

    // Bắn một vòng đạn lửa xung quanh (360 độ)
    auto gm = GameManager::GetInstance();
    int numBullets = 24;
    float bulletSpeed = 200.0f;
    float bulletDmg = 30.0f;
    for (int i = 0; i < numBullets; i++) {
        float angle = (i * (360.0f / numBullets)) * (PI / 180.0f);
        Vector2 vel = { cosf(angle) * bulletSpeed, sinf(angle) * bulletSpeed };
        gm->AddBullet(std::make_shared<FireBullet>(position, vel, bulletDmg, FireBullet::Type::NORMAL));
    }

    // Burst sparks khi bắt đầu chuyển dạng (vụ nổ tia lửa)
    for (int i = 0; i < 40; i++) {
        SparkParticle sp;
        sp.pos = position;
        float angle = (float)GetRandomValue(0, 360) * (PI / 180.0f);
        float speed = (float)GetRandomValue(100, 350);
        sp.velocity = { cosf(angle) * speed, sinf(angle) * speed };
        sp.alpha = 1.0f;
        sp.size = (float)GetRandomValue(3, 8);
        // Màu ngẫu nhiên: vàng / cam / đỏ
        int colorChoice = GetRandomValue(0, 2);
        if (colorChoice == 0) sp.color = {255, 255, 100, 255};      // Vàng sáng
        else if (colorChoice == 1) sp.color = {255, 160, 30, 255};  // Cam
        else sp.color = {255, 80, 20, 255};                         // Đỏ
        sparks.push_back(sp);
    }
}

void FirePhoenixBoss::UpdateTransition(float deltaTime) {
    transitionTimer -= deltaTime;

    // Nhấp nháy liên tục (rất nhanh)
    transitionFlashTimer += deltaTime;
    if (transitionFlashTimer >= 0.08f) {
        transitionFlashTimer = 0.0f;
        transitionFlashCount++;
    }

    if (transitionTimer <= 0.0f) {
        EnterPhase2();
    }
}

void FirePhoenixBoss::EnterPhase2() {
    currentPhase = Phase::PHASE_2;
    isInvulnerable = false;

    // Hồi full HP phase 2
    maxHp = phase2MaxHp;
    currentHp = phase2MaxHp;

    // Khi chuyển phase xong, bắn thêm 1 vòng đạn 360 độ
    auto gm = GameManager::GetInstance();
    int numBullets = 24;
    float bulletSpeed = 250.0f;
    float bulletDmg = 35.0f;
    for (int i = 0; i < numBullets; i++) {
        float angle = (i * (360.0f / numBullets)) * (PI / 180.0f);
        Vector2 vel = { cosf(angle) * bulletSpeed, sinf(angle) * bulletSpeed };
        gm->AddBullet(std::make_shared<FireBullet>(position, vel, bulletDmg, FireBullet::Type::NORMAL));
    }

    // Phase 2 params: tấn công nhiều hơn, nhanh hơn
    normalAttacksBeforeSkill = 3;
    attackCooldown = 1.1f; // Bắn nhanh hơn nhiều (trước là 1.8f)
    normalAttackCount = 0;
    attackTimer = 0.0f;
    nextSkillType = 0;
}

// --- TakeDamage & Die Override ---

void FirePhoenixBoss::TakeDamage(float incomingDamage) {
    if (isInvulnerable) return; // Bất tử trong lúc chuyển dạng
    Boss::TakeDamage(incomingDamage);
}

void FirePhoenixBoss::Die() {
    if (currentPhase == Phase::PHASE_1) {
        // Phase 1 hết HP → chuyển dạng, KHÔNG chết
        StartTransition();
    } else {
        // Phase 2 hết HP → chết thực sự
        Enemy::Die();
    }
}

// --- Attack Methods ---

void FirePhoenixBoss::FireNormalAttack() {
    auto gm = GameManager::GetInstance();
    float bulletSpeed = 300.0f;
    float bulletDmg = 30.0f;

    int numBullets = (currentPhase == Phase::PHASE_2) ? 9 : 5;
    float spreadAngle = (currentPhase == Phase::PHASE_2) ? 30.0f : 15.0f;

    auto player = gm->GetPlayer();
    float baseAngle = 90.0f * (PI / 180.0f);
    if (player && player->IsActive()) {
        Vector2 target = player->GetPosition();
        baseAngle = atan2(target.y - position.y, target.x - position.x);
    }

    for (int i = 0; i < numBullets; i++) {
        // Tính góc spread: phân bố đều từ -spreadAngle đến +spreadAngle
        float angleOffset = 0.0f;
        if (numBullets > 1) {
            angleOffset = -spreadAngle + (2.0f * spreadAngle * i) / (numBullets - 1);
        }
        float rad = baseAngle + angleOffset * (PI / 180.0f);
        Vector2 vel = { cosf(rad) * bulletSpeed, sinf(rad) * bulletSpeed };
        gm->AddBullet(std::make_shared<FireBullet>(position, vel, bulletDmg, FireBullet::Type::NORMAL));
    }

    normalAttackCount++;
}

void FirePhoenixBoss::FireLargeFireball() {
    auto gm = GameManager::GetInstance();
    float bulletSpeed = 200.0f;
    float bulletDmg = 80.0f;

    auto player = gm->GetPlayer();
    Vector2 vel = { 0.0f, bulletSpeed };
    if (player && player->IsActive()) {
        Vector2 target = player->GetHitbox().y < position.y ? Vector2{player->GetHitbox().x, position.y + 500.0f} : Vector2{player->GetHitbox().x, player->GetHitbox().y};
        float angle = atan2(target.y - position.y, target.x - position.x);
        vel = { cosf(angle) * bulletSpeed, sinf(angle) * bulletSpeed };
    }

    int numFireballs = (currentPhase == Phase::PHASE_2) ? 5 : 3;
    float spreadAngle = 15.0f;

    float baseAngle = atan2(vel.y, vel.x);
    for (int i = 0; i < numFireballs; i++) {
        float angleOffset = -spreadAngle + (2.0f * spreadAngle * i) / (numFireballs > 1 ? numFireballs - 1 : 1);
        if (numFireballs == 1) angleOffset = 0.0f;
        float rad = baseAngle + angleOffset * (PI / 180.0f);
        Vector2 finalVel = { cosf(rad) * bulletSpeed, sinf(rad) * bulletSpeed };
        gm->AddBullet(std::make_shared<FireBullet>(position, finalVel, bulletDmg, FireBullet::Type::EXPLOSIVE));
    }

    normalAttackCount = 0;
    nextSkillType = 1; // Lần sau dùng Fire Rain
}

void FirePhoenixBoss::FireRain() {
    auto gm = GameManager::GetInstance();
    float bulletSpeed = 180.0f;
    float bulletDmg = 20.0f;

    int numBullets = (currentPhase == Phase::PHASE_2) ? 40 : 24;

    for (int i = 0; i < numBullets; i++) {
        float angle = (i * (360.0f / numBullets)) * (PI / 180.0f);
        Vector2 vel = { cosf(angle) * bulletSpeed, sinf(angle) * bulletSpeed };
        gm->AddBullet(std::make_shared<FireBullet>(position, vel, bulletDmg, FireBullet::Type::NORMAL));
    }

    normalAttackCount = 0;
    nextSkillType = 0; // Lần sau dùng Fireball
}

// --- Wing Spark Particles ---

void FirePhoenixBoss::SpawnWingSparks(float deltaTime) {
    sparkSpawnTimer += deltaTime;

    float spawnInterval, startAlpha;
    int sparksPerWing;

    if (currentPhase == Phase::PHASE_2 || currentPhase == Phase::TRANSITIONING) {
        spawnInterval = 0.03f;
        startAlpha = 0.7f;
        sparksPerWing = GetRandomValue(6, 12);
    } else {
        spawnInterval = 0.05f;
        startAlpha = 0.5f;
        sparksPerWing = GetRandomValue(3, 7);
    }

    if (sparkSpawnTimer >= spawnInterval) {
        sparkSpawnTimer = 0.0f;

        // Spawn ở 2 bên cánh (offset relative to 350x350 boss center)
        float wingOffsets[2] = { -140.0f, 140.0f }; // trái, phải

        for (int w = 0; w < 2; w++) {
            for (int i = 0; i < sparksPerWing; i++) {
                SparkParticle sp;
                sp.isTexture = (GetRandomValue(1, 100) <= 15); // Chỉ 15% là ảnh tia lửa, 85% là chấm tròn
                
                sp.pos.x = position.x + wingOffsets[w] + (float)GetRandomValue(-25, 25);
                sp.pos.y = position.y - 15.0f + (float)GetRandomValue(-20, 20);
                sp.velocity.x = (float)GetRandomValue(-40, 40);
                sp.velocity.y = (float)GetRandomValue(-100, -30); // Bay nhẹ hơn
                sp.alpha = startAlpha;
                
                if (sp.isTexture) {
                    sp.size = (float)GetRandomValue(1, 2) + 0.5f; // Ảnh tia lửa nhỏ hơn
                } else {
                    sp.size = (float)GetRandomValue(1, 3) + 0.5f; // Chấm tròn
                }
                
                sp.rotation = (float)GetRandomValue(0, 360);
                sp.rotationSpeed = (float)GetRandomValue(-120, 120);

                // Màu ngẫu nhiên lửa
                int c = GetRandomValue(0, 3);
                if (c == 0) sp.color = {255, 255, 120, 255};
                else if (c == 1) sp.color = {255, 180, 50, 255};
                else if (c == 2) sp.color = {255, 200, 100, 255}; 
                else sp.color = {255, 100, 30, 255};

                sparks.push_back(sp);
            }
        }
    }
}

void FirePhoenixBoss::UpdateSparks(float deltaTime) {
    for (auto& sp : sparks) {
        // Gravity effect
        sp.velocity.y += 150.0f * deltaTime; // Rơi xuống nhẹ hơn
        
        sp.pos.x += sp.velocity.x * deltaTime;
        sp.pos.y += sp.velocity.y * deltaTime;
        sp.rotation += sp.rotationSpeed * deltaTime;
        sp.alpha -= 0.6f * deltaTime; // Tốc độ mờ hợp lý
        sp.size -= 1.0f * deltaTime;  
        if (sp.size < 0.2f) sp.size = 0.2f;
    }

    // Xóa particles đã mờ
    sparks.erase(
        std::remove_if(sparks.begin(), sparks.end(),
            [](const SparkParticle& s) { return s.alpha <= 0.0f || s.size <= 0.2f; }),
        sparks.end()
    );
}

void FirePhoenixBoss::DrawSparks() {
    auto gm = GameManager::GetInstance();
    for (const auto& sp : sparks) {
        if (sp.alpha <= 0.0f) continue;
        unsigned char a = (unsigned char)(sp.alpha * 255.0f);
        Color c = { sp.color.r, sp.color.g, sp.color.b, a };
        
        if (sp.isTexture) {
            float scale = sp.size / 3.0f;
            Vector2 origin = { (float)gm->GetTexSpark().width / 2.0f, (float)gm->GetTexSpark().height / 2.0f };
            Rectangle source = { 0.0f, 0.0f, (float)gm->GetTexSpark().width, (float)gm->GetTexSpark().height };
            Rectangle dest = { sp.pos.x, sp.pos.y, gm->GetTexSpark().width * scale, gm->GetTexSpark().height * scale };
            
            DrawTexturePro(gm->GetTexSpark(), source, dest, origin, sp.rotation, c);
        } else {
            DrawCircleGradient((int)sp.pos.x, (int)sp.pos.y, 
                               sp.size, c, {sp.color.r, sp.color.g, sp.color.b, 0});
        }
    }
}

// --- HP Bar ---

void FirePhoenixBoss::DrawBossHPBar() {
    float barWidth = 300.0f;
    float barHeight = 10.0f;
    float barX = position.x - barWidth / 2.0f;
    float barY = position.y - 200.0f; // Trên đầu boss

    float hpRatio = currentHp / maxHp;
    if (hpRatio < 0.0f) hpRatio = 0.0f;
    if (hpRatio > 1.0f) hpRatio = 1.0f;

    // Background
    DrawRectangle((int)barX, (int)barY, (int)barWidth, (int)barHeight, {40, 40, 40, 200});

    // HP fill
    Color fillColor;
    if (currentPhase == Phase::PHASE_1) {
        fillColor = {80, 220, 80, 255}; // Xanh lá
    } else {
        fillColor = {255, 120, 30, 255}; // Cam đỏ
    }
    DrawRectangle((int)barX, (int)barY, (int)(barWidth * hpRatio), (int)barHeight, fillColor);

    // Border
    DrawRectangleLines((int)barX, (int)barY, (int)barWidth, (int)barHeight, WHITE);

    // Phase text
    const char* phaseText = (currentPhase == Phase::PHASE_2) ? "PHASE 2" : "PHASE 1";
    if (currentPhase == Phase::TRANSITIONING) phaseText = "TRANSFORMING...";
    int textW = FontManager::GetInstance()->MeasureGameText(phaseText, 16, "Retro").x;
    FontManager::GetInstance()->DrawGameTextCentered(phaseText, position.x, (int)(barY - 20), 16, WHITE, "Retro");
}

// --- Main Update ---

void FirePhoenixBoss::Update(float deltaTime) {
    Vector2 oldPos = position;
    Boss::Update(deltaTime); // Cập nhật animation và các thông số cơ bản

    if (currentPhase == Phase::TRANSITIONING) {
        position = oldPos; // Ghi đè lại vị trí để đứng bất động
        targetPos = position; // Tránh boss tự chọn mục tiêu mới
    }

    // Particles luôn update (cả khi transitioning)
    SpawnWingSparks(deltaTime);
    UpdateSparks(deltaTime);

    if (currentPhase == Phase::TRANSITIONING) {
        UpdateTransition(deltaTime);
        return; // Không tấn công khi đang chuyển dạng
    }

    // Attack cycle (Phase 1 & Phase 2)
    attackTimer += deltaTime;
    if (attackTimer >= attackCooldown) {
        attackTimer = 0.0f;

        if (normalAttackCount < normalAttacksBeforeSkill) {
            FireNormalAttack();
        } else {
            // Dùng kỹ năng
            if (nextSkillType == 0) {
                FireLargeFireball();
            } else {
                FireRain();
            }
        }
    }
}

// --- Main Draw ---

void FirePhoenixBoss::Draw() {
    if (!isActive) return;

    // Vẽ sparks trước boss (layer phía sau)
    DrawSparks();

    // Vẽ boss sprite (sử dụng Boss::Draw base)
    // Nhưng cần xử lý transition flash, nên override toàn bộ
    auto gm = GameManager::GetInstance();
    Texture2D tex = gm->GetTexEnemyAnim(visualId - 1);

    float destW = 350.0f;
    float destH = 350.0f;
    Vector2 origin = { destW / 2.0f, destH / 2.0f };

    float frameSize = (float)tex.height;
    Rectangle srcRec = { (float)currentAnimFrame * frameSize, 0, frameSize, frameSize };

    // Tint color xử lý
    Color tintColor = WHITE;
    if (currentPhase == Phase::TRANSITIONING) {
        // Nhấp nháy mạnh: xen kẽ trắng lóa và mờ ảo (Opacity thấp)
        if (transitionFlashCount % 2 == 0) {
            tintColor = {255, 255, 255, 255}; // Sáng chói
        } else {
            tintColor = {255, 100, 50, 80}; // Đỏ cam, mờ đục (Opacity 80/255)
        }
    } else if (hitFlashTimer > 0.0f) {
        tintColor = {255, 80, 80, 255}; // Đỏ khi bị đánh
    } else if (currentPhase == Phase::PHASE_2) {
        // Phase 2: tint đỏ nhẹ để phân biệt
        tintColor = {255, 220, 200, 255};
    }

    Rectangle destRec = { position.x, position.y, destW, destH };
    DrawTexturePro(tex, srcRec, destRec, origin, wobbleAngle, tintColor);

    // Vẽ thanh HP boss
    DrawBossHPBar();
}
// ============================================================
// --- BouncingRedBullet Implementation ---
// ============================================================

BouncingRedBullet::BouncingRedBullet(Vector2 startPos, Vector2 velocity)
    : Bullet(startPos, 25.0f, 0.0f, false, 0, 15.0f), bounceCount(0), maxBounces(3) {
    SetVelocity(velocity);
}

void BouncingRedBullet::Update(float deltaTime) {
    if (!isActive) return;

    Vector2 vel = GetVelocity();
    position.x += vel.x * deltaTime;
    position.y += vel.y * deltaTime;

    bool bounced = false;
    if (position.x <= 0 || position.x >= 800) {
        vel.x = -vel.x;
        bounced = true;
    }
    if (position.y <= 0 || position.y >= 600) {
        vel.y = -vel.y;
        bounced = true;
    }

    if (bounced) {
        SetVelocity(vel);
        bounceCount++;
        if (bounceCount > maxBounces) {
            isActive = false;
        }
    }
}

void BouncingRedBullet::Draw() {
    if (!isActive) return;
    // Draw pure red glowing circle
    DrawCircleGradient((int)position.x, (int)position.y, 20.0f, {255, 0, 0, 150}, {150, 0, 0, 0});
    DrawCircleGradient((int)position.x, (int)position.y, 10.0f, {255, 100, 100, 255}, {255, 0, 0, 200});
}

// ============================================================
// --- RedBossBullet Implementation ---
// ============================================================

RedBossBullet::RedBossBullet(Vector2 startPos, Vector2 velocity)
    : Bullet(startPos, 25.0f, 0.0f, false, 0, 12.0f) {
    SetVelocity(velocity);
}

void RedBossBullet::Update(float deltaTime) {
    Bullet::Update(deltaTime);
}

void RedBossBullet::Draw() {
    if (!isActive) return;
    DrawCircleGradient((int)position.x, (int)position.y, 18.0f, {255, 0, 0, 150}, {150, 0, 0, 0});
    DrawCircleGradient((int)position.x, (int)position.y, 8.0f, {255, 100, 100, 255}, {255, 0, 0, 200});
}

// ============================================================
// --- Eggsecutioner Boss Implementation ---
// ============================================================

EggsecutionerBoss::EggsecutionerBoss(int visualId, const EnemyStats& stats, Vector2 pos)
    : Boss(visualId, stats, pos),
      nextSkillType(0),
      attackCooldown(3.5f),
      moveTimer(0.0f),
      isDashing(false),
      isYielding(false),
      dashSpeed(800.0f),
      normalSpeed(180.0f) {
    canShoot = false;
    currentHp = 40000.0f;
    maxHp = 40000.0f;
    
    // Desync at spawn so they don't look like they're dancing identically
    auto gm = GameManager::GetInstance();
    targetPos.x = (float)GetRandomValue(150, gm->GetScreenWidth() - 150);
    targetPos.y = (float)GetRandomValue(100, gm->GetScreenHeight() / 2 - 50);
    
    attackTimer = (float)GetRandomValue(0, 20) / 10.0f; // 0.0 to 2.0s random offset
    
    currentVelocity = {0, 0};
    intendedVel = {0, 0};
}

void EggsecutionerBoss::FireBouncingBullets() {
    auto gm = GameManager::GetInstance();
    float bulletSpeed = 250.0f;

    for (int i = 0; i < 5; i++) {
        float angle = (i * (360.0f / 5.0f)) * (PI / 180.0f);
        Vector2 vel = { cosf(angle) * bulletSpeed, sinf(angle) * bulletSpeed };
        gm->AddBullet(std::make_shared<BouncingRedBullet>(position, vel));
    }
}

void EggsecutionerBoss::FireRedDarts() {
    auto gm = GameManager::GetInstance();
    float bulletSpeed = 450.0f;

    auto player = gm->GetPlayer();
    float baseAngle = 90.0f * (PI / 180.0f);
    if (player && player->IsActive()) {
        Vector2 target = player->GetPosition();
        baseAngle = atan2(target.y - position.y, target.x - position.x);
    }

    for (int i = 0; i < 3; i++) {
        float angleOffset = -15.0f + i * 15.0f;
        float rad = baseAngle + angleOffset * (PI / 180.0f);
        Vector2 vel = { cosf(rad) * bulletSpeed, sinf(rad) * bulletSpeed };
        gm->AddBullet(std::make_shared<RedBossBullet>(position, vel));
    }
}

void EggsecutionerBoss::FireRedNova() {
    auto gm = GameManager::GetInstance();
    float bulletSpeed = 180.0f;

    for (int i = 0; i < 36; i++) {
        float angle = (i * 10.0f) * (PI / 180.0f);
        Vector2 vel = { cosf(angle) * bulletSpeed, sinf(angle) * bulletSpeed };
        gm->AddBullet(std::make_shared<RedBossBullet>(position, vel));
    }
}

void EggsecutionerBoss::DrawBossHPBar() {
    float barWidth = 300.0f;
    float barHeight = 10.0f;
    float barX = position.x - barWidth / 2.0f;
    float barY = position.y - 120.0f;

    float hpRatio = currentHp / maxHp;
    if (hpRatio < 0.0f) hpRatio = 0.0f;
    if (hpRatio > 1.0f) hpRatio = 1.0f;

    DrawRectangle((int)barX, (int)barY, (int)barWidth, (int)barHeight, {40, 40, 40, 200});
    DrawRectangle((int)barX, (int)barY, (int)(barWidth * hpRatio), (int)barHeight, {255, 50, 50, 255}); // Đỏ tươi
    DrawRectangleLines((int)barX, (int)barY, (int)barWidth, (int)barHeight, WHITE);

    int textW = FontManager::GetInstance()->MeasureGameText("THE EGG-SECUTIONER", 16, "Retro").x;
    FontManager::GetInstance()->DrawGameTextCentered("THE EGG-SECUTIONER", position.x, (int)(barY - 20), 16, WHITE, "Retro");
}

void EggsecutionerBoss::Update(float deltaTime) {
    Enemy::Update(deltaTime);
    battleTime += deltaTime;
    wobbleTimer += deltaTime * 3.0f;
    wobbleAngle = sin(wobbleTimer) * 20.0f;

    moveTimer += deltaTime;
    
    if (moveTimer > 5.0f) {
        moveTimer = 0.0f;
        auto gm = GameManager::GetInstance();
        auto player = gm->GetPlayer();
        
        bool iAmTargetingPlayer = false;
        
        // Xác định con Boss kia
        EggsecutionerBoss* otherBoss = nullptr;
        for (const auto& enemy : gm->GetActiveEnemies()) {
            if (enemy.get() != this && enemy->role == EnemyRole::BOSS && enemy->visualId == 11) {
                otherBoss = static_cast<EggsecutionerBoss*>(enemy.get());
                break;
            }
        }
        
        // Xác định ai sẽ là người nhắm vào player trong chu kỳ 5 giây này
        int cycle = (int)(battleTime / 5.0f);
        if (otherBoss) {
            if (cycle % 2 == 0) {
                iAmTargetingPlayer = (this > otherBoss);
            } else {
                iAmTargetingPlayer = (this < otherBoss);
            }
        } else {
            iAmTargetingPlayer = true; // Nếu con kia chết, con này luôn nhắm player
        }

        if (iAmTargetingPlayer && player && player->IsActive()) {
            // Nhắm vào vị trí quanh phi thuyền
            Vector2 pPos = player->GetPosition();
            targetPos.x = pPos.x + GetRandomValue(-150, 150);
            targetPos.y = pPos.y - GetRandomValue(200, 350); 
            
            // Giới hạn trong màn hình
            if (targetPos.x < 150) targetPos.x = 150;
            if (targetPos.x > gm->GetScreenWidth() - 150) targetPos.x = gm->GetScreenWidth() - 150;
            if (targetPos.y < 50) targetPos.y = 50;
            if (targetPos.y > gm->GetScreenHeight() / 2 + 150) targetPos.y = gm->GetScreenHeight() / 2 + 150;
        } else {
            // Con còn lại sẽ nhắm ngẫu nhiên, ưu tiên bay ra xa phi thuyền để tránh va chạm
            float playerX = (player && player->IsActive()) ? player->GetPosition().x : gm->GetScreenWidth() / 2.0f;
            
            if (playerX < gm->GetScreenWidth() / 2.0f) {
                // Player bên trái -> Boss né sang phải
                targetPos.x = (float)GetRandomValue(gm->GetScreenWidth() / 2 + 100, gm->GetScreenWidth() - 150);
            } else {
                // Player bên phải -> Boss né sang trái
                targetPos.x = (float)GetRandomValue(150, gm->GetScreenWidth() / 2 - 100);
            }
            targetPos.y = (float)GetRandomValue(50, gm->GetScreenHeight() / 2 - 50);
        }
        
        // Loại bỏ hoàn toàn Dashing (tốc biến)
        isDashing = false;
    }

    intendedVel = {0, 0};
    
    Vector2 dir = { targetPos.x - position.x, targetPos.y - position.y };
    float dist = sqrt(dir.x * dir.x + dir.y * dir.y);
    if (dist > 0) {
        float currentSpeed = normalSpeed;
        if (dist < 10.0f) {
            currentSpeed = 0.0f; // Tới nơi thì dừng lại
        } else if (dist < 100.0f) {
            currentSpeed = normalSpeed * (dist / 100.0f);
            if (currentSpeed < 30.0f) currentSpeed = 30.0f;
        }
        intendedVel = { (dir.x / dist) * currentSpeed, (dir.y / dist) * currentSpeed };
    }

    isYielding = false;
    auto gm = GameManager::GetInstance();
    for (const auto& enemy : gm->GetActiveEnemies()) {
        if (enemy.get() != this && enemy->role == EnemyRole::BOSS && enemy->visualId == 11) {
            EggsecutionerBoss* other = static_cast<EggsecutionerBoss*>(enemy.get());
            
            // 1. Hard Repulsion to guarantee they never physically stick
            Vector2 diff = { position.x - other->position.x, position.y - other->position.y };
            float dist = sqrt(diff.x * diff.x + diff.y * diff.y);
            float safeRadius = 260.0f; // Bán kính an toàn bao phủ toàn bộ cánh/đầu/chân
            if (dist < safeRadius && dist > 0) {
                float pushForce = (safeRadius - dist) * 3.0f * deltaTime;
                position.x += (diff.x / dist) * pushForce;
                position.y += (diff.y / dist) * pushForce;
            }

            // 2. Continuous Collision Prediction (Swept circle logic)
            bool conflict = false;
            for (float t = 0.2f; t <= 3.0f; t += 0.2f) {
                Vector2 myFuture = { position.x + intendedVel.x * t, position.y + intendedVel.y * t };
                Vector2 otherFuture = { other->position.x + other->currentVelocity.x * t, other->position.y + other->currentVelocity.y * t };
                if (Vector2Distance(myFuture, otherFuture) < 280.0f) {
                    conflict = true;
                    break;
                }
            }
            
            if (conflict) {
                if (this > other) {
                    isYielding = true;
                } else {
        
                    targetPos.x = (float)GetRandomValue(200, gm->GetScreenWidth() - 200);
                    targetPos.y = (float)GetRandomValue(100, 350);
                    if (isDashing) {
                        dashTarget = { (float)GetRandomValue(200, gm->GetScreenWidth() - 200), (float)GetRandomValue(100, 350) };
                    }
                }
            }
        }
    }

    if (isYielding) {
        currentVelocity = {0, 0};
    } else {
        currentVelocity = intendedVel;
        position.x += currentVelocity.x * deltaTime;
        position.y += currentVelocity.y * deltaTime;
    }

    attackTimer += deltaTime;
    if (attackTimer >= attackCooldown) {
        attackTimer = 0.0f;
        
        if (nextSkillType == 0) {
            FireBouncingBullets();
            attackCooldown = 2.0f;
        } else if (nextSkillType == 1) {
            FireRedDarts();
            attackCooldown = 1.5f;
        } else {
            FireRedNova();
            attackCooldown = 4.0f;
        }
        
        nextSkillType = (nextSkillType + 1) % 3;
    }
}

void EggsecutionerBoss::Draw() {
    if (!isActive) return;

    Texture2D tex = GameManager::GetInstance()->GetTexEnemyAnim(visualId - 1);
    
    if (tex.id == 0) return;
    
    // Automatically get frame dimensions from the texture
    float frameSize = (float)tex.height;
    
    // Use the correctly updated currentAnimFrame from Enemy::Update
    Rectangle srcRec = { (float)currentAnimFrame * frameSize, 0.0f, frameSize, frameSize };
    
    // Draw at 200x200 (Pixel perfect with the texture)
    float destW = 200.0f; 
    float destH = 200.0f;
    Vector2 origin = { destW / 2.0f, destH / 2.0f };
    
    Color tintColor = WHITE;
    if (hitFlashTimer > 0.0f) {
        tintColor = {255, 100, 100, 255};
    } else if (currentHp < maxHp * 0.3f) {
        tintColor = {255, 200, 200, 255}; // Reddish tint when low HP
    }
    
    Rectangle destRec = { position.x, position.y, destW, destH };
    DrawTexturePro(tex, srcRec, destRec, origin, wobbleAngle, tintColor);

    DrawBossHPBar();
}

void EggsecutionerBoss::Die() {
    Boss::Die();
}

// --- BlueRoundBullet Implementation ---

BlueRoundBullet::BlueRoundBullet(Vector2 startPos, Vector2 velocity, float damage)
    : Bullet(startPos, damage, 0.0f, false, 0, 16.0f), glowPulse(0.0f) {
    SetVelocity(velocity);
}

void BlueRoundBullet::Update(float deltaTime) {
    Bullet::Update(deltaTime);
    glowPulse += deltaTime * 5.0f;
}

void BlueRoundBullet::Draw() {
    if (!isActive) return;
    
    float pulse = (sinf(glowPulse) + 1.0f) * 0.5f;
    float glowRadius = GetRadius() * (1.2f + 0.3f * pulse);
    
    DrawCircleGradient((int)position.x, (int)position.y, glowRadius, Fade(SKYBLUE, 0.6f), Fade(DARKBLUE, 0.0f));
    DrawCircleGradient((int)position.x, (int)position.y, GetRadius(), WHITE, DARKBLUE);
}

// --- SnowballBullet Implementation ---

SnowballBullet::SnowballBullet(Vector2 startPos, Vector2 velocity, float damage)
    : Bullet(startPos, damage, 0.0f, false, 0, 10.0f), 
      age(0.0f), maxAge(2.0f), startRadius(10.0f), endRadius(45.0f) {
    SetVelocity(velocity);
    for (int i = 0; i < 15; i++) {
        Flake f;
        f.angle = GetRandomValue(0, 360) * (PI / 180.0f);
        f.speed = (float)GetRandomValue(10, 30);
        f.size = (float)GetRandomValue(1, 3);
        f.offset = {0, 0};
        flakes.push_back(f);
    }
}

void SnowballBullet::Update(float deltaTime) {
    Bullet::Update(deltaTime);
    age += deltaTime;
    
    float t = std::min(age / maxAge, 1.0f);
    float currentRadius = startRadius + (endRadius - startRadius) * t;
    SetRadius(currentRadius);
    
    for (auto& f : flakes) {
        f.offset.x += cosf(f.angle) * f.speed * deltaTime;
        f.offset.y += sinf(f.angle) * f.speed * deltaTime;
        if (f.offset.x*f.offset.x + f.offset.y*f.offset.y > currentRadius*currentRadius) {
            f.offset = {0, 0};
            f.angle = GetRandomValue(0, 360) * (PI / 180.0f);
        }
    }
}

void SnowballBullet::Draw() {
    if (!isActive) return;
    float r = GetRadius();
    DrawCircleGradient((int)position.x, (int)position.y, r, WHITE, LIGHTGRAY);
    for (const auto& f : flakes) {
        DrawCircleV({position.x + f.offset.x, position.y + f.offset.y}, f.size, WHITE);
    }
}

// --- EskimoBoss Implementation ---

EskimoBoss::EskimoBoss(int visualId, const EnemyStats& stats, Vector2 pos)
    : Boss(visualId, stats, pos),
      moveTimer(7.0f),
      targetPos(pos),
      isMoving(false),
      attackTimer(2.0f),
      normalAttackCount(0),
      normalAttacksBeforeSkill(3),
      attackCooldown(1.5f),
      flakeSpawnTimer(0.0f) {
    canShoot = false;
    movementBehavior = nullptr; // Override JSON movement to use custom floating logic
}

void EskimoBoss::SpawnSnowFlakes(float deltaTime) {
    flakeSpawnTimer += deltaTime;
    float spawnRate = 0.1f;
    
    if (flakeSpawnTimer >= spawnRate) {
        flakeSpawnTimer = 0.0f;
        SnowParticle s;
        s.pos = { position.x + GetRandomValue(-100, 100), position.y + GetRandomValue(-100, 100) };
        s.velocity = { (float)GetRandomValue(-20, 20), (float)GetRandomValue(50, 100) };
        s.alpha = 0.5f;
        s.size = (float)GetRandomValue(2, 5);
        flakes.push_back(s);
    }
}

void EskimoBoss::UpdateFlakes(float deltaTime) {
    for (auto it = flakes.begin(); it != flakes.end(); ) {
        it->pos.x += it->velocity.x * deltaTime;
        it->pos.y += it->velocity.y * deltaTime;
        it->alpha -= deltaTime * 0.5f;
        if (it->alpha <= 0.0f) {
            it = flakes.erase(it);
        } else {
            ++it;
        }
    }
}

void EskimoBoss::FireBlueBurst() {
    auto gm = GameManager::GetInstance();
    int numBullets = (currentHp < maxHp * 0.5f) ? 7 : 5;
    float speed = 300.0f;
    float spreadAngle = 45.0f * (PI / 180.0f);
    
    for (int i = 0; i < numBullets; i++) {
        float angleOffset = 0.0f;
        if (numBullets > 1) {
            angleOffset = -spreadAngle + (2.0f * spreadAngle * i) / (numBullets - 1);
        }
        float rad = (90.0f * (PI/180.0f)) + angleOffset; // Aim down
        Vector2 vel = { cosf(rad) * speed, sinf(rad) * speed };
        gm->AddBullet(std::make_shared<BlueRoundBullet>(position, vel, GetDamage()));
    }
}

void EskimoBoss::FireSnowballs() {
    auto gm = GameManager::GetInstance();
    float speed = 150.0f; // Chậm để tạo áp lực to dần
    
    int numSnowballs = (currentHp < maxHp * 0.5f) ? 5 : 3;
    float spreadAngle = (numSnowballs == 5) ? 20.0f * (PI / 180.0f) : 30.0f * (PI / 180.0f);
    int half = numSnowballs / 2;
    
    for (int i = -half; i <= half; i++) {
        float rad = (90.0f * (PI/180.0f)) + i * spreadAngle;
        Vector2 vel = { cosf(rad) * speed, sinf(rad) * speed };
        gm->AddBullet(std::make_shared<SnowballBullet>(position, vel, GetDamage() * 2.0f));
    }
}

void EskimoBoss::Update(float deltaTime) {
    Boss::Update(deltaTime);
    
    SpawnSnowFlakes(deltaTime);
    UpdateFlakes(deltaTime);
    
    auto gm = GameManager::GetInstance();
    if (gm->IsBossCutscene()) return;

    // --- Movement Logic ---
    if (!isMoving) {
        moveTimer -= deltaTime;
        if (moveTimer <= 0.0f) {
            moveTimer = (float)GetRandomValue(7, 8); // Chờ 7-8 giây cho lần di chuyển tiếp
            targetPos.x = (float)GetRandomValue(150, gm->GetScreenWidth() - 150);
            targetPos.y = (float)GetRandomValue(100, 300);
            isMoving = true;
        }
    } else {
        float dx = targetPos.x - position.x;
        float dy = targetPos.y - position.y;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist < 5.0f) {
            isMoving = false;
        } else {
            float moveSpeed = 150.0f;
            position.x += (dx / dist) * moveSpeed * deltaTime;
            position.y += (dy / dist) * moveSpeed * deltaTime;
        }
    }
    
    // --- Attack Logic ---
    attackTimer -= deltaTime;
    if (attackTimer <= 0.0f) {
        if (normalAttackCount < normalAttacksBeforeSkill) {
            FireBlueBurst();
            normalAttackCount++;
            attackTimer = attackCooldown;
        } else {
            FireSnowballs();
            normalAttackCount = 0;
            attackTimer = attackCooldown * 1.5f; // Chờ lâu hơn sau khi xài skill
        }
    }
}

void EskimoBoss::TakeDamage(float incomingDamage) {
    Boss::TakeDamage(incomingDamage);
}

void EskimoBoss::Die() {
    Boss::Die();
}

void EskimoBoss::DrawBossHPBar() {
    float barWidth = 300.0f;
    float barHeight = 10.0f;
    float barX = position.x - barWidth / 2.0f;
    float barY = position.y - 120.0f;

    DrawRectangle((int)barX - 2, (int)barY - 2, (int)barWidth + 4, (int)barHeight + 4, BLACK);
    
    float hpPercent = std::max(0.0f, currentHp / maxHp);
    Color hpColor = RED;
    
    DrawRectangle((int)barX, (int)barY, (int)(barWidth * hpPercent), (int)barHeight, hpColor);
    
    const char* phaseText = "BOSS 03";
    int textW = FontManager::GetInstance()->MeasureGameText(phaseText, 10, "Retro").x;
    FontManager::GetInstance()->DrawGameTextCentered(phaseText, position.x, (int)barY - 15, 10, WHITE, "Retro");
}

void EskimoBoss::Draw() {
    if (!isActive) return;

    Texture2D tex = GameManager::GetInstance()->GetTexEnemyAnim(visualId - 1);
    float destW = 350.0f;
    float destH = 350.0f;
    Vector2 origin = { destW / 2.0f, destH / 2.0f };
    
    float frameSize = 100.0f; // 2400x100 spritesheet (24 frames of 100x100)
    Rectangle srcRec = { (float)currentAnimFrame * frameSize, 0, frameSize, frameSize };
    
    Color tintColor = WHITE;
    if (hitFlashTimer > 0.0f) {
        tintColor = { 255, 100, 100, 255 };
    }

    Rectangle destRec = { position.x, position.y, destW, destH };
    DrawTexturePro(tex, srcRec, destRec, origin, wobbleAngle, tintColor);

    // Draw snowflakes
    for (const auto& s : flakes) {
        DrawCircleV(s.pos, s.size, Fade(WHITE, s.alpha));
    }

    DrawBossHPBar();
}

// --- Bomber Boss (Stage 5, Wave 10) ---
BomberBoss::BomberBoss(int visualId, const EnemyStats& stats, Vector2 pos)
    : Boss(visualId, stats, pos), attackTimer(2.0f), attackType(0),
      subAttackTimer(0.0f), subAttackCount(0) {
    canShoot = false;
    drawScale = 0.7f; // Giảm kích thước 30% cho đỡ mờ
}

void BomberBoss::DrawBossHPBar() {
    int screenWidth = GameManager::GetInstance()->GetScreenWidth();
    float barWidth = 400.0f;
    float barHeight = 15.0f;
    float x = (screenWidth - barWidth) / 2.0f;
    float y = 20.0f;
    
    DrawRectangle(x, y, barWidth, barHeight, DARKGRAY);
    float hpPercent = (float)currentHp / (float)maxHp;
    if (hpPercent < 0) hpPercent = 0;
    
    Color hpColor = (hpPercent > 0.5f) ? GREEN : ((hpPercent > 0.2f) ? ORANGE : RED);
    DrawRectangle(x, y, barWidth * hpPercent, barHeight, hpColor);
    DrawRectangleLines(x, y, barWidth, barHeight, WHITE);
    
    FontManager::GetInstance()->DrawGameTextCentered("BOMBER CHICKEN", x + barWidth/2, y - 15, 10, WHITE, "Retro");
}

void BomberBoss::FireVShape() {
    // 4 viên (W-Shape) thay vì 2
    float angles[4] = {60.0f, 80.0f, 100.0f, 120.0f};
    for (int i = 0; i < 4; i++) {
        float rad = angles[i] * (3.14159265f / 180.0f);
        Vector2 vel = { (float)cos(rad) * 220.0f, (float)sin(rad) * 220.0f };
        auto egg = std::make_shared<Bullet>(position, stats.damage, 220.0f, false, 3);
        egg->SetVelocity(vel);
        GameManager::GetInstance()->AddBullet(egg);
    }
}

void BomberBoss::Fire3Way() {
    // 5 viên thay vì 3
    float angles[5] = {60.0f, 75.0f, 90.0f, 105.0f, 120.0f};
    for (int i = 0; i < 5; i++) {
        float rad = angles[i] * (3.14159265f / 180.0f);
        Vector2 vel = { (float)cos(rad) * 200.0f, (float)sin(rad) * 200.0f };
        auto egg = std::make_shared<Bullet>(position, stats.damage, 200.0f, false, 3);
        egg->SetVelocity(vel);
        GameManager::GetInstance()->AddBullet(egg);
    }
}

void BomberBoss::Fire7Way() {
    // 13 viên thay vì 7
    float angles[13] = {30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f, 100.0f, 110.0f, 120.0f, 130.0f, 140.0f, 150.0f};
    for (int i = 0; i < 13; i++) {
        float rad = angles[i] * (3.14159265f / 180.0f);
        Vector2 vel = { (float)cos(rad) * 180.0f, (float)sin(rad) * 180.0f };
        auto egg = std::make_shared<Bullet>(position, stats.damage, 180.0f, false, 3);
        egg->SetVelocity(vel);
        GameManager::GetInstance()->AddBullet(egg);
    }
}

void BomberBoss::Update(float deltaTime) {
    if (!isActive) return;
    
    Boss::Update(deltaTime);
    
    // Sub-attack logic for V-Shape
    if (subAttackCount > 0) {
        subAttackTimer -= deltaTime;
        if (subAttackTimer <= 0.0f) {
            FireVShape();
            subAttackCount--;
            if (subAttackCount > 0) {
                subAttackTimer = 0.3f;
            }
        }
    } else {
        // Main attack cycle
        attackTimer -= deltaTime;
        if (attackTimer <= 0.0f) {
            if (attackType == 0) {
                // V-Shape bắn 5 đợt (thay vì 3)
                subAttackCount = 5;
                subAttackTimer = 0.0f;
                attackTimer = 2.0f;
                attackType = 1;
            } else if (attackType == 1) {
                // 3-Way (thực tế là 5 tia)
                Fire3Way();
                attackTimer = 1.0f; // Giảm thời gian chờ
                attackType = 2;
            } else if (attackType == 2) {
                // 7-Way (thực tế là 13 tia)
                Fire7Way();
                attackTimer = 1.5f; // Giảm thời gian chờ
                attackType = 0;
            }
        }
    }
}

void BomberBoss::Draw() {
    if (!isActive) return;
    Boss::Draw();
    DrawBossHPBar();
}

void BomberBoss::Die() {
    if (!isActive) return;
    Enemy::Die();
}

// --- VoidChickenBoss (Stage 5 Wave 15) ---

VoidChickenBoss::VoidChickenBoss(int visualId, const EnemyStats& stats, Vector2 startPos)
    : Boss(visualId, stats, startPos),
      state(State::IDLE_MOVE), stateTimer(0.0f),
      attackTimer(0.0f), shootAngle(0.0f), shotsFired(0) {
    canShoot = false;
    drawScale = 0.75f; // Giảm kích thước 25% theo yêu cầu
}

// Biến lưu số lần đánh thường
static int normalAttackCount = 0;

void VoidChickenBoss::TransitionTo(State newState) {
    state    = newState;
    stateTimer = 0.0f;
    attackTimer = 0.0f;
    shotsFired  = 0;
    shootAngle  = 0.0f;
}

// --- Bắn thường: 3 viên đạn tròn đỏ theo chùm -20°/0°/+20° ---
void VoidChickenBoss::FireNormalAttack() {
    auto gm = GameManager::GetInstance();
    for (int i = -1; i <= 1; i++) {
        float angleDeg = 180.0f + i * 20.0f;      // 160°, 180°, 200° (hướng xuống)
        auto traj = std::make_shared<SpreadTrajectory>(angleDeg);
        auto bullet = std::make_shared<Bullet>(position, stats.damage, 320.0f, false, 3, 14.0f);
        bullet->SetTrajectory(traj);
        gm->AddBullet(bullet);
    }
}

// --- Kỹ năng 1: Tia laser đỏ hướng về player (Tăng sức mạnh) ---
void VoidChickenBoss::FireLaserSkill() {
    auto gm = GameManager::GetInstance();
    float angleDeg = 180.0f;  
    if (gm->GetPlayer()) {
        Vector2 pp = gm->GetPlayer()->GetPosition();
        float dx = pp.x - position.x;
        float dy = pp.y - position.y;
        angleDeg = std::atan2(dy, dx) * (180.0f / PI) + 90.0f;
    }
    // Bắn 3 tia laser cùng lúc tạo thành chùm laser rộng hơn
    for (int i = -1; i <= 1; i++) {
        float laserAngle = angleDeg + i * 15.0f; 
        auto traj   = std::make_shared<SpreadTrajectory>(laserAngle);
        auto bullet = std::make_shared<Bullet>(position, stats.damage * 2.0f, 750.0f, false, 5, 8.0f);
        bullet->SetTrajectory(traj);
        gm->AddBullet(bullet);
    }
}

// --- Kỹ năng 2: Đạn tam giác homing bắn xung quanh (Tăng sức mạnh) ---
void VoidChickenBoss::FireHomingSkill() {
    auto gm = GameManager::GetInstance();
    const int  numBullets  = 16; // Tăng từ 12 lên 16 viên
    const float turnSpeedDeg = 80.0f;  // HomingTrajectory dùng degrees/s (không phải radian/s)
    for (int i = 0; i < numBullets; i++) {
        float initialAngle = i * (360.0f / numBullets);  
        auto traj   = std::make_shared<HomingTrajectory>(initialAngle, turnSpeedDeg);
        std::shared_ptr<Bullet> b = std::make_shared<Bullet>(position, stats.damage * 1.5f, 250.0f, false, 4, 11.0f);
        
        // Đạn tự hủy sau 12 giây
        b->SetMaxLifetime(12.0f);
        
        // Quỹ đạo homing về phía player
        b->SetTrajectory(traj);
        gm->AddBullet(b);
    }
}

void VoidChickenBoss::Update(float deltaTime) {
    if (!isActive) return;
    Boss::Update(deltaTime);
    if (GameManager::GetInstance()->IsBossCutscene()) return;

    stateTimer  += deltaTime;
    attackTimer += deltaTime;

    switch (state) {
        case State::IDLE_MOVE: {
            // Di chuyển lắc lư nhẹ theo sin, hướng về y=150 nếu chưa tới
            position.x += std::sin(GetTime() * 1.2f) * stats.speed * 0.8f * deltaTime;
            if (position.y < 150.0f) position.y += stats.speed * deltaTime;

            if (stateTimer > 2.0f) {
                // Đánh thường là chủ đạo, dùng 2-3 lần mới xài skill
                if (normalAttackCount < 2 || (normalAttackCount == 2 && GetRandomValue(0, 1) == 0)) {
                    TransitionTo(State::ATTACK_NORMAL);
                } else {
                    int r = GetRandomValue(1, 2);
                    if (r == 1) TransitionTo(State::SKILL_LASER);
                    else        TransitionTo(State::SKILL_HOMING);
                    normalAttackCount = 0; // Reset đếm sau khi dùng skill
                }
            }
            break;
        }
        case State::ATTACK_NORMAL: {
            // Bắn 8 lượt, mỗi lượt 3 viên, cách 0.25s
            position.x += std::sin(GetTime() * 1.2f) * stats.speed * 0.4f * deltaTime;
            if (attackTimer >= 0.25f && shotsFired < 8) {
                attackTimer = 0.0f;
                shotsFired++;
                FireNormalAttack();
            }
            if (stateTimer > 2.5f) {
                normalAttackCount++;
                TransitionTo(State::IDLE_MOVE);
            }
            break;
        }
        case State::SKILL_LASER: {
            // Bắn 5 đợt chùm laser, cách 0.4s
            if (attackTimer >= 0.4f && shotsFired < 5) {
                attackTimer = 0.0f;
                shotsFired++;
                FireLaserSkill();
            }
            if (stateTimer > 2.5f) TransitionTo(State::IDLE_MOVE);
            break;
        }
        case State::SKILL_HOMING: {
            // Chờ 0.5s rồi xả 1 lần 16 viên tam giác homing, xả 2 đợt cách nhau 1s
            if (attackTimer >= 1.0f && shotsFired < 2) {
                attackTimer = 0.0f;
                shotsFired++;
                FireHomingSkill();
            }
            if (stateTimer > 3.0f) TransitionTo(State::IDLE_MOVE);
            break;
        }
    }
}

void VoidChickenBoss::DrawBossHPBar() {
    int sw = GameManager::GetInstance()->GetScreenWidth();
    float barW = 450.0f, barH = 16.0f;
    float x = (sw - barW) / 2.0f, y = 20.0f;
    DrawRectangle((int)x, (int)y, (int)barW, (int)barH, DARKGRAY);
    float pct = (maxHp > 0.0f) ? (currentHp / maxHp) : 0.0f;
    if (pct < 0.0f) pct = 0.0f;
    Color c = (pct > 0.5f) ? GREEN : ((pct > 0.2f) ? ORANGE : RED);
    DrawRectangle((int)x, (int)y, (int)(barW * pct), (int)barH, c);
    DrawRectangleLines((int)x, (int)y, (int)barW, (int)barH, WHITE);
    const char* name = "VOID CHICKEN";
    FontManager::GetInstance()->DrawGameTextCentered(name, x + barW / 2.0f, (int)(y - 16), 11, WHITE, "Retro");
}

void VoidChickenBoss::Draw() {
    if (!isActive) return;
    Boss::Draw();
    DrawBossHPBar();
}

void VoidChickenBoss::Die() {
    if (!isActive) return;
    Enemy::Die();
}
