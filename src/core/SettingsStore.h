#pragma once

#include "core/Types.h"

#include <string>

namespace hr {

struct SettingsStore {
    bool wiggleChest = true;
    bool shouldShine = true;
    ChestPosition chestLocation = ChestPosition::MinimapTopLeft;

    void Load(const std::string& filePath);
    void Save(const std::string& filePath) const;
};

}  // namespace hr
