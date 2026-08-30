#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include "ISpaceshipController.h"
#include "raylib.h"

enum class AIState { IDLE, EVADE, ATTACK };

class AIController : public ISpaceshipController {
private:
    AIState currentState = AIState::IDLE;
    float manaActivationCooldown = 0.0f;

    // Hằng số cấu hình (constexpr — không allocate runtime memory)
    static constexpr float DANGER_RADIUS        = 130.0f;
    static constexpr float REPULSION_RADIUS     = 110.0f;
    static constexpr float SAFE_OFFSET          = 10.0f;
    static constexpr float MARGIN               = 60.0f;
    static constexpr float Y_SAFE_RATIO         = 0.55f;
    static constexpr float MANA_THRESHOLD       = 1.0f;
    static constexpr float MANA_COOLDOWN        = 5.0f;
    static constexpr float AI_LERP_SPEED        = 8.0f;
    static constexpr float AI_MOVE_SPEED        = 280.0f;
    static constexpr int   MIN_ENEMIES_FOR_MANA = 2;

    Enemy*  findBestTarget(
        const std::vector<std::shared_ptr<Enemy>>& enemies, Vector2 aiPos);
    Vector2 computeEvadeVector(
        const std::vector<std::shared_ptr<Bullet>>& bullets, Vector2 aiPos);
    Vector2 computeRepulsionVector(
        const std::vector<std::shared_ptr<Enemy>>& enemies, Vector2 aiPos);

    bool hasValidTarget = false;

public:
    void Update(
        Spaceship* ship,
        float deltaTime,
        const std::vector<std::shared_ptr<Bullet>>& activeBullets,
        const std::vector<std::shared_ptr<Enemy>>& activeEnemies,
        bool isBossCutscene,
        int screenWidth,
        int screenHeight
    ) override;

    bool HasValidTarget() const { return hasValidTarget; }
};

#endif // AI_CONTROLLER_H
