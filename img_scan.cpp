#include "raylib.h"
#include <iostream>
#include <fstream>
#include <vector>

int main() {
    InitWindow(100, 100, "Invisible");
    Image img = LoadImage("assets/spaceship/SpaceShip01.png");
    if (img.data == nullptr) {
        std::cerr << "Failed to load image!" << std::endl;
        CloseWindow();
        return 1;
    }
    
    std::cout << "Image Size: " << img.width << "x" << img.height << std::endl;
    
    // Scan rows and cols
    std::vector<int> colPixels(img.width, 0);
    std::vector<int> rowPixels(img.height, 0);
    
    for (int x = 0; x < img.width; x++) {
        for (int y = 0; y < img.height; y++) {
            Color c = GetImageColor(img, x, y);
            if (c.a > 10) {
                colPixels[x]++;
                rowPixels[y]++;
            }
        }
    }
    
    std::cout << "X bounds with pixels: ";
    bool inObject = false;
    for (int x = 0; x < img.width; x++) {
        if (colPixels[x] > 0 && !inObject) {
            std::cout << "[" << x << " to ";
            inObject = true;
        } else if (colPixels[x] == 0 && inObject) {
            std::cout << x - 1 << "] ";
            inObject = false;
        }
    }
    if (inObject) std::cout << img.width - 1 << "]";
    std::cout << std::endl;

    std::cout << "Y bounds with pixels: ";
    inObject = false;
    for (int y = 0; y < img.height; y++) {
        if (rowPixels[y] > 0 && !inObject) {
            std::cout << "[" << y << " to ";
            inObject = true;
        } else if (rowPixels[y] == 0 && inObject) {
            std::cout << y - 1 << "] ";
            inObject = false;
        }
    }
    if (inObject) std::cout << img.height - 1 << "]";
    std::cout << std::endl;

    UnloadImage(img);
    CloseWindow();
    return 0;
}
