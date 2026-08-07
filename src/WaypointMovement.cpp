#include "WaypointMovement.h"
#include <cmath>

WaypointMovement::WaypointMovement(const std::vector<Vector2>& points) 
    : waypoints(points), currentWaypointIndex(0) {}

void WaypointMovement::Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) {
    if (currentWaypointIndex >= waypoints.size()) {
        return; // Hover in place if all waypoints reached
    }

    Vector2 target = waypoints[currentWaypointIndex];
    float dx = target.x - position.x;
    float dy = target.y - position.y;
    float distanceSq = dx * dx + dy * dy;

    float moveDist = moveSpeed * deltaTime;
    if (distanceSq <= moveDist * moveDist) {
        // Reached waypoint
        position = target;
        currentWaypointIndex++;
    } else {
        float distance = std::sqrt(distanceSq);
        position.x += (dx / distance) * moveDist;
        position.y += (dy / distance) * moveDist;
    }
}
