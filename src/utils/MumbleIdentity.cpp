#include "utils/MumbleIdentity.h"

#include <nlohmann/json.hpp>
#include <windows.h>

namespace hr {
namespace MumbleIdentity {

namespace {

std::string WideToUtf8(const wchar_t* wide) {
    if (!wide || wide[0] == L'\0') return {};

    const int size = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
    if (size <= 0) return {};

    std::string result(static_cast<size_t>(size - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide, -1, result.data(), size, nullptr, nullptr);
    return result;
}

int UiszFromJson(const nlohmann::json& j) {
    if (j.contains("uisz")) {
        return j["uisz"].get<int>();
    }
    return 1;
}

}  // namespace

float ParseUiScale(const Mumble::Data* data) {
    switch (ParseUisz(data)) {
        case 0:
            return 0.9f;
        case 2:
            return 1.111f;
        case 3:
            return 1.224f;
        default:
            return 1.0f;
    }
}

int ParseUisz(const Mumble::Data* data) {
    if (!data) return 1;

    const std::string identity = WideToUtf8(data->Identity);
    if (identity.empty()) return 1;

    try {
        return UiszFromJson(nlohmann::json::parse(identity));
    } catch (...) {
        return 1;
    }
}

}  // namespace MumbleIdentity
}  // namespace hr
