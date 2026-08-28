#include "HeartItem.h"
#include "GameManager.h"
#include "Spaceship.h"

HeartItem::HeartItem(Vector2 pos) 
    : PowerUpItem(pos, ItemType::HEART_POWERUP, 20.0f) {}

void HeartItem::Draw() {
    if (!isActive) return;
    Texture2D tex = GameManager::GetInstance()->GetTexHeartItem();
    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height},
                   {position.x, position.y, 50.0f, 50.0f}, {25.0f, 25.0f}, 0.0f, WHITE);
}

void HeartItem::OnPickup(Spaceship* player) {
    if (player) {
        player->Heal(player->GetMaxHp() * 0.25f);
        GameManager::GetInstance()->PlayPickupSound();
    }
}
