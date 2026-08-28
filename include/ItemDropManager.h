#ifndef ITEMDROPMANAGER_H
#define ITEMDROPMANAGER_H

#include <memory>
#include "Item.h"

class Spaceship;
class Enemy;

class ItemDropManager {
private:
    static ItemDropManager* instance;
    ItemDropManager() = default;

public:
    static ItemDropManager* GetInstance();
    static void DestroyInstance();

    void OnNewBatchStarted();

    // Quyết định có drop không và drop loại nào
    std::shared_ptr<Item> TryDrop(const Enemy* enemy, const Spaceship* player, int currentStage);
    
private:
    int itemsDroppedInCurrentBatch = 0;
};

#endif // ITEMDROPMANAGER_H
