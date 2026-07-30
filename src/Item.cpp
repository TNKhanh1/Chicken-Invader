#include "Item.h"

Item::Item(Vector2 pos, ItemType itemType) : GameObject(pos), type(itemType) {}

void Item::Init() {}

void Item::Update(float deltaTime) {
    position.y += 100.0f * deltaTime; 
    
    if (position.y > 720.0f) { 
        isActive = false;
    }
}

void Item::Draw() {}

ItemType Item::GetType() const { return type; }
