#include "SwordItem.h"
#include "GameManager.h"
#include "Spaceship.h"

SwordItem::SwordItem(Vector2 pos) 
    : PowerUpItem(pos, ItemType::SWORD, -30.0f) {}

void SwordItem::Draw() {
    if (!isActive) return;
    Texture2D tex = GameManager::GetInstance()->GetTexSwordItem();
    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height},
                   {position.x, position.y, 50.0f, 50.0f}, {25.0f, 25.0f}, 0.0f, WHITE);
}

void SwordItem::OnPickup(Spaceship* player) {
    if (player) {
        player->ApplyBuff(BuffType::SWORD, 15.0f);
        GameManager::GetInstance()->PlayPickupSound();
    }
}
