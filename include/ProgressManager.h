#ifndef PROGRESSMANAGER_H
#define PROGRESSMANAGER_H

#include <string>

class ProgressManager {
private:
    static ProgressManager* instance;
    int highestUnlockedStage;
    std::string dataFilePath;

    ProgressManager();
    ~ProgressManager() = default;

public:
    // Delete copy constructor and assignment operator
    ProgressManager(const ProgressManager&) = delete;
    void operator=(const ProgressManager&) = delete;

    static ProgressManager* GetInstance();

    void LoadProgress();
    void SaveProgress();
    void ResetAllProgress();

    int GetHighestUnlockedStage() const;
    void UnlockStage(int stage);
    bool IsStageUnlocked(int stage) const;
};

#endif // PROGRESSMANAGER_H
