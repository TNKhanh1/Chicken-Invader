#ifndef POWERUPITEM_H
#define POWERUPITEM_H

#include "Item.h"

class Spaceship;

class PowerUpItem : public Item {
protected:
    float fallSpeed;
    float driftX;
    float time;

public:
    PowerUpItem(Vector2 pos, ItemType type, float drift = 0.0f);

    void Update(float deltaTime) override;
    
    // Default implementation, overridden by subclasses
    void Draw() override {}

    // Template method pattern for item effect
    virtual void OnPickup(Spaceship* player) = 0;
};

#endif // POWERUPITEM_H
