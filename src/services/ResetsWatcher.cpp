#include "services/ResetsWatcher.h"

#include "core/Branding.h"

#include <ctime>

namespace hr {

namespace {

std::chrono::system_clock::time_point UtcFromTm(std::tm tm) {
    tm.tm_isdst = 0;
#ifdef _WIN32
    const std::time_t tt = _mkgmtime(&tm);
#else
    const std::time_t tt = timegm(&tm);
#endif
    return std::chrono::system_clock::from_time_t(tt);
}

std::chrono::system_clock::time_point NowUtc() {
    return std::chrono::system_clock::now();
}

}  // namespace

ResetsWatcher::ResetsWatcher() { CalcNextDailyReset(); }

void ResetsWatcher::CalcNextDailyReset() {
    const auto now = NowUtc();

    if constexpr (kDebugMinuteResets) {
        const auto epoch = now.time_since_epoch();
        const auto wholeMinutes =
            std::chrono::duration_cast<std::chrono::minutes>(epoch);
        nextDailyReset_ =
            std::chrono::system_clock::time_point(wholeMinutes + std::chrono::minutes(1));
        lastDailyReset_ = nextDailyReset_ - std::chrono::minutes(1);
        return;
    }

    const std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm utc{};
#ifdef _WIN32
    gmtime_s(&utc, &tt);
#else
    gmtime_r(&tt, &utc);
#endif

    std::tm next = utc;
    next.tm_mday += 1;
    next.tm_hour = 0;
    next.tm_min = 0;
    next.tm_sec = 0;
    nextDailyReset_ = UtcFromTm(next);
    lastDailyReset_ = nextDailyReset_ - std::chrono::hours(24);
}

void ResetsWatcher::Update() {
    const auto now = NowUtc();
    if (now >= nextDailyReset_) {
        CalcNextDailyReset();
        if (dailyResetHandler_) {
            dailyResetHandler_(nextDailyReset_);
        }
    }
}

}  // namespace hr
