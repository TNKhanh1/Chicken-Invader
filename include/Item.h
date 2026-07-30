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
    Item(Vector2 pos, ItemType itemType);

    void Init() override;

    void Update(float deltaTime) override;

    void Draw() override;

    ItemType GetType() const;
};



#endif // ITEM_H
