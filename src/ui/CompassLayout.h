#pragma once

#include "core/Types.h"
#include "mumble/Mumble.h"
#include "nexus/Nexus.h"

namespace hr {

struct ScreenContext {
    float width = 1920.0f;
    float height = 1080.0f;
    int uisz = 1;
    float uiScale = 1.0f;
    float screenScale = 1.0f;
    float mapScale = 1.0f;
    uint32_t mumbleTick = 0;
    uint16_t rawCompassWidth = 0;
    uint16_t rawCompassHeight = 0;
    float compassWidth = 0.0f;
    float compassHeight = 0.0f;
    bool hasMumbleCompass = false;
    bool mapOpen = false;
    bool compassTopRight = false;
    bool compassRotationEnabled = false;
    float compassRotation = 0.0f;
};

struct ScreenPoint {
    float x = 0.0f;
    float y = 0.0f;
};

struct MinimapLayout {
    ImRectLike mapBounds{};
    ScreenPoint chestPos{};
    uint16_t rawCompassWidth = 0;
    uint16_t rawCompassHeight = 0;
    uint32_t mumbleTick = 0;
    float screenScale = 1.0f;
    bool fromMumble = false;
    bool valid = false;
};

ScreenContext BuildScreenContext(const Mumble::Data* mumble, const NexusLinkData_t* nexus);
ImRectLike MapBounds(const ScreenContext& ctx);
ImRectLike MapBounds(const ScreenContext& ctx, bool layoutPreview);
ScreenPoint ChestScreenPos(ChestPosition location, const ScreenContext& screen, const ImRectLike& mapBounds);
MinimapLayout ComputeMinimapLayout(ChestPosition location,
                                   bool layoutPreview,
                                   const Mumble::Data* mumble,
                                   const NexusLinkData_t* nexus);

}  // namespace hr
