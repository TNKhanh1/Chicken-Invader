import os

def create_formation_builder():
    base_dir = r"e:\ChickenInvader_local\ChickenInvader"
    h_path = os.path.join(base_dir, "include", "FormationBuilder.h")
    cpp_path = os.path.join(base_dir, "src", "FormationBuilder.cpp")

    h_content = """#ifndef FORMATIONBUILDER_H
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
"""

    cpp_content = """#include "FormationBuilder.h"
#include <cmath>

std::vector<SpawnData> FormationBuilder::BuildGrid(int rows, int cols, float spacingX, float spacingY, float startY, float targetBaseY, float screenWidth) {
    std::vector<SpawnData> data;
    float startX = screenWidth / 2.0f;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            float offsetX = (c - (cols - 1) / 2.0f) * spacingX;
            float x = startX + offsetX;
            float y = startY + r * spacingY;
            float targetY = targetBaseY + r * spacingY;
            data.push_back({{x, y}, {x, targetY}, {}});
        }
    }
    return data;
}

std::vector<SpawnData> FormationBuilder::BuildVShape(int count, float spacingX, float spacingY, float startY, float targetBaseY, int layers, float layerSpacing, float screenWidth) {
    std::vector<SpawnData> data;
    float midIndex = (count - 1) / 2.0f;
    for (int layer = 0; layer < layers; ++layer) {
        for (int i = 0; i < count; ++i) {
            float x = screenWidth / 2.0f + (i - midIndex) * spacingX;
            float y = startY - std::abs(i - midIndex) * spacingY - layer * layerSpacing;
            float targetY = targetBaseY + std::abs(i - midIndex) * spacingY + layer * layerSpacing;
            data.push_back({{x, y}, {x, targetY}, {}});
        }
    }
    return data;
}

std::vector<SpawnData> FormationBuilder::BuildSweepToGrid(int rows, int colsPerSide, float spacingX, float spacingY, float targetBaseY, float screenWidth) {
    std::vector<SpawnData> data;
    int countPerSide = rows * colsPerSide;
    for (int col = 0; col < 2; ++col) {
        float startX = (col == 0) ? -100.0f : screenWidth + 100.0f;
        for (int i = 0; i < countPerSide; ++i) {
            float startY = 300.0f + i * 150.0f;
            float intersectX = (col == 0) ? (screenWidth / 2.0f - 150.0f) : (screenWidth / 2.0f + 150.0f);
            float intersectY = startY;
            float gridY = targetBaseY + (i / colsPerSide) * spacingY;
            float gridX = screenWidth / 2.0f + ((col == 0) ? (-300.0f + (i % colsPerSide) * 200.0f) : (100.0f + (i % colsPerSide) * 200.0f));
            
            SpawnData sd;
            sd.startPos = {startX, startY};
            sd.waypoints = {{intersectX, intersectY}, {gridX, gridY}};
            data.push_back(sd);
        }
    }
    return data;
}

std::vector<SpawnData> FormationBuilder::BuildIntersectingV(int count, float spacingX, float spacingY, float targetBaseY, float screenWidth, float screenHeight) {
    std::vector<SpawnData> data;
    float midIndex = (count - 1) / 2.0f;
    for (int i = 0; i < count; ++i) {
        // Top V
        float topStartX = screenWidth / 2.0f + (i - midIndex) * spacingX;
        float topStartY = -100.0f - std::abs(i - midIndex) * spacingY;
        float intersectX = topStartX;
        float intersectY = screenHeight / 2.0f;
        float gridX_top = screenWidth / 2.0f - 200.0f + (i % 5) * 100.0f;
        float gridY_top = targetBaseY + (i / 5) * 100.0f;
        
        SpawnData sdTop;
        sdTop.startPos = {topStartX, topStartY};
        sdTop.waypoints = {{intersectX, intersectY}, {gridX_top, gridY_top}};
        data.push_back(sdTop);
        
        // Bottom V
        float botStartX = screenWidth / 2.0f + (i - midIndex) * spacingX;
        float botStartY = screenHeight + 100.0f + std::abs(i - midIndex) * spacingY;
        float gridX_bot = screenWidth / 2.0f - 200.0f + (i % 5) * 100.0f;
        float gridY_bot = targetBaseY + 200.0f + (i / 5) * 100.0f;
        
        SpawnData sdBot;
        sdBot.startPos = {botStartX, botStartY};
        sdBot.waypoints = {{intersectX, intersectY}, {gridX_bot, gridY_bot}};
        data.push_back(sdBot);
    }
    return data;
}

std::vector<SpawnData> FormationBuilder::BuildRandomRain(int count, float duration, float startY, float speed, float screenWidth) {
    std::vector<SpawnData> data;
    for (int i = 0; i < count; i++) {
        float x = (float)GetRandomValue(100, (int)screenWidth - 100);
        // spread out over duration: delay is implicit in starting Y high up
        float maxOffset = duration * speed;
        float y = startY - GetRandomValue(0, (int)maxOffset);
        data.push_back({{x, y}, {x, 0.0f}, {}});
    }
    return data;
}

std::vector<SpawnData> FormationBuilder::BuildTargetedPlayer(float startY, float playerX) {
    std::vector<SpawnData> data;
    data.push_back({{playerX, startY}, {playerX, 0.0f}, {}});
    return data;
}
"""

    with open(h_path, "w", encoding="utf-8") as f:
        f.write(h_content)
    with open(cpp_path, "w", encoding="utf-8") as f:
        f.write(cpp_content)
    
    print("Created FormationBuilder classes.")

if __name__ == "__main__":
    create_formation_builder()
