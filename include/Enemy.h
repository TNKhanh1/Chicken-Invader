#ifndef ENEMY_H
#define ENEMY_H

#include "Character.h"
#include "MovementStrategy.h"
#include <memory>

class Enemy : public Character {
private:
    std::unique_ptr<IMovementBehavior> movementBehavior;
    int pointValue; // Điểm số khi bị giết

public:
    Enemy(Vector2 pos, float hp, float dmg, float arm, float spd, int points)
        : Character(pos, hp, dmg, arm, spd), pointValue(points) {}

    int GetPointValue() const { return pointValue; }

    void Init() override {
        // Tải texture quái vật
    }

    float shootTimer = 0.0f;

    void Update(float deltaTime) override;

    void Draw() override;

    void Die() override {
        isActive = false;
        DropItem();
        // Thông báo hệ thống cộng điểm, cộng exp cho người chơi
    }

    void SetMovementBehavior(std::unique_ptr<IMovementBehavior> behavior) {
        movementBehavior = std::move(behavior);
    }

    void DropItem() {
        // Logic rơi vật phẩm:
        // + Trái tim hồi máu
        // + Đùi gà (Exp)
        // + Bom
        // + % rơi 1 level trực tiếp cho phi thuyền
    }

    Rectangle GetHitbox() const {
        return {position.x - 25, position.y - 25, 50, 50};
    }
};

#endif // ENEMY_H
