#include "Enemy.h"
#include "raylib.h"
#include "GameManager.h"
#include "Bullet.h"
#include <algorithm>
#include <cmath>

Enemy::Enemy(int vId, EnemyRole r, const EnemyStats& s, Vector2 pos)
    : Character(pos, s.hp, s.damage, s.armor, s.speed), pointValue(s.score) {
    visualId = vId;
    role = r;
    stats = s;
    // Khởi tạo gà ngẫu nhiên (Asteroid không xài nhưng kệ nó, không ảnh hưởng)
    // Gán frame random cho ASTEROID để các cục đá không xoay đồng bộ
    currentFrame = GetRandomValue(0, 29);
 }

int Enemy::GetPointValue() const { return pointValue; }

void Enemy::Init() {
}

void Enemy::ResetEggTimer() {
    eggDropTimer = (GetRandomValue(20, 60)) / 10.0f;
}

void Enemy::Die() {
    isActive = false;
    DropItem();
}

void Enemy::SetMovementBehavior(std::unique_ptr<IMovementBehavior> behavior) {
    movementBehavior = std::move(behavior);
}

void Enemy::DropItem() {
}

void Enemy::TakeDamage(float incomingDamage) {
    hitFlashTimer = 0.15f; 
    Character::TakeDamage(incomingDamage);
}

float Enemy::baseSizeForType() const {
    // Khôi phục lại kích thước hiển thị 100.0f mặc định cho mọi loại gà
    // Để không làm hỏng cân bằng game (Wave 3 Tank Chicken quá to)
    return 100.0f * stats.scale; 
}

Rectangle Enemy::GetHitbox() const {
    // Khôi phục lại hitbox gốc 50x50 ở tâm
    float s = stats.scale;
    return {position.x - 25 * s, position.y - 25 * s, 50 * s, 50 * s};
}

void Enemy::Update(float deltaTime) {
    prevPosition = position; // Capture position before movement

    if (hitFlashTimer > 0.0f) {
        hitFlashTimer -= deltaTime;
    }

    // Update Sprite Sheet Animation (30 fps for 24 frames)
    animTimer += deltaTime;
    if (animTimer >= 1.0f / 30.0f) {
        animTimer -= 1.0f / 30.0f;
        currentAnimFrame = (currentAnimFrame + 1) % 24;
    }

    // Áp dụng Strategy di chuyển
    if (movementBehavior) {
        auto gm = GameManager::GetInstance();
        movementBehavior->Move(position, moveSpeed, deltaTime, gm->GetScreenWidth(), gm->GetScreenHeight());
    }

    // Cập nhật animation và trail của ASTEROID
    if (role == EnemyRole::ASTEROID) {
        // --- Cập nhật frame (có 30 frames, chỉ lấy từ hàng 1 - thân thiên thạch) ---
        frameTimer += deltaTime;
        if (frameTimer >= 0.04f) { // ~25 FPS animation
            frameTimer = 0.0f;
            currentFrame = (currentFrame + 1) % 30; // 30 frames trong hàng 1
        }

        // --- Spawn trail point tại vị trí hiện tại (mỗi 40ms) ---
        trailSpawnTimer += deltaTime;
        if (trailSpawnTimer >= 0.04f) {
            trailSpawnTimer = 0.0f;
            TrailPoint tp;
            tp.pos   = position;
            tp.alpha = 1.0f;
            tp.frame = currentFrame;
            trailPoints.push_back(std::move(tp));
        }

        // --- Fade các trail point, xóa những cái đã biến mất ---
        // fade speed = 2.0f (biến mất sau 0.5 giây)
        const float fadeSpeed = 2.0f;
        for (auto& tp : trailPoints) {
            tp.alpha -= fadeSpeed * deltaTime;
        }
        trailPoints.erase(
            std::remove_if(trailPoints.begin(), trailPoints.end(),
                [](const Enemy::TrailPoint& t) { return t.alpha <= 0.0f; }),
            trailPoints.end()
        );
    }
    
    // Logic thả trứng (bắn đạn)
    if (canShoot) {
        if (pendingDoubleShots > 0) {
            doubleShotTimer -= deltaTime;
            if (doubleShotTimer <= 0.0f) {
                auto egg = std::make_shared<Bullet>(position, damage, 175.0f, false);
                GameManager::GetInstance()->AddBullet(egg);
                pendingDoubleShots--;
                doubleShotTimer = 0.2f; // Delay for consecutive egg
            }
        }
        
        eggDropTimer -= deltaTime;
        if (eggDropTimer <= 0.0f) {
            ResetEggTimer();
            if (visualId == 4) {
                // chicken04: Bắn 3 viên xòe hình quạt (Fan Spread)
                float angleSpread = 20.0f; // Góc lệch giữa các viên đạn (độ)
                for (int i = -1; i <= 1; i++) {
                    // Góc bắn thẳng xuống là 90 độ
                    float rad = (90.0f + i * angleSpread) * (3.14159265f / 180.0f);
                    Vector2 vel = { (float)cos(rad) * 175.0f, (float)sin(rad) * 175.0f };
                    auto egg = std::make_shared<Bullet>(position, damage, 175.0f, false);
                    egg->SetVelocity(vel);
                    GameManager::GetInstance()->AddBullet(egg);
                }
            } else {
                auto egg = std::make_shared<Bullet>(position, damage, 175.0f, false);
                GameManager::GetInstance()->AddBullet(egg);
                
                // For chicken03, 30% chance to drop a second egg shortly after
                if (visualId == 3 && GetRandomValue(1, 100) <= 30) {
                    pendingDoubleShots = 1;
                    doubleShotTimer = 0.2f;
                }
            }
        }
    }
    
    // Hủy enemy nếu đi quá xa khỏi màn hình
    if (position.y > GameManager::GetInstance()->GetScreenHeight() + 300.0f ||
        position.x > GameManager::GetInstance()->GetScreenWidth() + 300.0f ||
        position.x < -1000.0f) {
        isActive = false;
    }
}

void Enemy::Draw() {
    auto gm = GameManager::GetInstance();
    
    if (role == EnemyRole::ASTEROID) {
        Texture2D tex = (asteroidVariant == 1) ? gm->GetTexAsteroid1() : gm->GetTexAsteroid2();
        const int   COLS    = (asteroidVariant == 2) ? 15 : 30;
        const float FRAME_W = (float)tex.width  / COLS;  
        const float FULL_H  = (float)tex.height;           
        const int   bodyCol = currentFrame % COLS;

        const float DEST_W      = 100.0f;
        const float DEST_ROW_H  = DEST_W * (FULL_H * 0.5f / FRAME_W); 
        const float DEST_FULL_H = DEST_ROW_H * 2.0f;                   
        Vector2 fullOrigin = { DEST_W * 0.5f, DEST_FULL_H };

        for (const auto& tp : trailPoints) {
            float radius = 10.0f * tp.alpha;
            unsigned char a = (unsigned char)(tp.alpha * 160.0f);
            if (asteroidVariant == 2) {
                DrawCircleGradient((int)tp.pos.x, (int)tp.pos.y,
                                   radius, {255, 160, 30, a}, {255, 80, 0, 0});
            } else {
                DrawCircleGradient((int)tp.pos.x, (int)tp.pos.y,
                                   radius, {200, 200, 210, a}, {120, 120, 130, 0});
            }
        }

        Rectangle fullSrc = { bodyCol * FRAME_W, 0.0f, FRAME_W, FULL_H };
        Rectangle fullDst = { position.x, position.y, DEST_W, DEST_FULL_H };
        DrawTexturePro(tex, fullSrc, fullDst, fullOrigin, 0.0f, WHITE);

    } else {
        Texture2D tex = gm->GetTexEnemyAnim(visualId - 1);
        
        float base = baseSizeForType();
        float aspectRatio = 1.0f; // Mỗi khung hình là hình vuông
        float frameSize = (float)tex.height;

        // --- Tilt nghiêng khi di chuyển ngang ---
        float dx = position.x - prevPosition.x;
        float tiltAngle = std::max(-12.0f, std::min(12.0f, dx * 0.8f));

        // --- Hit flash ---
        Color tintColor = WHITE;
        float shakeX = 0.0f, shakeY = 0.0f;
        if (hitFlashTimer > 0.0f) {
            tintColor = { 255, 80, 80, 255 };
            shakeX = (float)GetRandomValue(-3, 3);
            shakeY = (float)GetRandomValue(-3, 3);
        }

        // --- Final draw ---
        float destW = base;
        float destH = base * aspectRatio;
        Vector2 origin = { destW / 2.0f, destH / 2.0f }; // Vẽ từ tâm
        
        // Sprite sheet nằm ngang, 24 frames, mỗi frame là frameSize x frameSize
        Rectangle srcRec = { (float)currentAnimFrame * frameSize, 0, frameSize, frameSize };
        Rectangle destRec = { position.x + shakeX, position.y + shakeY, destW, destH };
        
        DrawTexturePro(tex, srcRec, destRec, origin, tiltAngle, tintColor);
    }
                   
    // Hiện thanh máu nếu chuột di vào hitbox
    if (CheckCollisionPointRec(GetMousePosition(), GetHitbox())) {
        float hpRatio = GetHp() / GetMaxHp();
        if (hpRatio < 0.0f) hpRatio = 0.0f;
        DrawRectangle(position.x - 25, position.y - 50, 50, 6, RED);
        DrawRectangle(position.x - 25, position.y - 50, 50 * hpRatio, 6, GREEN);
        DrawRectangleLines(position.x - 25, position.y - 50, 50, 6, BLACK);
    }
}
