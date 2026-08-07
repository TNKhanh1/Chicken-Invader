#ifndef WAYPOINTMOVEMENT_H
#define WAYPOINTMOVEMENT_H

#include "IMovementBehavior.h"
#include <vector>

class WaypointMovement : public IMovementBehavior {
private:
    std::vector<Vector2> waypoints;
    int currentWaypointIndex;
public:
    WaypointMovement(const std::vector<Vector2>& points);
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override;
};

#endif
