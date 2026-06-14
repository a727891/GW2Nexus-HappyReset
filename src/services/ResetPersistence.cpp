#include "services/ResetPersistence.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace hr {

namespace {

std::string TimePointToIso(const std::chrono::system_clock::time_point& tp) {
    const std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm utc{};
#ifdef _WIN32
    gmtime_s(&utc, &tt);
#else
    gmtime_r(&tt, &utc);
#endif
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &utc);
    return buf;
}

std::chrono::system_clock::time_point ParseIso(const std::string& value) {
    if (value.empty()) return {};
    std::tm tm{};
    if (value.size() >= 19 && value[10] == 'T') {
        tm.tm_year = std::stoi(value.substr(0, 4)) - 1900;
        tm.tm_mon = std::stoi(value.substr(5, 2)) - 1;
        tm.tm_mday = std::stoi(value.substr(8, 2));
        tm.tm_hour = std::stoi(value.substr(11, 2));
        tm.tm_min = std::stoi(value.substr(14, 2));
        tm.tm_sec = std::stoi(value.substr(17, 2));
        tm.tm_isdst = 0;
#ifdef _WIN32
        const std::time_t tt = _mkgmtime(&tm);
#else
        const std::time_t tt = timegm(&tm);
#endif
        return std::chrono::system_clock::from_time_t(tt);
    }
    return {};
}

}  // namespace

void ResetPersistence::Load(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        lastKnownReset.clear();
        lastKnownReset["default"] = {};
        return;
    }

    nlohmann::json j;
    in >> j;
    if (j.contains("version")) version = j["version"].get<std::string>();
    lastKnownReset.clear();
    if (j.contains("lastKnownReset") && j["lastKnownReset"].is_object()) {
        for (auto it = j["lastKnownReset"].begin(); it != j["lastKnownReset"].end(); ++it) {
            lastKnownReset[it.key()] = ParseIso(it.value().get<std::string>());
        }
    }
}

void ResetPersistence::Save(const std::string& path) const {
    nlohmann::json resets = nlohmann::json::object();
    for (const auto& [account, tp] : lastKnownReset) {
        resets[account] = TimePointToIso(tp);
    }

    nlohmann::json j = {
        {"version", version},
        {"lastKnownReset", resets},
    };

    std::filesystem::path p(path);
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path());
    }

    std::ofstream out(path);
    out << j.dump(2);
}

std::chrono::system_clock::time_point ResetPersistence::GetLastSave(
    const std::string& account) const {
    const auto it = lastKnownReset.find(account);
    if (it == lastKnownReset.end()) return {};
    return it->second;
}

void ResetPersistence::SaveClear(const std::string& account, const std::string& path) {
    lastKnownReset[account] = std::chrono::system_clock::now();
    Save(path);
}

}  // namespace hr
