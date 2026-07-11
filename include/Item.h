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
        // DrawTexture
    }

    ItemType GetType() const { return type; }
};

#endif // ITEM_H
