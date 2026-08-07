with open('src/GameManager.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

old_logic = '''                } else if (activeEnemies.empty()) {
                    // Cấu trúc wave hiện tại đều có 3 batch (từ wave 1 đến wave 4)
                    int maxBatch = 3; 
                    if (currentBatch < maxBatch) {
                        // Khi dọn sạch batch hiện tại, chuẩn bị sang batch tiếp theo
                        currentBatch++;
                        isWaveTransitioning = true;
                        waveTimer = 3.0f;
                    } else {
                        // Nếu đã xong batch cuối của wave, chờ cho hết items rơi xuống rồi chuyển sang chọn chỉ số
                        if (activeItems.empty()) {
                            EnterStatSelection(currentWave + 1);
                        }
                    }
                }'''

new_logic = '''                } else if (activeEnemies.empty()) {
                    int maxBatch = WaveManager::GetInstance()->GetMaxBatchForWave(currentWave);
                    if (currentBatch < maxBatch) {
                        currentBatch++;
                        isWaveTransitioning = true;
                        waveTimer = 3.0f;
                    } else {
                        if (activeItems.empty()) {
                            if (currentWave < WaveManager::GetInstance()->GetTotalWaves()) {
                                EnterStatSelection(currentWave + 1);
                            } else {
                                currentStage++;
                                currentWave = 1;
                                currentBatch = 1;
                                WaveManager::GetInstance()->LoadStage("data/stage" + std::to_string(currentStage) + ".json");
                                EnterStatSelection(currentWave);
                            }
                        }
                    }
                }'''
content = content.replace(old_logic, new_logic)

with open('src/GameManager.cpp', 'w', encoding='utf-8') as f:
    f.write(content)
