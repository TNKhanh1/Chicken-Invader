#ifndef BULLET_POOL_H
#define BULLET_POOL_H

#include "Bullet.h"
#include <vector>
#include <memory>

// Object Pool Pattern cho hệ thống đạn tích hợp cơ chế Silent Drop Overflow Policy
class BulletPool {
private:
    std::vector<std::shared_ptr<Bullet>> pool;
    size_t poolSize;

public:
    BulletPool(size_t initialSize = 1000);
    ~BulletPool() = default;

    // Lấy 1 viên đạn từ bể (nếu đầy trả về nullptr - Silent Drop fallback)
    std::shared_ptr<Bullet> Acquire(Vector2 startPos, float dmg, float spd, bool isPlayerBullet = true, int type = 0, float radius = 12.0f);
    
    // Trả lại viên đạn về bể (vô hiệu hóa cờ active)
    void Release(std::shared_ptr<Bullet> bullet);
    
    // Cập nhật tất cả đạn đang bay
    void Update(float deltaTime);
    
    // Vẽ tất cả đạn đang bay
    void Draw();
    
    // Lấy danh sách các viên đạn đang hoạt động để kiểm tra va chạm
    std::vector<std::shared_ptr<Bullet>> GetActiveBullets() const;
    
    size_t GetPoolSize() const { return poolSize; }
};

#endif // BULLET_POOL_H
