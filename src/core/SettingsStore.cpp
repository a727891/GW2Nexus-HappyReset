#include "core/SettingsStore.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace hr {

namespace {

ChestPosition ChestFromString(const std::string& value) {
    if (value == "MinimapTopRight") return ChestPosition::MinimapTopRight;
    if (value == "MinimapInsideTopLeft") return ChestPosition::MinimapInsideTopLeft;
    if (value == "MinimapInsideTopRight") return ChestPosition::MinimapInsideTopRight;
    if (value == "MinimapInsideBottomLeft") return ChestPosition::MinimapInsideBottomLeft;
    if (value == "MinimapInsideBottomRight") return ChestPosition::MinimapInsideBottomRight;
    if (value == "ScreenBottomRight") return ChestPosition::ScreenBottomRight;
    return ChestPosition::MinimapTopLeft;
}

const char* ChestToString(ChestPosition pos) {
    switch (pos) {
        case ChestPosition::MinimapTopRight:
            return "MinimapTopRight";
        case ChestPosition::MinimapInsideTopLeft:
            return "MinimapInsideTopLeft";
        case ChestPosition::MinimapInsideTopRight:
            return "MinimapInsideTopRight";
        case ChestPosition::MinimapInsideBottomLeft:
            return "MinimapInsideBottomLeft";
        case ChestPosition::MinimapInsideBottomRight:
            return "MinimapInsideBottomRight";
        case ChestPosition::ScreenBottomRight:
            return "ScreenBottomRight";
        default:
            return "MinimapTopLeft";
    }
}

}  // namespace

void SettingsStore::Load(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in.is_open()) return;

    nlohmann::json j;
    in >> j;

    if (j.contains("wiggleChest")) wiggleChest = j["wiggleChest"].get<bool>();
    if (j.contains("shouldShine")) shouldShine = j["shouldShine"].get<bool>();
    if (j.contains("chestLocation")) {
        chestLocation = ChestFromString(j["chestLocation"].get<std::string>());
    }
}

void SettingsStore::Save(const std::string& filePath) const {
    nlohmann::json j = {
        {"wiggleChest", wiggleChest},
        {"shouldShine", shouldShine},
        {"chestLocation", ChestToString(chestLocation)},
    };

    std::filesystem::path filePathObj(filePath);
    if (filePathObj.has_parent_path()) {
        std::filesystem::create_directories(filePathObj.parent_path());
    }

    std::ofstream out(filePath);
    out << j.dump(2);
}

}  // namespace hr
