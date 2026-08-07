import os
import re

def refactor_game_manager():
    base_dir = r"e:\ChickenInvader_local\ChickenInvader"
    h_path = os.path.join(base_dir, "include", "GameManager.h")
    cpp_path = os.path.join(base_dir, "src", "GameManager.cpp")

    # 1. GameManager.cpp modifications
    with open(cpp_path, "r", encoding="utf-8") as f:
        cpp_content = f.read()

    # Inject #include "WaveManager.h"
    if '#include "WaveManager.h"' not in cpp_content:
        cpp_content = cpp_content.replace('#include "GameManager.h"', '#include "GameManager.h"\n#include "WaveManager.h"')

    # Replace GameManager::SpawnWaveBatch logic
    # Find bool GameManager::SpawnWaveBatch(int wave, int batch) { ... }
    # Since it's large, we'll use regex to replace the entire body.
    spawn_wave_pattern = re.compile(r'bool GameManager::SpawnWaveBatch\(int wave, int batch\) \{.*?\n\}', re.DOTALL)
    
    new_spawn_wave = """bool GameManager::SpawnWaveBatch(int wave, int batch) {
    return WaveManager::GetInstance()->SpawnBatch(wave, batch);
}"""

    cpp_content = re.sub(spawn_wave_pattern, new_spawn_wave, cpp_content)

    # In InitGame(), load the stage json.
    init_game_pattern = r'(void GameManager::InitGame\(\) \{.*?ResetPlayer\(\);)'
    init_game_repl = r'\1\n    WaveManager::GetInstance()->LoadStage("data/stage1.json");'
    cpp_content = re.sub(init_game_pattern, init_game_repl, cpp_content, flags=re.DOTALL)

    # In Update(float deltaTime), call WaveManager::Update inside GameState::GAMEPLAY
    update_pattern = r'(case GameState::GAMEPLAY: \{)'
    update_repl = r'\1\n        WaveManager::GetInstance()->Update(deltaTime);'
    cpp_content = re.sub(update_pattern, update_repl, cpp_content)

    with open(cpp_path, "w", encoding="utf-8") as f:
        f.write(cpp_content)

    print("Refactored GameManager.cpp successfully.")

if __name__ == "__main__":
    refactor_game_manager()
