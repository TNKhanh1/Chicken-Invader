#include "AIController.h"
#include "Spaceship.h"
#include "Enemy.h"
#include "Bullet.h"
#include "raymath.h"
#include <algorithm>

Enemy* AIController::findBestTarget(const std::vector<std::shared_ptr<Enemy>>& enemies, Vector2 aiPos) {
    Enemy* bestTarget = nullptr;
    float bestDistSq = 9999999.0f;

    for (const auto& enemyPtr : enemies) {
        if (!enemyPtr || !enemyPtr->IsActive()) continue;

        Vector2 enemyPos = enemyPtr->GetPosition();
        
        // Skip enemies that are not strictly above the AI (AI should only shoot upwards)
        if (enemyPos.y >= aiPos.y - SAFE_OFFSET) continue;

        float distSq = Vector2DistanceSqr(aiPos, enemyPos);
        if (distSq < bestDistSq) {
            bestDistSq = distSq;
            bestTarget = enemyPtr.get();
        }
    }

    return bestTarget;
}

Vector2 AIController::computeEvadeVector(const std::vector<std::shared_ptr<Bullet>>& bullets, Vector2 aiPos) {
    Vector2 evadeVec = {0.0f, 0.0f};

    for (const auto& bulletPtr : bullets) {
        if (!bulletPtr || !bulletPtr->IsActive()) continue;
        if (bulletPtr->IsPlayerBullet()) continue; // Ignore player bullets

        Vector2 bulletPos = bulletPtr->GetCenter();
        float dist = Vector2Distance(aiPos, bulletPos);

        if (dist < DANGER_RADIUS && dist > 0.1f) {
            Vector2 dir = Vector2Normalize(Vector2Subtract(aiPos, bulletPos));
            float weight = (DANGER_RADIUS - dist) / DANGER_RADIUS;
            evadeVec = Vector2Add(evadeVec, Vector2Scale(dir, weight));
        }
    }

    return evadeVec;
}

Vector2 AIController::computeRepulsionVector(const std::vector<std::shared_ptr<Enemy>>& enemies, Vector2 aiPos) {
    Vector2 repVec = {0.0f, 0.0f};

    for (const auto& enemyPtr : enemies) {
        if (!enemyPtr || !enemyPtr->IsActive()) continue;

        Vector2 enemyPos = enemyPtr->GetPosition();
        float dist = Vector2Distance(aiPos, enemyPos);

        if (dist < REPULSION_RADIUS && dist > 0.1f) {
            Vector2 dir = Vector2Normalize(Vector2Subtract(aiPos, enemyPos));
            float weight = (REPULSION_RADIUS - dist) / REPULSION_RADIUS;
            repVec = Vector2Add(repVec, Vector2Scale(dir, weight));
        }
    }

    return repVec;
}

void AIController::Update(
    Spaceship* ship,
    float deltaTime,
    const std::vector<std::shared_ptr<Bullet>>& activeBullets,
    const std::vector<std::shared_ptr<Enemy>>& activeEnemies,
    bool isBossCutscene,
    int screenWidth,
    int screenHeight
) {
    if (!ship || !ship->IsActive()) return;

    if (isBossCutscene) return; // Do nothing during cutscenes

    // Update Mana Cooldown
    if (manaActivationCooldown > 0.0f) {
        manaActivationCooldown -= deltaTime;
    }

    Vector2 aiPos = ship->GetPosition();

    // 1. Calculate Vectors
    Vector2 evadeVec = computeEvadeVector(activeBullets, aiPos);
    Vector2 repVec = computeRepulsionVector(activeEnemies, aiPos);
    Vector2 totalDanger = Vector2Add(evadeVec, repVec);
    float threatLevel = Vector2Length(totalDanger);

    Enemy* bestTarget = findBestTarget(activeEnemies, aiPos);
    hasValidTarget = (bestTarget != nullptr);

    Vector2 desiredPos = aiPos;

    // 2. State Machine Logic
    if (threatLevel > 0.3f) {
        currentState = AIState::EVADE;
        Vector2 escapeDir = Vector2Normalize(totalDanger);
        desiredPos = Vector2Add(aiPos, Vector2Scale(escapeDir, 200.0f));
    } else if (bestTarget != nullptr) {
        currentState = AIState::ATTACK;
        Vector2 targetPos = bestTarget->GetPosition();
        
        // Slightly move towards target's X position to align shots
        // Linear interpolation for smooth tracking on X axis
        desiredPos.x = aiPos.x + (targetPos.x - aiPos.x) * 0.3f * deltaTime * 10.0f;
        desiredPos.y = aiPos.y; // Keep current Y in attack mode to maintain safe distance
    } else {
        currentState = AIState::IDLE;
        desiredPos = aiPos;
    }

    // 3. Constrain to Safety Zone
    float minX = MARGIN;
    float maxX = (float)screenWidth - MARGIN;
    float minY = (float)screenHeight * Y_SAFE_RATIO;
    float maxY = (float)screenHeight - MARGIN;

    if (desiredPos.x < minX) desiredPos.x = minX;
    if (desiredPos.x > maxX) desiredPos.x = maxX;
    if (desiredPos.y < minY) desiredPos.y = minY;
    if (desiredPos.y > maxY) desiredPos.y = maxY;

    // 4. Smooth Movement
    Vector2 newPos;
    newPos.x = aiPos.x + (desiredPos.x - aiPos.x) * AI_LERP_SPEED * deltaTime;
    newPos.y = aiPos.y + (desiredPos.y - aiPos.y) * AI_LERP_SPEED * deltaTime;
    ship->SetPosition(newPos);

    // 5. Firing Logic
    if (hasValidTarget && ship->CanFire()) {
        ship->Fire();
    }

    // 6. Mana Activation Logic
    if (manaActivationCooldown <= 0.0f) {
        float maxMana = ship->GetMaxMana();
        if (maxMana > 0.0f && ship->GetCurrentMana() >= maxMana * MANA_THRESHOLD) {
            // Count active enemies
            int activeCount = 0;
            for (const auto& enemy : activeEnemies) {
                if (enemy && enemy->IsActive()) activeCount++;
            }

            if (activeCount >= MIN_ENEMIES_FOR_MANA) {
                ship->ActivateMana();
                manaActivationCooldown = MANA_COOLDOWN;
            }
        }
    }
}
