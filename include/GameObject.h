#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "raylib.h"

// Lớp cơ sở (Base Class) cho tất cả các đối tượng trong game
class GameObject {
protected:
    Vector2 position;
    Texture2D texture;
    bool isActive;

public:
    GameObject() : position({0.0f, 0.0f}), isActive(true) {}
    GameObject(Vector2 pos) : position(pos), isActive(true) {}
    
    virtual ~GameObject() = default;

    // Các hàm vòng đời chính
    virtual void Init() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Draw() = 0;

    // Getters / Setters
    Vector2 GetPosition() const { return position; }
    void SetPosition(Vector2 pos) { position = pos; }
    
    bool IsActive() const { return isActive; }
    void SetActive(bool active) { isActive = active; }
};

#endif // GAMEOBJECT_H
