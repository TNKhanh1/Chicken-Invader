// --- BlueRoundBullet Implementation ---

BlueRoundBullet::BlueRoundBullet(Vector2 startPos, Vector2 velocity, float damage)
    : Bullet(startPos, damage, 0.0f, false, 0, 12.0f), glowPulse(0.0f) {
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
      currentPhase(Phase::PHASE_1),
      phase1MaxHp(stats.hp),
      phase2MaxHp(stats.hp * 1.5f),
      transitionTimer(0.0f),
      isInvulnerable(false),
      transitionFlashCount(0),
      transitionFlashTimer(0.0f),
      attackTimer(2.0f),
      normalAttackCount(0),
      normalAttacksBeforeSkill(3),
      attackCooldown(2.5f),
      flakeSpawnTimer(0.0f) {
    canShoot = false;
}

void EskimoBoss::SpawnSnowFlakes(float deltaTime) {
    flakeSpawnTimer += deltaTime;
    float spawnRate = (currentPhase == Phase::PHASE_2) ? 0.05f : 0.1f;
    
    if (flakeSpawnTimer >= spawnRate) {
        flakeSpawnTimer = 0.0f;
        int numFlakes = (currentPhase == Phase::PHASE_2) ? 3 : 1;
        
        for (int i=0; i<numFlakes; i++) {
            SnowParticle s;
            s.pos = { position.x + GetRandomValue(-100, 100), position.y + GetRandomValue(-100, 100) };
            s.velocity = { (float)GetRandomValue(-20, 20), (float)GetRandomValue(50, 100) };
            s.alpha = (currentPhase == Phase::PHASE_2) ? 0.8f : 0.5f;
            s.size = (float)GetRandomValue(2, 5);
            flakes.push_back(s);
        }
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

void EskimoBoss::EnterPhase2() {
    currentPhase = Phase::PHASE_2;
    isInvulnerable = false;
    currentHp = phase2MaxHp;
    maxHp = phase2MaxHp;
    normalAttacksBeforeSkill = 4;
    attackCooldown = 1.8f;
    attackTimer = 2.0f;
    normalAttackCount = 0;
}

void EskimoBoss::FireBlueBurst() {
    auto gm = GameManager::GetInstance();
    int numBullets = (currentPhase == Phase::PHASE_2) ? 7 : 5;
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
    
    if (currentPhase == Phase::PHASE_1) {
        // Bắn 1 quả thẳng xuống
        Vector2 vel = { 0.0f, speed };
        gm->AddBullet(std::make_shared<SnowballBullet>(position, vel, GetDamage() * 2.0f));
    } else {
        // Bắn 3 quả tỏa ra
        float spreadAngle = 30.0f * (PI / 180.0f);
        for (int i = -1; i <= 1; i++) {
            float rad = (90.0f * (PI/180.0f)) + i * spreadAngle;
            Vector2 vel = { cosf(rad) * speed, sinf(rad) * speed };
            gm->AddBullet(std::make_shared<SnowballBullet>(position, vel, GetDamage() * 2.0f));
        }
    }
}

void EskimoBoss::Update(float deltaTime) {
    Boss::Update(deltaTime);
    
    SpawnSnowFlakes(deltaTime);
    UpdateFlakes(deltaTime);
    
    if (currentPhase == Phase::TRANSITIONING) {
        transitionTimer -= deltaTime;
        transitionFlashTimer -= deltaTime;
        if (transitionFlashTimer <= 0.0f) {
            transitionFlashTimer = 0.15f;
            transitionFlashCount++;
        }
        
        // Burst flakes
        for (int i = 0; i < 5; i++) {
            SnowParticle s;
            s.pos = position;
            s.velocity = { (float)GetRandomValue(-200, 200), (float)GetRandomValue(-200, 200) };
            s.alpha = 1.0f;
            s.size = (float)GetRandomValue(3, 8);
            flakes.push_back(s);
        }
        
        if (transitionTimer <= 0.0f) {
            EnterPhase2();
        }
        return; // Dừng lại ở đây khi đang chuyển phase
    }
    
    auto gm = GameManager::GetInstance();
    if (gm->IsBossCutscene()) return;
    
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
    if (isInvulnerable) return;
    Boss::TakeDamage(incomingDamage);
}

void EskimoBoss::Die() {
    if (currentPhase == Phase::PHASE_1) {
        currentPhase = Phase::TRANSITIONING;
        transitionTimer = 2.0f;
        isInvulnerable = true;
        transitionFlashCount = 0;
        transitionFlashTimer = 0.15f;
    } else if (currentPhase == Phase::PHASE_2) {
        Boss::Die();
    }
}

void EskimoBoss::DrawBossHPBar() {
    float barWidth = 300.0f;
    float barHeight = 10.0f;
    float barX = position.x - barWidth / 2.0f;
    float barY = position.y - 120.0f;

    DrawRectangle((int)barX - 2, (int)barY - 2, (int)barWidth + 4, (int)barHeight + 4, BLACK);
    
    float hpPercent = std::max(0.0f, currentHp / maxHp);
    Color hpColor = (currentPhase == Phase::PHASE_1) ? GREEN : RED;
    
    DrawRectangle((int)barX, (int)barY, (int)(barWidth * hpPercent), (int)barHeight, hpColor);
    
    const char* phaseText = (currentPhase == Phase::PHASE_1) ? "PHASE 1" : "PHASE 2";
    if (currentPhase == Phase::TRANSITIONING) phaseText = "ENRAGING!";
    
    int textW = MeasureText(phaseText, 10);
    DrawText(phaseText, (int)(position.x - textW / 2.0f), (int)barY - 15, 10, WHITE);
}

void EskimoBoss::Draw() {
    if (!isActive) return;

    Texture2D tex = GameManager::GetInstance()->GetTexEnemyAnim(visualId - 1);
    float destW = 350.0f;
    float destH = 350.0f;
    Vector2 origin = { destW / 2.0f, destH / 2.0f };
    
    float frameSize = 100.0f; // We generated a 2400x100 spritesheet (24 frames of 100x100)
    Rectangle srcRec = { (float)currentAnimFrame * frameSize, 0, frameSize, frameSize };
    
    Color tintColor = WHITE;
    if (currentPhase == Phase::TRANSITIONING && (transitionFlashCount % 2 == 0)) {
        tintColor = RED;
    } else if (hitFlashTimer > 0.0f) {
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
