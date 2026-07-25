#ifndef MOVEMENT_STRATEGY_H
#define MOVEMENT_STRATEGY_H

#include "raylib.h"
#include <cmath>

// Interface cho Strategy Pattern (Cơ chế di chuyển của gà)
class IMovementBehavior {
public:
    virtual ~IMovementBehavior() = default;
    
    // Hàm di chuyển, truyền thêm screenWidth và screenHeight để có thể Wrap màn hình
    virtual void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) = 0;
};

// Các class cụ thể
class StraightMovement : public IMovementBehavior {
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override {
        // Gà bay thẳng xuống dưới
        position.y += moveSpeed * deltaTime;
    }
};

class HorizontalSweepMovement : public IMovementBehavior {
private:
    float direction;
public:
    HorizontalSweepMovement(float startDir) : direction(startDir) {}

    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override {
        // Move horizontally
        position.x += direction * moveSpeed * deltaTime;
        
        // Screen Wrap (Bay ra lề này thì chui ra từ lề kia)
        if (direction > 0 && position.x > screenWidth + 100) {
            position.x = -100;
        } else if (direction < 0 && position.x < -100) {
            position.x = screenWidth + 100;
        }
    }
};

class VerticalZigzagMovement : public IMovementBehavior {
private:
    float startX;
    bool initialized = false;
    float dx = 1.0f;
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override {
        if (!initialized) {
            startX = position.x;
            initialized = true;
        }
        
        // Move down steadily (chậm hơn so với trước)
        position.y += moveSpeed * 0.3f * deltaTime;
        
        // Linear zigzag horizontally
        position.x += dx * moveSpeed * 0.5f * deltaTime;

        if (position.x > startX + 250.0f) {
            dx = -1.0f;
        } else if (position.x < startX - 250.0f) {
            dx = 1.0f;
        }

        // Wrap around vertically (bay ra khỏi đáy thì ném lại lên đỉnh)
        if (position.y > screenHeight + 100) {
            position.y = -100;
        }
    }
};

class HorizontalBounceMovement : public IMovementBehavior {
private:
    float startX;
    float drift;
    float direction;
    float targetY;
    bool initialized = false;
public:
    HorizontalBounceMovement(float targetY, float drift = 300.0f, float startDir = 1.0f) 
        : targetY(targetY), drift(drift), direction(startDir) {}

    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override {
        if (!initialized) {
            startX = position.x;
            initialized = true;
        }

        // Đi xuống vị trí targetY trước
        if (position.y < targetY) {
            position.y += moveSpeed * deltaTime;
        } else {
            // Đã vào vị trí, bắt đầu đi ngang (Ping-pong)
            position.x += direction * moveSpeed * deltaTime;
            
            if (direction > 0 && position.x > startX + drift) {
                direction = -1.0f;
            } else if (direction < 0 && position.x < startX - drift) {
                direction = 1.0f;
            }
        }
    }
};

class MeteorDiveMovement : public IMovementBehavior {
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override {
        // Dive extremely fast straight down (3x speed)
        position.y += moveSpeed * 3.0f * deltaTime;
        // Kamikaze chickens do not wrap around, they just fly off screen
    }
};

class SineZigzagMovement : public IMovementBehavior {
private:
    float startX;
    float time = 0.0f;
    float amplitude = 150.0f;
    float frequency = 3.0f;
    bool initialized = false;
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override {
        if (!initialized) {
            startX = position.x;
            initialized = true;
        }
        time += deltaTime;
        
        position.y += moveSpeed * 0.5f * deltaTime;
        position.x = startX + sin(time * frequency) * amplitude;
        
        if (position.y > screenHeight + 100) {
            position.y = -100;
        }
    }
};

class SpiralMovement : public IMovementBehavior {
private:
    float time = 0.0f;
    float radius = 0.0f;
    float maxRadius = 250.0f;
    float rotationSpeed = 3.0f;
    Vector2 center;
    bool initialized = false;
public:
    void Move(Vector2& position, float moveSpeed, float deltaTime, int screenWidth, int screenHeight) override {
        if (!initialized) {
            center = position;
            initialized = true;
        }
        time += deltaTime;
        
        // Move center down slowly
        center.y += moveSpeed * 0.4f * deltaTime;
        
        // Gradually expand radius
        if (radius < maxRadius) {
            radius += 50.0f * deltaTime;
        }
        
        // Spiral around center
        position.x = center.x + cos(time * rotationSpeed) * radius;
        position.y = center.y + sin(time * rotationSpeed) * radius;
    }
};

#endif // MOVEMENT_STRATEGY_H
