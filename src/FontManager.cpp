#include "FontManager.h"
#include <iostream>

FontManager* FontManager::instance = nullptr;

FontManager* FontManager::GetInstance() {
    if (instance == nullptr) {
        instance = new FontManager();
    }
    return instance;
}

void FontManager::DestroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

void FontManager::LoadFonts() {
    // Nạp Font hỗ trợ tiếng Việt
    count = 0;
    for (int i = 32; i <= 126; i++) codepoints[count++] = i; // Basic Latin
    for (int i = 161; i <= 255; i++) codepoints[count++] = i; // Latin-1 Supplement
    for (int i = 0x0100; i <= 0x017F; i++) codepoints[count++] = i; // Latin Extended-A
    for (int i = 0x0180; i <= 0x024F; i++) codepoints[count++] = i; // Latin Extended-B
    for (int i = 0x1EA0; i <= 0x1EF9; i++) codepoints[count++] = i; // Latin Extended Additional (Tiếng Việt)
    
    fontPaths["Modern"] = "assets/fonts/static/Inter-Bold.ttf";
    fontPaths["Retro"] = "assets/fonts/retro-pixel-thick.ttf";
    fontPaths["Arcade"] = "assets/fonts/retro-pixel-arcade.ttf";

    // Preload common font sizes to avoid runtime stutter
    std::vector<int> commonSizes = { 10, 14, 16, 18, 20, 24, 30, 40, 50 };
    for (int size : commonSizes) {
        GetFont("Modern", size);
        GetFont("Retro", size);
    }
}

void FontManager::UnloadFonts() {
    for (auto& pair : fonts) {
        UnloadFont(pair.second);
    }
    fonts.clear();
}

int FontManager::GetNearestFontSize(int targetSize) {
    std::vector<int> commonSizes = { 10, 14, 16, 18, 20, 24, 30, 40, 50, 60, 72, 96 };
    int bestSize = commonSizes[0];
    int minDiff = std::abs(targetSize - bestSize);
    
    for (int size : commonSizes) {
        int diff = std::abs(targetSize - size);
        if (diff < minDiff) {
            minDiff = diff;
            bestSize = size;
        }
    }
    
    // Nếu chênh lệch quá lớn (vd target = 150), cho phép tạo size mới
    if (minDiff > 10) return targetSize;
    return bestSize;
}

Font FontManager::GetFont(const std::string& fontId, int fontSize) {
    // Round to nearest common size to prevent loading too many font textures
    int targetSize = GetNearestFontSize(fontSize);
    
    std::string key = fontId + "_" + std::to_string(targetSize);
    
    auto it = fonts.find(key);
    if (it != fonts.end()) {
        return it->second;
    }
    
    auto pathIt = fontPaths.find(fontId);
    if (pathIt != fontPaths.end()) {
        Font newFont = LoadFontEx(pathIt->second.c_str(), targetSize, codepoints, count);
        
        if (fontId == "Modern") {
            SetTextureFilter(newFont.texture, TEXTURE_FILTER_BILINEAR);
        } else {
            SetTextureFilter(newFont.texture, TEXTURE_FILTER_POINT);
        }
        
        fonts[key] = newFont;
        return newFont;
    }
    
    return GetFontDefault();
}

void FontManager::DrawGameText(const std::string& text, float x, float y, float fontSize, Color color, const std::string& fontId) {
    int sizeInt = (int)std::round(fontSize);
    Font font = GetFont(fontId, sizeInt);
    
    // Ép kiểu tọa độ về số nguyên (hoặc làm tròn) để tránh tình trạng filter làm nhòe chữ khi vẽ ở nửa pixel (fractional pixel).
    Vector2 pos = { std::round(x), std::round(y) };
    
    // Spacing mặc định thường là fontSize / 10
    float spacing = (fontId == "Retro" || fontId == "Arcade") ? fontSize / 15.0f : fontSize / 20.0f;
    if (spacing < 1.0f) spacing = 1.0f;
    
    DrawTextEx(font, text.c_str(), pos, fontSize, spacing, color);
}

void FontManager::DrawGameTextCentered(const std::string& text, float centerX, float y, float fontSize, Color color, const std::string& fontId) {
    Vector2 size = MeasureGameText(text, fontSize, fontId);
    DrawGameText(text, centerX - size.x / 2.0f, y, fontSize, color, fontId);
}

Vector2 FontManager::MeasureGameText(const std::string& text, float fontSize, const std::string& fontId) {
    int sizeInt = (int)std::round(fontSize);
    Font font = GetFont(fontId, sizeInt);
    
    float spacing = (fontId == "Retro" || fontId == "Arcade") ? fontSize / 15.0f : fontSize / 20.0f;
    if (spacing < 1.0f) spacing = 1.0f;
    
    return MeasureTextEx(font, text.c_str(), fontSize, spacing);
}
