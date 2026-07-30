#ifndef MEAT_H
#define MEAT_H

#include "Item.h"

class Meat : public Item {
private:
    Vector2 velocity;
    float time;
public:
    Meat(Vector2 pos, Vector2 initialVelocity);

    void Update(float deltaTime) override;

    void Draw() override;
};

#endif // MEAT_H
