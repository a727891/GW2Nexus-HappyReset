#pragma once

#include "core/Types.h"
#include "mumble/Mumble.h"
#include "nexus/Nexus.h"

namespace hr {

struct ScreenContext {
    float width = 1920.0f;
    float height = 1080.0f;
    float mapScale = 1.0f;
    uint16_t compassWidth = 0;
    uint16_t compassHeight = 0;
    bool mapOpen = false;
    bool compassTopRight = false;
    bool compassRotationEnabled = false;
    float compassRotation = 0.0f;
};

ScreenContext BuildScreenContext(const Mumble::Data* mumble, const NexusLinkData_t* nexus);
ImRectLike MapBounds(const ScreenContext& ctx);
ImRectLike MapBounds(const ScreenContext& ctx, bool layoutPreview);

}  // namespace hr
