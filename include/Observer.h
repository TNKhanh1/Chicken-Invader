#ifndef OBSERVER_H
#define OBSERVER_H

#include <string>

// Enum để định nghĩa các loại Event phát sinh trong game
enum class EventType {
    PLAYER_TOOK_DAMAGE,
    PLAYER_HEALED,
    PLAYER_MANA_CHANGED,
    PLAYER_EXP_GAINED,
    PLAYER_LEVEL_UP,
    ENEMY_DIED,
    BOSS_SPAWNED
};

// Interface cho Observer (Người lắng nghe/nhận sự kiện)
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void OnNotify(EventType event, const std::string& data) = 0;
};

// Interface cho Subject (Người phát sự kiện)
class ISubject {
public:
    virtual ~ISubject() = default;
    virtual void AddObserver(IObserver* observer) = 0;
    virtual void RemoveObserver(IObserver* observer) = 0;
    virtual void Notify(EventType event, const std::string& data) = 0;
};

#endif // OBSERVER_H
