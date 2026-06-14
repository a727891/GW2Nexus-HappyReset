#pragma once

#include <chrono>
#include <functional>

namespace hr {

class ResetsWatcher {
public:
    using DailyResetHandler = std::function<void(std::chrono::system_clock::time_point resetTime)>;

    ResetsWatcher();

    void Update();
    void SetDailyResetHandler(DailyResetHandler handler) { dailyResetHandler_ = std::move(handler); }

    std::chrono::system_clock::time_point LastDailyReset() const { return lastDailyReset_; }
    std::chrono::system_clock::time_point NextDailyReset() const { return nextDailyReset_; }

private:
    void CalcNextDailyReset();

    DailyResetHandler dailyResetHandler_;
    std::chrono::system_clock::time_point lastDailyReset_;
    std::chrono::system_clock::time_point nextDailyReset_;
};

}  // namespace hr
