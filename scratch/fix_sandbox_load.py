import os
import re

def fix_sandbox_load():
    base_dir = r"e:\ChickenInvader_local\ChickenInvader"
    cpp_path = os.path.join(base_dir, "src", "GameManager.cpp")

    with open(cpp_path, "r", encoding="utf-8") as f:
        content = f.read()

    # The code looks like this:
    #                 currentStage = testConfig.stage;
    #                 currentWave = testConfig.wave;
    #                 currentBatch = testConfig.batch;
    #                 isWaveTransitioning = true;
    
    pattern = r'(currentStage = testConfig\.stage;\s+currentWave = testConfig\.wave;\s+currentBatch = testConfig\.batch;\s+isWaveTransitioning = true;)'
    
    replacement = r'\1\n                WaveManager::GetInstance()->LoadStage("data/stage" + std::to_string(currentStage) + ".json");'
    
    content = re.sub(pattern, replacement, content)

    with open(cpp_path, "w", encoding="utf-8") as f:
        f.write(content)
        
    print("Fixed Sandbox JSON Loading.")

if __name__ == "__main__":
    fix_sandbox_load()
