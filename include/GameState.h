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
    SETTINGS,
    COMING_SOON
};

#endif // GAME_STATE_H
