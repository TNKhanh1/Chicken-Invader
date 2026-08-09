#include "FormationBuilder.h"
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
            float startY = 50.0f + i * 120.0f; // Tăng khoảng cách rơi dọc để không bị dính vào nhau
            float intersectX = (col == 0) ? (screenWidth / 2.0f - spacingX) : (screenWidth / 2.0f + spacingX);
            float intersectY = startY;
            float gridY = targetBaseY + (i / colsPerSide) * spacingY;
            
            float colOffsetX = (col == 0) ? -spacingX - (colsPerSide - 1 - (i % colsPerSide)) * spacingX : spacingX + (i % colsPerSide) * spacingX;
            float gridX = screenWidth / 2.0f + colOffsetX;
            
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
        // Tính toán tọa độ dạng lưới chung (2 nửa tạo thành hình chữ nhật hoàn chỉnh)
        int cols = 5;
        float gridOffsetX = ((i % cols) - (cols - 1) / 2.0f) * spacingX;
        int topRows = (count + cols - 1) / cols;

        // Top V
        float topStartX = screenWidth / 2.0f + (i - midIndex) * spacingX;
        float topStartY = -100.0f - std::abs(i - midIndex) * spacingY;
        float intersectX = topStartX;
        float intersectY = screenHeight / 2.0f;
        
        float gridX_top = screenWidth / 2.0f + gridOffsetX;
        float gridY_top = targetBaseY + (i / cols) * spacingY;
        
        SpawnData sdTop;
        sdTop.startPos = {topStartX, topStartY};
        sdTop.waypoints = {{intersectX, intersectY}, {gridX_top, gridY_top}};
        data.push_back(sdTop);
        
        // Bottom V
        float botStartX = screenWidth / 2.0f + (i - midIndex) * spacingX;
        // Giới hạn y không vượt quá mức cull (screenHeight + 300) của Enemy
        float botStartY = screenHeight + 50.0f + std::abs(i - midIndex) * (spacingY * 0.5f);
        
        float gridX_bot = screenWidth / 2.0f + gridOffsetX;
        float gridY_bot = targetBaseY + (topRows + i / cols) * spacingY;
        
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

std::vector<SpawnData> FormationBuilder::BuildRing(int count, float radius, Vector2 center) {
    std::vector<SpawnData> data;
    if (count <= 0) return data;
    float angleStep = 2.0f * PI / count;
    for (int i = 0; i < count; ++i) {
        float angle = i * angleStep;
        float dx = std::cos(angle) * radius;
        float dy = std::sin(angle) * radius;
        
        Vector2 targetPos = {center.x + dx, center.y + dy};
        
        // They spawn way outside the screen and move to their target
        Vector2 startPos = {center.x + dx * 3.0f, center.y - 400.0f + dy}; 
        
        data.push_back({startPos, targetPos, {targetPos}});
    }
    return data;
}
