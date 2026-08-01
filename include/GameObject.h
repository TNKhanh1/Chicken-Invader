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
    GameObject();
    GameObject(Vector2 pos);
    
    virtual ~GameObject() {
        // printf("Destroying GameObject\n");
    }

    // Các hàm vòng đời chính
    virtual void Init() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Draw() = 0;

    // Getters / Setters
    Vector2 GetPosition() const;
    void SetPosition(Vector2 pos);
    
    bool IsActive() const;
    void SetActive(bool active);
};

#endif // GAMEOBJECT_H
