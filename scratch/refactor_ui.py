import re
import sys

def refactor_test_ui():
    header_path = r'e:\ChickenInvader_local\ChickenInvader\include\GameManager.h'
    cpp_path = r'e:\ChickenInvader_local\ChickenInvader\src\GameManager.cpp'

    # 1. Update GameManager.h
    with open(header_path, 'r', encoding='utf-8') as f:
        h_content = f.read()

    # We previously added testSelectedStage, let's replace all of them with a struct
    old_h_vars = r"""    // Test selection
    int testSelectedStage = 1;
    int testSelectedWave = 1;
    int testSelectedBatch = 1;"""

    new_h_vars = """    // Test selection configuration (Encapsulated)
    struct TestConfig {
        int stage = 1;
        int wave = 1;
        int batch = 1;
        int maxStage = 2;
        int maxWave = 10;
        int maxBatch = 5;
    } testConfig;"""

    if old_h_vars in h_content:
        h_content = h_content.replace(old_h_vars, new_h_vars)
        with open(header_path, 'w', encoding='utf-8') as f:
            f.write(h_content)
        print("Updated GameManager.h successfully.")
    else:
        print("Could not find variables in GameManager.h. Maybe they are already updated.")

    # 2. Update GameManager.cpp
    with open(cpp_path, 'r', encoding='utf-8') as f:
        cpp_content = f.read()

    old_cpp_ui = r"""        case GameState::WAVE_SELECTION: \{
            DrawText\("TEST STAGE & WAVE", screenWidth/2 - MeasureText\("TEST STAGE & WAVE", 40\)/2, 120, 40, YELLOW\);
            
            // Buttons to select Stage
            DrawText\(TextFormat\("STAGE: %d", testSelectedStage\), screenWidth/2 - 70, 220, 30, WHITE\);
            if \(DrawButton\(\{ \(float\)screenWidth/2 - 150, 210, 50, 50 \}, "<"\)\) \{
                if \(testSelectedStage > 1\) \{ testSelectedStage--; testSelectedWave = 1; testSelectedBatch = 1; \}
            \}
            if \(DrawButton\(\{ \(float\)screenWidth/2 \+ 100, 210, 50, 50 \}, ">"\)\) \{
                if \(testSelectedStage < 2\) \{ testSelectedStage\+\+; testSelectedWave = 1; testSelectedBatch = 1; \} // Giới hạn tạm ở Stage 2
            \}

            // Buttons to select Wave
            DrawText\(TextFormat\("WAVE: %d", testSelectedWave\), screenWidth/2 - 70, 320, 30, WHITE\);
            if \(DrawButton\(\{ \(float\)screenWidth/2 - 150, 310, 50, 50 \}, "<"\)\) \{
                if \(testSelectedWave > 1\) \{ testSelectedWave--; testSelectedBatch = 1; \}
            \}
            if \(DrawButton\(\{ \(float\)screenWidth/2 \+ 100, 310, 50, 50 \}, ">"\)\) \{
                if \(testSelectedWave < 10\) \{ testSelectedWave\+\+; testSelectedBatch = 1; \}
            \}

            // Buttons to select Batch
            DrawText\(TextFormat\("BATCH: %d", testSelectedBatch\), screenWidth/2 - 70, 420, 30, WHITE\);
            if \(DrawButton\(\{ \(float\)screenWidth/2 - 150, 410, 50, 50 \}, "<"\)\) \{
                if \(testSelectedBatch > 1\) testSelectedBatch--;
            \}
            if \(DrawButton\(\{ \(float\)screenWidth/2 \+ 100, 410, 50, 50 \}, ">"\)\) \{
                int maxBatch = 5; 
                if \(testSelectedBatch < maxBatch\) testSelectedBatch\+\+;
            \}

            if \(DrawButton\(\{ \(float\)screenWidth/2 - 100, 520, 200, 50 \}, "START TEST"\)\) \{
                currentStage = testSelectedStage;
                currentWave = testSelectedWave;
                currentBatch = testSelectedBatch;"""

    new_cpp_ui = """        case GameState::WAVE_SELECTION: {
            DrawText("TEST STAGE & WAVE", screenWidth/2 - MeasureText("TEST STAGE & WAVE", 40)/2, 120, 40, YELLOW);
            
            // Fix Out-of-Bounds Issue: Show warning if trying to start non-existent waves
            bool isValidSelection = (testConfig.stage == 1 && testConfig.wave <= 4);
            if (!isValidSelection) {
                DrawText("WARNING: WAVE NOT YET IMPLEMENTED", screenWidth/2 - 200, 180, 20, RED);
            }

            // Buttons to select Stage
            DrawText(TextFormat("STAGE: %d", testConfig.stage), screenWidth/2 - 70, 220, 30, WHITE);
            if (DrawButton({ (float)screenWidth/2 - 150, 210, 50, 50 }, "<")) {
                if (testConfig.stage > 1) { testConfig.stage--; testConfig.wave = 1; testConfig.batch = 1; }
            }
            if (DrawButton({ (float)screenWidth/2 + 100, 210, 50, 50 }, ">")) {
                if (testConfig.stage < testConfig.maxStage) { testConfig.stage++; testConfig.wave = 1; testConfig.batch = 1; }
            }

            // Buttons to select Wave
            DrawText(TextFormat("WAVE: %d", testConfig.wave), screenWidth/2 - 70, 320, 30, WHITE);
            if (DrawButton({ (float)screenWidth/2 - 150, 310, 50, 50 }, "<")) {
                if (testConfig.wave > 1) { testConfig.wave--; testConfig.batch = 1; }
            }
            if (DrawButton({ (float)screenWidth/2 + 100, 310, 50, 50 }, ">")) {
                if (testConfig.wave < testConfig.maxWave) { testConfig.wave++; testConfig.batch = 1; }
            }

            // Buttons to select Batch
            DrawText(TextFormat("BATCH: %d", testConfig.batch), screenWidth/2 - 70, 420, 30, WHITE);
            if (DrawButton({ (float)screenWidth/2 - 150, 410, 50, 50 }, "<")) {
                if (testConfig.batch > 1) testConfig.batch--;
            }
            if (DrawButton({ (float)screenWidth/2 + 100, 410, 50, 50 }, ">")) {
                if (testConfig.batch < testConfig.maxBatch) testConfig.batch++;
            }

            if (DrawButton({ (float)screenWidth/2 - 100, 520, 200, 50 }, "START TEST")) {
                // Prevent starting broken waves
                if (!isValidSelection) return; 

                currentStage = testConfig.stage;
                currentWave = testConfig.wave;
                currentBatch = testConfig.batch;"""

    if re.search(old_cpp_ui, cpp_content):
        cpp_content = re.sub(old_cpp_ui, new_cpp_ui, cpp_content)
        with open(cpp_path, 'w', encoding='utf-8') as f:
            f.write(cpp_content)
        print("Updated GameManager.cpp successfully.")
    else:
        print("Could not find cpp block. Let's dump part of the file to see why.")

if __name__ == "__main__":
    refactor_test_ui()
