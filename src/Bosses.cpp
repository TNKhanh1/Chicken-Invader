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
    
    // Drumstick drop logic removed as requested
    
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
      attackCooldown(2.5f),
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

    int numBullets = (currentPhase == Phase::PHASE_2) ? 5 : 3;
    float spreadAngle = (currentPhase == Phase::PHASE_2) ? 30.0f : 15.0f;

    for (int i = 0; i < numBullets; i++) {
        // Tính góc spread: phân bố đều từ -spreadAngle đến +spreadAngle
        float angleOffset = 0.0f;
        if (numBullets > 1) {
            angleOffset = -spreadAngle + (2.0f * spreadAngle * i) / (numBullets - 1);
        }
        float rad = (90.0f + angleOffset) * (PI / 180.0f); // 90° = hướng xuống
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

    int numFireballs = (currentPhase == Phase::PHASE_2) ? 2 : 1;

    if (numFireballs == 1) {
        // Bắn 1 quả lớn hướng về player
        gm->AddBullet(std::make_shared<FireBullet>(position, vel, bulletDmg, FireBullet::Type::EXPLOSIVE));
    } else {
        // Phase 2: 2 quả spread nhẹ
        float baseAngle = atan2(vel.y, vel.x);
        for (int i = 0; i < 2; i++) {
            float angleOffset = (i == 0) ? -15.0f : 15.0f;
            float rad = baseAngle + angleOffset * (PI / 180.0f);
            Vector2 finalVel = { cosf(rad) * bulletSpeed, sinf(rad) * bulletSpeed };
            gm->AddBullet(std::make_shared<FireBullet>(position, finalVel, bulletDmg, FireBullet::Type::EXPLOSIVE));
        }
    }

    normalAttackCount = 0;
    nextSkillType = 1; // Lần sau dùng Fire Rain
}

void FirePhoenixBoss::FireRain() {
    auto gm = GameManager::GetInstance();
    float bulletSpeed = 180.0f;
    float bulletDmg = 20.0f;

    int numBullets = (currentPhase == Phase::PHASE_2) ? 20 : 12;

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
        spawnInterval = 0.05f;
        startAlpha = 0.8f;
        sparksPerWing = GetRandomValue(3, 5);
    } else {
        spawnInterval = 0.1f;
        startAlpha = 0.4f;
        sparksPerWing = GetRandomValue(1, 2);
    }

    if (sparkSpawnTimer >= spawnInterval) {
        sparkSpawnTimer = 0.0f;

        // Spawn ở 2 bên cánh (offset relative to 350×350 boss center)
        float wingOffsets[2] = { -120.0f, 120.0f }; // trái, phải

        for (int w = 0; w < 2; w++) {
            for (int i = 0; i < sparksPerWing; i++) {
                SparkParticle sp;
                sp.pos.x = position.x + wingOffsets[w] + (float)GetRandomValue(-15, 15);
                sp.pos.y = position.y - 30.0f + (float)GetRandomValue(-20, 20);
                sp.velocity.x = (float)GetRandomValue(-30, 30);
                sp.velocity.y = (float)GetRandomValue(-80, -20); // Bay lên
                sp.alpha = startAlpha;
                sp.size = (float)GetRandomValue(2, 5);

                // Màu ngẫu nhiên lửa
                int c = GetRandomValue(0, 2);
                if (c == 0) sp.color = {255, 255, 120, 255};
                else if (c == 1) sp.color = {255, 180, 50, 255};
                else sp.color = {255, 100, 30, 255};

                sparks.push_back(sp);
            }
        }
    }
}

void FirePhoenixBoss::UpdateSparks(float deltaTime) {
    for (auto& sp : sparks) {
        sp.pos.x += sp.velocity.x * deltaTime;
        sp.pos.y += sp.velocity.y * deltaTime;
        sp.alpha -= 1.5f * deltaTime; // Fade out trong ~0.5s
        sp.size -= 2.0f * deltaTime;  // Co dần
        if (sp.size < 0.5f) sp.size = 0.5f;
    }

    // Xóa particles đã mờ
    sparks.erase(
        std::remove_if(sparks.begin(), sparks.end(),
            [](const SparkParticle& s) { return s.alpha <= 0.0f; }),
        sparks.end()
    );
}

void FirePhoenixBoss::DrawSparks() {
    for (const auto& sp : sparks) {
        if (sp.alpha <= 0.0f) continue;
        unsigned char a = (unsigned char)(sp.alpha * 255.0f);
        Color c = { sp.color.r, sp.color.g, sp.color.b, a };
        DrawCircleGradient((int)sp.pos.x, (int)sp.pos.y,
                           sp.size, c, {sp.color.r, sp.color.g, sp.color.b, 0});
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
    int textW = MeasureText(phaseText, 16);
    DrawText(phaseText, (int)(position.x - textW / 2), (int)(barY - 20), 16, WHITE);
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
