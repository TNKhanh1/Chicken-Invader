#include "raylib.h"
#include <iostream>
#include <vector>

int main() {
    InitWindow(100, 100, "Scanner");
    Image img = LoadImage("assets/asteroidNormal.png");
    if (img.data == nullptr) {
        std::cerr << "Failed to load image!" << std::endl;
        CloseWindow();
        return 1;
    }
    
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
    
    std::cout << "Empty columns (gaps): ";
    for (int x = 0; x < img.width; x++) {
        if (colPixels[x] == 0) std::cout << x << " ";
    }
    std::cout << "\nEmpty rows (gaps): ";
    for (int y = 0; y < img.height; y++) {
        if (rowPixels[y] == 0) std::cout << y << " ";
    }
    std::cout << std::endl;
    
    UnloadImage(img);
    CloseWindow();
    return 0;
}
