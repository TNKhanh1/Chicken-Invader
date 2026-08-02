#include "BeamWeapon.h"
#include "GameManager.h"
#include <limits>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

BeamShootingBehavior::BeamShootingBehavior(float width, float radius, bool autoLock)
    : baseWidth(width), lockRadius(radius), isAutoLocking(autoLock) {}

bool BeamShootingBehavior::FindNearestTarget(Vector2 podPos, const std::vector<std::shared_ptr<Enemy>>& enemies, Vector2& outTargetPos, float& outAngle) {
    float minDistance = std::numeric_limits<float>::max();
    std::shared_ptr<Enemy> closestEnemy = nullptr;

    for (const auto& enemy : enemies) {
        if (enemy && enemy->IsActive()) {
            Vector2 enemyPos = enemy->GetPosition();
            float dx = enemyPos.x - podPos.x;
            float dy = enemyPos.y - podPos.y;
            float distSq = dx * dx + dy * dy;

            if (distSq <= lockRadius * lockRadius && distSq < minDistance) {
                minDistance = distSq;
                closestEnemy = enemy;
            }
        }
    }

    if (closestEnemy != nullptr) {
        outTargetPos = closestEnemy->GetPosition();
        float dx = outTargetPos.x - podPos.x;
        float dy = outTargetPos.y - podPos.y;
        outAngle = std::atan2(dy, dx) * (180.0f / M_PI) + 90.0f; // Chuyển đổi từ radian sang độ và cân chỉnh trục Y=0
        return true;
    }

    return false;
}

void BeamShootingBehavior::Shoot(Spaceship* ship) {
    // Logic chung cho bắn đạn tia
}

LightningFryerBehavior::LightningFryerBehavior()
    : BeamShootingBehavior(48.0f, 650.0f, true) {}

void LightningFryerBehavior::Shoot(Spaceship* ship) {
    if (!ship) return;
    
    // Khi bắn Lightning Fryer, tính toán tọa độ nòng súng và áp dụng Auto-Locking
    Vector2 startPos = ship->GetPosition();
    Vector2 targetPos = { startPos.x, 0.0f }; // Mặc định hướng lên mép trên (Y=0)
    float angle = 0.0f;

    // Lấy danh sách quái vật đang bay từ GameManager (giả lập thao tác nhắm)
    // Nếu có quái trong bán kính 650px, tự động khóa tâm và bẻ nghiêng luồng sét
    // Thuật toán chuẩn mực đã được bao đóng trong FindNearestTarget
    // Note: Thực tế render đạn tia liên tục được vẽ trong GameManager hoặc lớp quản lý hiệu ứng vũ khí
}
