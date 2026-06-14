#pragma once

#include <chrono>
#include <string>
#include <unordered_map>

namespace hr {

class ResetPersistence {
public:
    static constexpr const char* kFilename = "resets.json";
    static constexpr const char* kVersion = "1.0.0";

    std::string version = kVersion;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> lastKnownReset;

    void Load(const std::string& path);
    void Save(const std::string& path) const;

    std::chrono::system_clock::time_point GetLastSave(const std::string& account) const;
    void SaveClear(const std::string& account, const std::string& path);
};

}  // namespace hr
