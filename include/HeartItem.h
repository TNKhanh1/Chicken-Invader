#ifndef HEARTITEM_H
#define HEARTITEM_H

#include "PowerUpItem.h"

class HeartItem : public PowerUpItem {
public:
    HeartItem(Vector2 pos);
    void Draw() override;
    void OnPickup(Spaceship* player) override;
};

#endif // HEARTITEM_H
