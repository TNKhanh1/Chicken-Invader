#include "PowerUpItem.h"
#include "GameManager.h"
#include <cmath>

PowerUpItem::PowerUpItem(Vector2 pos, ItemType itemType, float drift)
    : Item(pos, itemType), fallSpeed(120.0f), driftX(drift), time(0.0f) {}

void PowerUpItem::Update(float deltaTime) {
    time += deltaTime;
    
    // Rơi thẳng, trôi ngang nhẹ
    position.y += fallSpeed * deltaTime;
    position.x += std::sin(time * 3.0f) * driftX * deltaTime;

    auto gm = GameManager::GetInstance();
    if (position.y > gm->GetScreenHeight() + 50) {
        isActive = false;
    }
}
