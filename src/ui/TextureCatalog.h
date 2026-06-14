#pragma once

#include <imgui.h>
#include "nexus/Nexus.h"

namespace hr {

class TextureCatalog {
public:
    static TextureCatalog& Instance();

    void Initialize(AddonAPI_t* api);
    void Shutdown();

    ImTextureID ChestClosed() const { return chestClosed_; }
    ImTextureID ChestOpened() const { return chestOpened_; }
    ImTextureID Shine() const { return shine_; }

    bool Ready() const { return chestClosed_ != nullptr; }

private:
    TextureCatalog() = default;

    AddonAPI_t* api_ = nullptr;
    ImTextureID chestClosed_ = nullptr;
    ImTextureID chestOpened_ = nullptr;
    ImTextureID shine_ = nullptr;
};

}  // namespace hr
