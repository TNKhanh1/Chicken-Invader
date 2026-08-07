import os
import re

def clean_wave_manager():
    base_dir = r"e:\ChickenInvader_local\ChickenInvader"
    h_path = os.path.join(base_dir, "include", "WaveManager.h")
    cpp_path = os.path.join(base_dir, "src", "WaveManager.cpp")

    # Clean header
    with open(h_path, "r", encoding="utf-8") as f:
        h_content = f.read()
    
    # Remove struct DelayedSpawn and std::vector<DelayedSpawn> delayedSpawns;
    h_content = re.sub(r'struct DelayedSpawn \{[^}]+\};\s*', '', h_content, flags=re.DOTALL)
    h_content = re.sub(r'std::vector<DelayedSpawn>\s+delayedSpawns;\s*', '', h_content)
    
    with open(h_path, "w", encoding="utf-8") as f:
        f.write(h_content)

    # Clean cpp Update
    with open(cpp_path, "r", encoding="utf-8") as f:
        cpp_content = f.read()

    update_body = """void WaveManager::Update(float deltaTime) {
    // No longer handling delayed spawns because original logic spawns all at once.
}
"""
    cpp_content = re.sub(r'void WaveManager::Update\(float deltaTime\) \{.*?(?=bool WaveManager::SpawnBatch)', update_body, cpp_content, flags=re.DOTALL)
    
    with open(cpp_path, "w", encoding="utf-8") as f:
        f.write(cpp_content)
        
    print("Cleaned WaveManager.")

if __name__ == "__main__":
    clean_wave_manager()
