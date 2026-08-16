code = """
// --- Bomber Boss (Stage 5, Wave 10) ---
BomberBoss::BomberBoss(int visualId, const EnemyStats& stats, Vector2 pos)
    : Boss(visualId, stats, pos), attackTimer(2.0f), attackType(0),
      subAttackTimer(0.0f), subAttackCount(0) {
    canShoot = false;
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
    
    DrawText("BOMBER CHICKEN", x + barWidth/2 - MeasureText("BOMBER CHICKEN", 10)/2, y - 15, 10, WHITE);
}

void BomberBoss::FireVShape() {
    // 2 viên V-Shape: góc 75 và 105
    float angles[2] = {75.0f, 105.0f};
    for (int i = 0; i < 2; i++) {
        float rad = angles[i] * (3.14159265f / 180.0f);
        Vector2 vel = { (float)cos(rad) * 220.0f, (float)sin(rad) * 220.0f };
        auto egg = std::make_shared<Bullet>(position, stats.damage, 220.0f, false, 3);
        egg->SetVelocity(vel);
        GameManager::GetInstance()->AddBullet(egg);
    }
}

void BomberBoss::Fire3Way() {
    float angles[3] = {60.0f, 90.0f, 120.0f};
    for (int i = 0; i < 3; i++) {
        float rad = angles[i] * (3.14159265f / 180.0f);
        Vector2 vel = { (float)cos(rad) * 200.0f, (float)sin(rad) * 200.0f };
        auto egg = std::make_shared<Bullet>(position, stats.damage, 200.0f, false, 3);
        egg->SetVelocity(vel);
        GameManager::GetInstance()->AddBullet(egg);
    }
}

void BomberBoss::Fire7Way() {
    float angles[7] = {45.0f, 60.0f, 75.0f, 90.0f, 105.0f, 120.0f, 135.0f};
    for (int i = 0; i < 7; i++) {
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
                // V-Shape 3 đợt
                subAttackCount = 3;
                subAttackTimer = 0.0f;
                attackTimer = 2.0f;
                attackType = 1;
            } else if (attackType == 1) {
                // 3-Way
                Fire3Way();
                attackTimer = 1.5f;
                attackType = 2;
            } else if (attackType == 2) {
                // 7-Way
                Fire7Way();
                attackTimer = 2.5f;
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
"""
with open('src/Bosses.cpp', 'a', encoding='utf-8') as f:
    f.write(code)
