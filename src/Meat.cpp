#include "Meat.h"
#include "GameManager.h"
#include <cmath>

Meat::Meat(Vector2 pos, Vector2 initialVelocity) 
    : Item(pos, ItemType::DRUMSTICK), velocity(initialVelocity), time(0.0f) {}

void Meat::Update(float deltaTime) {
    time += deltaTime;
    velocity.y += 400.0f * deltaTime; // Gravity
    position.y += velocity.y * deltaTime;
    position.x += velocity.x * deltaTime + std::sin(time * 5.0f) * 60.0f * deltaTime;
    
    auto gm = GameManager::GetInstance();
    if (position.y > gm->GetScreenHeight() + 50) {
        isActive = false;
    }
}

void Meat::Draw() {
    if (!isActive) return;
    Texture2D texMeat = GameManager::GetInstance()->GetTexMeat();
    DrawTexturePro(texMeat, {0, 0, (float)texMeat.width, (float)texMeat.height},
                   {position.x, position.y, 40.0f, 40.0f}, {20.0f, 20.0f}, 0.0f, WHITE);
}
