#ifndef ITEM_H
#define ITEM_H

#include "GameObject.h"

enum class ItemType {
    HEART,          // Trái tim hồi máu (hiện có - chưa dùng)
    DRUMSTICK,      // Đùi gà tăng exp
    BOMB,           // Bom gây sát thương toàn bản đồ
    LEVEL_UP,       // Tăng trực tiếp 1 level (tỷ lệ rất thấp)
    EGG,            // Trứng gà (gây sát thương cho phi thuyền)
    HEART_POWERUP,  // [NEW] - Hồi 25% HP
    SWORD,          // [NEW] - +25% DMG trong 15s
    SHIELD          // [NEW] - Giảm 30% DMG nhận vào trong 15s
};

class Item : public GameObject {
private:
    ItemType type;

public:
    Item(Vector2 pos, ItemType itemType);

    void Init() override;

    void Update(float deltaTime) override;

    void Draw() override;

    ItemType GetType() const;
};



#endif // ITEM_H
