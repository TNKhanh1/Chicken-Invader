#ifndef FORMATIONBUILDER_H
#define FORMATIONBUILDER_H

#include <vector>
#include "raylib.h"
#include <string>

struct SpawnData {
    Vector2 startPos;
    Vector2 targetPos;
    std::vector<Vector2> waypoints;
};

class FormationBuilder {
public:
    static std::vector<SpawnData> BuildGrid(int rows, int cols, float spacingX, float spacingY, float startY, float targetBaseY, float screenWidth);
    static std::vector<SpawnData> BuildVShape(int count, float spacingX, float spacingY, float startY, float targetBaseY, int layers, float layerSpacing, float screenWidth);
    static std::vector<SpawnData> BuildSweepToGrid(int rows, int colsPerSide, float spacingX, float spacingY, float targetBaseY, float screenWidth);
    static std::vector<SpawnData> BuildIntersectingV(int count, float spacingX, float spacingY, float targetBaseY, float screenWidth, float screenHeight);
    static std::vector<SpawnData> BuildRandomRain(int count, float duration, float startY, float speed, float screenWidth);
    static std::vector<SpawnData> BuildTargetedPlayer(float startY, float playerX);
};

#endif
