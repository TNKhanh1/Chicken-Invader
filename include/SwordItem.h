#ifndef SWORDITEM_H
#define SWORDITEM_H

#include "PowerUpItem.h"

class SwordItem : public PowerUpItem {
public:
    SwordItem(Vector2 pos);
    void Draw() override;
    void OnPickup(Spaceship* player) override;
};

#endif // SWORDITEM_H
