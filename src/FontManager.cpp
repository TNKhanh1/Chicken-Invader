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
    int codepoints[1000];
    int count = 0;
    for (int i = 32; i <= 126; i++) codepoints[count++] = i; // Basic Latin
    for (int i = 161; i <= 255; i++) codepoints[count++] = i; // Latin-1 Supplement
    for (int i = 0x0100; i <= 0x017F; i++) codepoints[count++] = i; // Latin Extended-A
    for (int i = 0x0180; i <= 0x024F; i++) codepoints[count++] = i; // Latin Extended-B
    for (int i = 0x1EA0; i <= 0x1EF9; i++) codepoints[count++] = i; // Latin Extended Additional (Tiếng Việt)
    
    // Load Modern Font (Inter-Bold) with high base size to prevent pixelation on scale
    Font modern = LoadFontEx("assets/fonts/static/Inter-Bold.ttf", 128, codepoints, count);
    SetTextureFilter(modern.texture, TEXTURE_FILTER_BILINEAR);
    fonts["Modern"] = modern;
    
    // Load Retro Font (retro-pixel-thick)
    // NOTE: Retro fonts might look better with POINT filter, but the user explicitly
    // requested to avoid blurriness and pixelation. Bilinear with high res handles scaling best.
    Font retro = LoadFontEx("assets/fonts/retro-pixel-thick.ttf", 128, codepoints, count);
    SetTextureFilter(retro.texture, TEXTURE_FILTER_BILINEAR);
    fonts["Retro"] = retro;
    
    // Load Retro Arcade (retro-pixel-arcade) just in case
    Font arcade = LoadFontEx("assets/fonts/retro-pixel-arcade.ttf", 128, codepoints, count);
    SetTextureFilter(arcade.texture, TEXTURE_FILTER_BILINEAR);
    fonts["Arcade"] = arcade;
}

void FontManager::UnloadFonts() {
    for (auto& pair : fonts) {
        UnloadFont(pair.second);
    }
    fonts.clear();
}

Font FontManager::GetFont(const std::string& fontId) const {
    auto it = fonts.find(fontId);
    if (it != fonts.end()) {
        return it->second;
    }
    return GetFontDefault();
}

void FontManager::DrawGameText(const std::string& text, float x, float y, float fontSize, Color color, const std::string& fontId) {
    Font font = GetFont(fontId);
    
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
    Font font = GetFont(fontId);
    float spacing = (fontId == "Retro" || fontId == "Arcade") ? fontSize / 15.0f : fontSize / 20.0f;
    if (spacing < 1.0f) spacing = 1.0f;
    
    return MeasureTextEx(font, text.c_str(), fontSize, spacing);
}
