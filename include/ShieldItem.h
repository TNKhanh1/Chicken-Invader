#ifndef SHIELDITEM_H
#define SHIELDITEM_H

#include "PowerUpItem.h"

class ShieldItem : public PowerUpItem {
public:
    ShieldItem(Vector2 pos);
    void Draw() override;
    void OnPickup(Spaceship* player) override;
};

#endif // SHIELDITEM_H
