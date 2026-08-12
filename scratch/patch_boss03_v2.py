import sys

def patch_file():
    with open('src/Bosses.cpp', 'r', encoding='utf-8') as f:
        content = f.read()
    
    marker = "// --- EskimoBoss Implementation ---"
    idx = content.find(marker)
    if idx == -1:
        print("Marker not found.")
        return
    
    prefix = content[:idx]
    
    new_impl = """// --- EskimoBoss Implementation ---

EskimoBoss::EskimoBoss(int visualId, const EnemyStats& stats, Vector2 pos)
    : Boss(visualId, stats, pos),
      moveTimer(7.0f),
      targetPos(pos),
      isMoving(false),
      attackTimer(2.0f),
      normalAttackCount(0),
      normalAttacksBeforeSkill(3),
      attackCooldown(2.5f),
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
    int numBullets = 5;
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
    
    // Bắn 3 quả tỏa ra
    float spreadAngle = 30.0f * (PI / 180.0f);
    for (int i = -1; i <= 1; i++) {
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
    int textW = MeasureText(phaseText, 10);
    DrawText(phaseText, (int)(position.x - textW / 2.0f), (int)barY - 15, 10, WHITE);
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
"""
    with open('src/Bosses.cpp', 'w', encoding='utf-8') as f:
        f.write(prefix + new_impl)
        
if __name__ == '__main__':
    patch_file()
