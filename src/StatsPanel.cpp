#include "StatsPanel.h"
#include "Spaceship.h"
#include "GameManager.h" // for DrawTextCustom
#include "FontManager.h"
#include "raymath.h"
#include <string>

// Option A: Tái định nghĩa mảng Argument ngắn gọn nội bộ
static const char* ARGUMENT_NAMES[10] = {
    "EXP Amplifier",
    "Stat Windfall",
    "Abundant Gifts",
    "Boss Hunter",
    "Armor Crusher",
    "Blood Fury",
    "Bloodthirst",
    "Energy Flow",
    "Round Recovery",
    "Fast Track"
};

static const char* ARGUMENT_DESC[10] = {
    "+50% EXP from all enemies",
    "+3 Stat Selections now",
    "4 choices in Stat Selection",
    "+80% bonus damage to Bosses",
    "Bonus dmg = 3% target HP",
    "+2 permanent flat Damage on kill",
    "Restore 5% Max HP on kill",
    "Restore 2 Mana per shot",
    "Restore 30% Max HP at wave start",
    "Immediately gain 3 Levels"
};

StatsPanel::StatsPanel() {
    slideX = PANEL_X_HIDE;
}

void StatsPanel::Update(float deltaTime, bool tabHeld) {
    if (tabHeld) {
        slideX = slideX + (PANEL_X_SHOW - slideX) * LERP_IN_SPD * deltaTime;
    } else {
        slideX = slideX + (PANEL_X_HIDE - slideX) * LERP_OUT_SPD * deltaTime;
    }
}

void StatsPanel::DrawRow(float x, float& y, const char* label, const char* value, Color valueColor) const {
    FontManager::GetInstance()->DrawGameText(label, (int)x, (int)y, 20, LIGHTGRAY, "Modern");
    FontManager::GetInstance()->DrawGameText(value, (int)(x + 130), (int)y, 20, valueColor, "Modern");
    y += 28.0f;
}

void StatsPanel::Draw(const Spaceship* player, int screenHeight) const {
    if (slideX <= PANEL_X_HIDE + 2.0f || player == nullptr) return;

    // Tính toán chiều cao
    float panelH = 340.0f + (player->activeArguments.size() * 55.0f);
    if (panelH > screenHeight - 40.0f) panelH = screenHeight - 40.0f;
    float panelY = (screenHeight - panelH) / 2.0f;

    Rectangle bounds = { slideX, panelY, PANEL_W, panelH };

    // Vẽ nền và viền
    DrawRectangleRec(bounds, {0, 0, 0, 210});
    DrawRectangleLinesEx(bounds, 1.0f, {80, 180, 255, 200});

    float cx = slideX + 20.0f;
    float cy = panelY + 20.0f;

    // Header
    FontManager::GetInstance()->DrawGameText("[ PLAYER STATS ]", (int)(slideX + 80), (int)cy, 24, YELLOW, "Modern");
    cy += 40.0f;

    // --- CÁC CHỈ SỐ CƠ BẢN ---
    char buf[128];

    // HP
    snprintf(buf, sizeof(buf), "%.0f / %.0f", player->GetHp(), player->GetMaxHp());
    DrawRow(cx, cy, "HP", buf, GREEN);

    // Damage
    float permDmg = player->GetPermanentDamageBonus();
    if (permDmg > 0) {
        snprintf(buf, sizeof(buf), "%.0f (+%.0f bonus)", player->GetDamage(), permDmg);
    } else {
        snprintf(buf, sizeof(buf), "%.0f", player->GetDamage());
    }
    DrawRow(cx, cy, "Damage", buf, ORANGE);

    // Armor
    snprintf(buf, sizeof(buf), "%.0f", player->GetArmor());
    DrawRow(cx, cy, "Armor", buf, GRAY);

    // Fire Rate
    snprintf(buf, sizeof(buf), "%.2f /s", player->GetAttackSpeed());
    DrawRow(cx, cy, "Fire Rate", buf, RAYWHITE);

    // Crit Chance
    snprintf(buf, sizeof(buf), "%.0f%%", player->GetCritChance());
    DrawRow(cx, cy, "Crit Chance", buf, RED);

    // Crit Damage
    snprintf(buf, sizeof(buf), "%.0f%%", player->GetCritDamage());
    DrawRow(cx, cy, "Crit Damage", buf, MAROON);

    // Mana
    snprintf(buf, sizeof(buf), "%.0f / %.0f", player->GetCurrentMana(), player->GetMaxMana());
    DrawRow(cx, cy, "Mana", buf, BLUE);

    // Level
    snprintf(buf, sizeof(buf), "%d", player->GetLevel());
    DrawRow(cx, cy, "Level", buf, YELLOW);

    // EXP
    snprintf(buf, sizeof(buf), "%.0f / %.0f", player->GetCurrentExp(), player->GetMaxExp());
    DrawRow(cx, cy, "EXP", buf, PURPLE);

    // --- ĐƯỜNG PHÂN CÁCH ---
    cy += 10.0f;
    DrawLine((int)slideX + 20, (int)cy, (int)(slideX + PANEL_W - 20), (int)cy, {80, 180, 255, 100});
    cy += 20.0f;

    // --- LÕI (ARGUMENTS) ---
    const auto& args = player->GetActiveArguments();
    snprintf(buf, sizeof(buf), "[ ARGUMENTS (%d owned) ]", (int)args.size());
    FontManager::GetInstance()->DrawGameText(buf, (int)slideX + 20, (int)cy, 22, GOLD, "Modern");
    cy += 35.0f;

    for (int argId : args) {
        if (argId >= 0 && argId < 10) {
            FontManager::GetInstance()->DrawGameText(TextFormat("- %s", ARGUMENT_NAMES[argId]), (int)cx, (int)cy, 20, SKYBLUE, "Modern");
            cy += 25;
            FontManager::GetInstance()->DrawGameText(ARGUMENT_DESC[argId], (int)(cx + 20), (int)cy, 18, LIGHTGRAY, "Modern");
            cy += 30.0f;
        }
    }
}
