#include "ItemDropManager.h"
#include "Spaceship.h"
#include "Enemy.h"
#include "HeartItem.h"
#include "SwordItem.h"
#include "ShieldItem.h"
#include "raylib.h"

ItemDropManager* ItemDropManager::instance = nullptr;

ItemDropManager* ItemDropManager::GetInstance() {
    if (!instance) {
        instance = new ItemDropManager();
    }
    return instance;
}

void ItemDropManager::DestroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

void ItemDropManager::OnNewBatchStarted() {
    itemsDroppedInCurrentBatch = 0;
}

std::shared_ptr<Item> ItemDropManager::TryDrop(const Enemy* enemy, const Spaceship* player, int currentStage) {
    if (!enemy || !player) return nullptr;

    // 1. Không rơi đồ nếu đang có buff
    if (player->HasSwordBuff() || player->HasShieldBuff()) {
        return nullptr;
    }

    // 2. Tính tỷ lệ drop động (Dynamic Drop Rate)
    // Trước khi có item đầu tiên: Stage 1 = 15%, Stage > 1 = 10%
    // Sau khi đã rơi item:         Stage 1 = 3%,  Stage > 1 = 2%
    float dropRate = 0.0f;
    if (itemsDroppedInCurrentBatch == 0) {
        dropRate = (currentStage == 1) ? 0.15f : 0.10f;
    } else {
        dropRate = (currentStage == 1) ? 0.03f : 0.02f;
    }
    
    int roll = GetRandomValue(0, 99);
    
    if (roll >= dropRate * 100) {
        return nullptr;
    }

    // Đánh dấu đã rơi item
    itemsDroppedInCurrentBatch++;

    // 3. Rơi item ngẫu nhiên (40% Heart, 30% Sword, 30% Shield)
    int typeRoll = GetRandomValue(0, 99);
    Vector2 dropPos = enemy->GetPosition();

    if (typeRoll < 40) {
        return std::make_shared<HeartItem>(dropPos);
    } else if (typeRoll < 70) {
        return std::make_shared<SwordItem>(dropPos);
    } else {
        return std::make_shared<ShieldItem>(dropPos);
    }
}
