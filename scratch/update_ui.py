import re

def update_game_manager():
    file_path = r'e:\ChickenInvader_local\ChickenInvader\src\GameManager.cpp'
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    old_block = r"""        case GameState::WAVE_SELECTION: \{
            DrawText\("WAVE SELECTION", screenWidth/2 - MeasureText\("WAVE SELECTION", 40\)/2, 200, 40, YELLOW\);
            
            // Buttons to select Wave
            DrawText\(TextFormat\("WAVE: %d", testSelectedWave\), screenWidth/2 - 70, 300, 30, WHITE\);
            if \(DrawButton\(\{ \(float\)screenWidth/2 - 150, 290, 50, 50 \}, "<"\)\) \{
                if \(testSelectedWave > 1\) \{ testSelectedWave--; testSelectedBatch = 1; \}
            \}
            if \(DrawButton\(\{ \(float\)screenWidth/2 \+ 100, 290, 50, 50 \}, ">"\)\) \{
                if \(testSelectedWave < 4\) \{ testSelectedWave\+\+; testSelectedBatch = 1; \}
            \}

            // Buttons to select Batch
            DrawText\(TextFormat\("BATCH: %d", testSelectedBatch\), screenWidth/2 - 70, 400, 30, WHITE\);
            if \(DrawButton\(\{ \(float\)screenWidth/2 - 150, 390, 50, 50 \}, "<"\)\) \{
                if \(testSelectedBatch > 1\) testSelectedBatch--;
            \}
            if \(DrawButton\(\{ \(float\)screenWidth/2 \+ 100, 390, 50, 50 \}, ">"\)\) \{
                int maxBatch = 3; 
                if \(testSelectedBatch < maxBatch\) testSelectedBatch\+\+;
            \}

            if \(DrawButton\(\{ \(float\)screenWidth/2 - 100, 500, 200, 50 \}, "START TEST"\)\) \{
                currentWave = testSelectedWave;
                currentBatch = testSelectedBatch;"""

    new_block = """        case GameState::WAVE_SELECTION: {
            DrawText("TEST STAGE & WAVE", screenWidth/2 - MeasureText("TEST STAGE & WAVE", 40)/2, 120, 40, YELLOW);
            
            // Buttons to select Stage
            DrawText(TextFormat("STAGE: %d", testSelectedStage), screenWidth/2 - 70, 220, 30, WHITE);
            if (DrawButton({ (float)screenWidth/2 - 150, 210, 50, 50 }, "<")) {
                if (testSelectedStage > 1) { testSelectedStage--; testSelectedWave = 1; testSelectedBatch = 1; }
            }
            if (DrawButton({ (float)screenWidth/2 + 100, 210, 50, 50 }, ">")) {
                if (testSelectedStage < 2) { testSelectedStage++; testSelectedWave = 1; testSelectedBatch = 1; } // Giới hạn tạm ở Stage 2
            }

            // Buttons to select Wave
            DrawText(TextFormat("WAVE: %d", testSelectedWave), screenWidth/2 - 70, 320, 30, WHITE);
            if (DrawButton({ (float)screenWidth/2 - 150, 310, 50, 50 }, "<")) {
                if (testSelectedWave > 1) { testSelectedWave--; testSelectedBatch = 1; }
            }
            if (DrawButton({ (float)screenWidth/2 + 100, 310, 50, 50 }, ">")) {
                if (testSelectedWave < 10) { testSelectedWave++; testSelectedBatch = 1; }
            }

            // Buttons to select Batch
            DrawText(TextFormat("BATCH: %d", testSelectedBatch), screenWidth/2 - 70, 420, 30, WHITE);
            if (DrawButton({ (float)screenWidth/2 - 150, 410, 50, 50 }, "<")) {
                if (testSelectedBatch > 1) testSelectedBatch--;
            }
            if (DrawButton({ (float)screenWidth/2 + 100, 410, 50, 50 }, ">")) {
                int maxBatch = 5; 
                if (testSelectedBatch < maxBatch) testSelectedBatch++;
            }

            if (DrawButton({ (float)screenWidth/2 - 100, 520, 200, 50 }, "START TEST")) {
                currentStage = testSelectedStage;
                currentWave = testSelectedWave;
                currentBatch = testSelectedBatch;"""

    new_content = re.sub(old_block, new_block, content)

    if new_content != content:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print("Updated UI selection successfully.")
    else:
        print("Could not find the target block to replace.")

if __name__ == "__main__":
    update_game_manager()
