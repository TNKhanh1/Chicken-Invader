#ifndef GAME_STATE_H
#define GAME_STATE_H

// Enum định nghĩa các trạng thái của game
enum class GameState {
    MAIN_MENU,
    PLAYING,
    PAUSED,
    SHOP,
    ENCYCLOPEDIA, // Sách bách khoa toàn thư
    GAME_OVER,
    
    // Test states
    TEST_MENU,
    TEST_ENEMY,
    TEST_SPACESHIP,
    TEST_GAMEPLAY,
    WAVE_SELECTION,
    SETTINGS,
    COMING_SOON,
    STAT_SELECTION,       // Màn hình chọn chỉ số - hiện trước mỗi wave
    ARGUMENT_SELECTION,   // Màn hình chọn argument (lõi) - hiện trước wave 5/10/15
    WAVE_INTRO,           // Hiệu ứng chuyển wave: warp speed background + chữ WAVE X
    PLAYER_SELECT,        // Màn hình chọn chế độ 1P/2P/AI
    RUNE_SELECTION        // Màn hình chọn Runes đầu game
};

#endif // GAME_STATE_H
