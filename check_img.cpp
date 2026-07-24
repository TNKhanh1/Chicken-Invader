#include <iostream>
#include "raylib.h"

int main() {
    InitWindow(100, 100, "temp");
    Image img1 = LoadImage("assets/asteroidType2.png");
    std::cout << "AsteroidType2: " << img1.width << "x" << img1.height << std::endl;
    UnloadImage(img1);
    CloseWindow();
    return 0;
}
