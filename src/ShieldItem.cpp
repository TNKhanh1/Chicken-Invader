#include "ShieldItem.h"
#include "GameManager.h"
#include "Spaceship.h"

ShieldItem::ShieldItem(Vector2 pos) 
    : PowerUpItem(pos, ItemType::SHIELD, 30.0f) {}

void ShieldItem::Draw() {
    if (!isActive) return;
    Texture2D tex = GameManager::GetInstance()->GetTexShieldItem();
    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height},
                   {position.x, position.y, 50.0f, 50.0f}, {25.0f, 25.0f}, 0.0f, WHITE);
}

void ShieldItem::OnPickup(Spaceship* player) {
    if (player) {
        player->ApplyBuff(BuffType::SHIELD, 15.0f);
        GameManager::GetInstance()->PlayPickupSound();
    }
}
