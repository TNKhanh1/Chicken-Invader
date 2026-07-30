#include "GameObject.h"

GameObject::GameObject() : position({0.0f, 0.0f}), isActive(true) {}
GameObject::GameObject(Vector2 pos) : position(pos), isActive(true) {}

Vector2 GameObject::GetPosition() const { return position; }
void GameObject::SetPosition(Vector2 pos) { position = pos; }

bool GameObject::IsActive() const { return isActive; }
void GameObject::SetActive(bool active) { isActive = active; }
