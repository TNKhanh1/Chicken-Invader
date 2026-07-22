#include "raylib.h"
#include <iostream>

int main() {
    InitWindow(100, 100, "Invisible");
    Image img = LoadImage("thamkhao/Sprite_References/ChickenEnemy.png");
    if (img.data == nullptr) {
        std::cerr << "Failed to load image!" << std::endl;
        CloseWindow();
        return 1;
    }
    
    std::cout << "Image Size: " << img.width << "x" << img.height << std::endl;
    
    UnloadImage(img);
    CloseWindow();
    return 0;
}
