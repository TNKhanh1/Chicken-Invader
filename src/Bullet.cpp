#include "Bullet.h"
#include <cmath>
#include "GameManager.h"

Bullet::Bullet(Vector2 startPos, float dmg, float spd, bool playerBullet, int type, float r) 
    : GameObject(startPos), damage(dmg), speed(spd), isPlayerBullet(playerBullet), 
      hasCustomVelocity(false), velocity({0.0f, 0.0f}), prevPosition(startPos), 
      bulletType(type), radius(r), angle(0.0f), activeTimer(0.0f), maxLifetime(0.0f), shooter(nullptr) {
    
    trajectory = nullptr; // Mặc định không có quỹ đạo
}

void Bullet::SetVelocity(Vector2 vel) {
    velocity = vel;
    hasCustomVelocity = true;
    // Tính góc quay sprite dựa trên hướng velocity (0° = hướng lên trên)
    if (vel.x != 0.0f || vel.y != 0.0f) {
        angle = atan2f(vel.x, -vel.y) * (180.0f / PI);
    } else {
        angle = 0.0f;
    }
}

void Bullet::SetTrajectory(std::shared_ptr<IBulletTrajectory> traj) {
    trajectory = traj;
}

void Bullet::Reset(Vector2 startPos, float dmg, float spd, bool playerBullet, int type, float r) {
    position = startPos;
    prevPosition = startPos;
    damage = dmg;
    speed = spd;
    isPlayerBullet = playerBullet;
    hasCustomVelocity = false;
    velocity = {0.0f, 0.0f};
    bulletType = type;
    radius = r;
    angle = 0.0f;
    isActive = true;
    activeTimer = 0.0f;
    maxLifetime = 0.0f;
    trajectory = nullptr;
    shooter = nullptr;
}

void Bullet::SetMaxLifetime(float time) {
    maxLifetime = time;
}

void Bullet::Init() {
}

void Bullet::Update(float deltaTime) {
    if (!isActive) return;

    prevPosition = position;
    
    if (maxLifetime > 0.0f) {
        activeTimer += deltaTime;
        if (activeTimer >= maxLifetime) {
            isActive = false;
            return;
        }
    }

    if (trajectory) {
        trajectory->UpdatePosition(position, angle, speed, deltaTime);
    } else if (hasCustomVelocity) {
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
    } else {
        if (isPlayerBullet) {
            position.y -= speed * deltaTime;
        } else {
            position.y += speed * deltaTime;
        }
    }

    if (position.y < -100 || position.y > 1100 || position.x < -100 || position.x > 2000) {
        isActive = false;
    }
}

float Bullet::GetDamage() const { return damage; }
bool Bullet::IsPlayerBullet() const { return isPlayerBullet; }

float Bullet::GetRadius() const { return radius; }
void Bullet::SetRadius(float r) { radius = r; }
Vector2 Bullet::GetCenter() const { return position; }
Vector2 Bullet::GetPrevCenter() const { return prevPosition; }
Vector2 Bullet::GetVelocity() const {
    if (hasCustomVelocity) return velocity;
    float rad = (angle - 90.0f) * (3.14159265f / 180.0f);
    return { (float)cos(rad) * speed, (float)sin(rad) * speed };
}

Rectangle Bullet::GetHitbox() const {
    return {position.x - radius, position.y - radius, radius * 2.0f, radius * 2.0f};
}

void Bullet::Draw() {
    if (!isActive) return;
    
    if (isPlayerBullet) {
        GameManager* gm = GameManager::GetInstance();
        Texture2D tex = {0};
        float width = 16.0f, height = 32.0f;
        float rot = angle;
        
        switch(bulletType) {
            case 1: tex = gm->GetTexBulletStrong(); width = 20.0f; height = 40.0f; break; // Hypergun Strong
            case 2: tex = gm->GetTexBulletWeak(); width = 12.0f; height = 24.0f; break; // Hypergun Weak
            case 3: tex = gm->GetTexNeutronGun(0); width = 14.0f; height = 30.0f; break; // Neutron Weak
            case 4: tex = gm->GetTexNeutronGun(1); width = 18.0f; height = 40.0f; break; // Neutron Med
            case 5: tex = gm->GetTexNeutronGun(2); width = 24.0f; height = 52.0f; break; // Neutron Strong
            case 6: tex = gm->GetTexRiddler(); width = 16.0f; height = 28.0f; break; // Riddler
            case 7: tex = gm->GetTexIonBlaster(0); width = 18.0f; height = 28.0f; break; // Ion Single
            case 8: tex = gm->GetTexIonBlaster(1); width = 26.0f; height = 38.0f; break; // Ion Double
            case 9: { // Utensil Poker Fork (quay nĩa)
                tex = gm->GetTexUtensilPoker(0); width = 22.0f; height = 44.0f;
                rot = (float)GetTime() * 720.0f;
                break;
            }
            case 10: { // Utensil Poker Carving (quay dao)
                tex = gm->GetTexUtensilPoker(1); width = 26.0f; height = 50.0f;
                rot = (float)GetTime() * 720.0f + 180.0f;
                break;
            }
            case 11: tex = gm->GetTexLaserCannon(0); width = 16.0f; height = 36.0f; break; // Laser Cannon Weak
            case 12: tex = gm->GetTexLaserCannon(1); width = 18.0f; height = 42.0f; break; // Laser Cannon Med Weak
            case 13: tex = gm->GetTexLaserCannon(2); width = 20.0f; height = 50.0f; break; // Laser Cannon Med Strong
            case 14: tex = gm->GetTexLaserCannon(3); width = 24.0f; height = 58.0f; break; // Laser Cannon Strong
            default:
                tex = gm->GetTexBulletPlayer(); width = 14.0f; height = 30.0f; break;
        }

        if (tex.id != 0) {
            DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height},
                           {position.x, position.y, width, height}, {width / 2.0f, height / 2.0f}, rot, WHITE);
        } else {
            DrawCircle(position.x, position.y, (int)radius, SKYBLUE);
        }
    } else {
        // Enemy Bullet
        if (bulletType == 3) {
            // Procedural Red Split Bullet (chicken05)
            DrawCircleGradient(position.x, position.y, radius, {255, 100, 100, 255}, {150, 0, 0, 255});
            DrawCircle(position.x, position.y, radius * 0.6f, {255, 200, 200, 200});
        } else if (bulletType == 4) {
            // Homing Triangle (Red) — nhỏ, đều, gọn
            float rad = angle * (PI / 180.0f);
            // Tip nhọn theo hướng bay
            Vector2 p1 = { position.x + std::sin(rad) * radius * 1.5f, position.y - std::cos(rad) * radius * 1.5f };
            // Hai đáy đối xứng hai bên hướng vuông góc
            float sideRad = rad + PI / 2.0f;
            Vector2 p2 = { position.x + std::sin(sideRad) * radius * 0.7f - std::sin(rad) * radius * 0.5f,
                           position.y - std::cos(sideRad) * radius * 0.7f + std::cos(rad) * radius * 0.5f };
            Vector2 p3 = { position.x - std::sin(sideRad) * radius * 0.7f - std::sin(rad) * radius * 0.5f,
                           position.y + std::cos(sideRad) * radius * 0.7f + std::cos(rad) * radius * 0.5f };
            // Vẽ fill đỏ cùng màu với đạn tròn
            DrawTriangle(p1, p3, p2, {255, 100, 100, 255}); // CCW
            // Vẽ viền ngoài đỏ sẫm
            DrawTriangleLines(p1, p3, p2, {150, 0, 0, 255});
            // Chấm sáng tâm màu hồng nhạt (giống đạn tròn)
            DrawCircle((int)position.x, (int)position.y, radius * 0.25f, {255, 200, 200, 200});
        } else if (bulletType == 5) {
            // Laser Segment (Red) — 3-layer glow cho đẹp
            float rad = angle * (PI / 180.0f);
            float length = 45.0f;
            Vector2 p1 = { position.x - std::sin(rad) * length / 2.0f, position.y + std::cos(rad) * length / 2.0f };
            Vector2 p2 = { position.x + std::sin(rad) * length / 2.0f, position.y - std::cos(rad) * length / 2.0f };
            // Lớp ngoài: hào quang đỏ mờ
            DrawLineEx(p1, p2, radius * 3.5f, {255, 40, 40, 80});
            // Lớp giữa: thân laser đỏ đậm
            DrawLineEx(p1, p2, radius * 2.0f, {255, 60, 60, 210});
            // Lớp trong: lõi trắng hồng sáng
            DrawLineEx(p1, p2, radius * 0.7f, {255, 210, 210, 255});
            // Điểm sáng ở hai đầu
            DrawCircle((int)p1.x, (int)p1.y, radius * 0.8f, {255, 150, 150, 200});
            DrawCircle((int)p2.x, (int)p2.y, radius * 0.8f, {255, 150, 150, 200});
        } else {
            Texture2D tex = GameManager::GetInstance()->GetTexEnemyBullet();
            if (tex.id != 0) {
                DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, 
                               {position.x, position.y, 20.0f, 25.0f}, {10.0f, 12.5f}, 0.0f, WHITE);
            } else {
                DrawCircle(position.x, position.y, 8, RED);
            }
        }
    }
}

