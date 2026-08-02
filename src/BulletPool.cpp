#include "BulletPool.h"
#include <iostream>

BulletPool::BulletPool(size_t initialSize) : poolSize(initialSize) {
    pool.reserve(poolSize);
    for (size_t i = 0; i < poolSize; ++i) {
        auto bullet = std::make_shared<Bullet>(Vector2{0, 0}, 0.0f, 0.0f, true, 0, 12.0f);
        bullet->SetActive(false);
        pool.push_back(bullet);
    }
}

std::shared_ptr<Bullet> BulletPool::Acquire(Vector2 startPos, float dmg, float spd, bool isPlayerBullet, int type, float radius) {
    for (auto& bullet : pool) {
        if (!bullet->IsActive()) {
            bullet->Reset(startPos, dmg, spd, isPlayerBullet, type, radius);
            return bullet;
        }
    }
    // Overflow Policy: Silent Drop (Bể đầy 1000 viên, trả về nullptr để bỏ qua không sinh đạn mà không bị crash/segfault)
    return nullptr;
}

void BulletPool::Release(std::shared_ptr<Bullet> bullet) {
    if (bullet) {
        bullet->SetActive(false);
    }
}

void BulletPool::Update(float deltaTime) {
    for (auto& bullet : pool) {
        if (bullet->IsActive()) {
            bullet->Update(deltaTime);
        }
    }
}

void BulletPool::Draw() {
    for (auto& bullet : pool) {
        if (bullet->IsActive()) {
            bullet->Draw();
        }
    }
}

std::vector<std::shared_ptr<Bullet>> BulletPool::GetActiveBullets() const {
    std::vector<std::shared_ptr<Bullet>> activeList;
    for (const auto& bullet : pool) {
        if (bullet->IsActive()) {
            activeList.push_back(bullet);
        }
    }
    return activeList;
}
