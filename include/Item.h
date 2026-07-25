#ifndef ITEM_H
#define ITEM_H

#include "GameObject.h"

// Enum cho các loại vật phẩm
enum class ItemType {
    HEART,      // Trái tim hồi máu
    DRUMSTICK,  // Đùi gà tăng exp
    BOMB,       // Bom gây sát thương toàn bản đồ
    LEVEL_UP,   // Tăng trực tiếp 1 level (tỷ lệ rất thấp)
    EGG         // Trứng gà (gây sát thương cho phi thuyền)
};

class Item : public GameObject {
private:
    ItemType type;

public:
    Item(Vector2 pos, ItemType itemType) : GameObject(pos), type(itemType) {}

    void Init() override {
        // Load asset tương ứng với ItemType
    }

    void Update(float deltaTime) override {
        // Logic rơi tự do từ trên xuống
        position.y += 100.0f * deltaTime; 
        
        // Vượt quá màn hình thì hủy
        if (position.y > 720.0f) { // Giả sử chiều cao màn hình là 720
            isActive = false;
        }
    }

    void Draw() override {
        // Lớp cơ sở không vẽ gì cả
    }

    ItemType GetType() const { return type; }
};

#include "GameManager.h"
#include <cmath>

class Meat : public Item {
private:
    Vector2 velocity;
    float time;
public:
    Meat(Vector2 pos, Vector2 initialVelocity) 
        : Item(pos, ItemType::DRUMSTICK), velocity(initialVelocity), time(0.0f) {}

    void Update(float deltaTime) override {
        time += deltaTime;
        velocity.y += 400.0f * deltaTime; // Gravity
        position.y += velocity.y * deltaTime;
        position.x += velocity.x * deltaTime + sin(time * 5.0f) * 60.0f * deltaTime;
        
        auto gm = GameManager::GetInstance();
        if (position.y > gm->GetScreenHeight() + 50) {
            isActive = false;
        }
    }

    void Draw() override {
        if (!isActive) return;
        Texture2D texMeat = GameManager::GetInstance()->GetTexMeat();
        DrawTexturePro(texMeat, {0, 0, (float)texMeat.width, (float)texMeat.height},
                       {position.x, position.y, 40.0f, 40.0f}, {20.0f, 20.0f}, 0.0f, WHITE);
    }
};

#endif // ITEM_H
