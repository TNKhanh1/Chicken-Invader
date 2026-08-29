#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>

class FontManager {
private:
    static FontManager* instance;
    std::unordered_map<std::string, Font> fonts;
    std::unordered_map<std::string, std::string> fontPaths;
    
    int codepoints[1000];
    int count;

    FontManager() {}
    ~FontManager() { UnloadFonts(); }

public:
    static FontManager* GetInstance();
    static void DestroyInstance();

    void LoadFonts();
    void UnloadFonts();

    // Làm tròn kích thước font để giảm thiểu load nhiều texture
    int GetNearestFontSize(int targetSize);

    // Lấy font theo ID và kích thước
    Font GetFont(const std::string& fontId, int fontSize);

    // Hàm tiện ích bọc lại DrawTextEx
    // Mặc định dùng font "Modern" (Inter-Bold)
    void DrawGameText(const std::string& text, float x, float y, float fontSize, Color color, const std::string& fontId = "Modern");
    
    // Vẽ chữ căn giữa
    void DrawGameTextCentered(const std::string& text, float centerX, float y, float fontSize, Color color, const std::string& fontId = "Modern");
    
    // Đo kích thước chuỗi
    Vector2 MeasureGameText(const std::string& text, float fontSize, const std::string& fontId = "Modern");
};

#endif // FONT_MANAGER_H
