#include "ui/CompassLayout.h"

#include "utils/MumbleIdentity.h"

namespace hr {

namespace {

constexpr float kChestSize = 64.0f;

float BottomSeparation(int uisz, float screenScale) {
    int delta = 37;
    switch (uisz) {
        case 0:
            delta = 33;
            break;
        case 2:
            delta = 41;
            break;
        case 3:
            delta = 45;
            break;
        default:
            delta = 37;
            break;
    }
    return static_cast<float>(delta) * screenScale;
}

ImRectLike DefaultPreviewMapBounds(const ScreenContext& ctx) {
    const float compassW = 200.0f * ctx.screenScale;
    const float compassH = 200.0f * ctx.screenScale;
    ImRectLike rect{};
    rect.w = compassW;
    rect.h = compassH;
    rect.x = ctx.width - compassW;
    rect.y = ctx.height - compassH - BottomSeparation(ctx.uisz, ctx.screenScale);
    return rect;
}

ImRectLike MinimapRect(float screenW,
                       float screenH,
                       float compassW,
                       float compassH,
                       float separation,
                       bool compassTopRight) {
    ImRectLike rect{};
    rect.w = compassW;
    rect.h = compassH;
    rect.x = screenW - compassW;
    if (compassTopRight) {
        rect.y = 0.0f;
    } else {
        rect.y = screenH - compassH - separation;
    }
    return rect;
}

}  // namespace

ScreenContext BuildScreenContext(const Mumble::Data* mumble, const NexusLinkData_t* nexus) {
    ScreenContext ctx;
    if (nexus && nexus->Width > 0 && nexus->Height > 0) {
        ctx.width = static_cast<float>(nexus->Width);
        ctx.height = static_cast<float>(nexus->Height);
    }

    if (!mumble) {
        return ctx;
    }

    ctx.mumbleTick = mumble->UITick;
    ctx.uisz = MumbleIdentity::ParseUisz(mumble);
    ctx.uiScale = MumbleIdentity::ParseUiScale(mumble);
    if (nexus && nexus->Scaling > 0.01f) {
        ctx.screenScale = nexus->Scaling;
    } else {
        ctx.screenScale = ctx.uiScale;
    }

    ctx.rawCompassWidth = mumble->Context.Compass.Width;
    ctx.rawCompassHeight = mumble->Context.Compass.Height;
    ctx.compassWidth = static_cast<float>(mumble->Context.Compass.Width) * ctx.screenScale;
    ctx.compassHeight = static_cast<float>(mumble->Context.Compass.Height) * ctx.screenScale;
    ctx.compassRotation = mumble->Context.Compass.Rotation;
    ctx.mapScale = mumble->Context.Compass.Scale > 0.0f ? mumble->Context.Compass.Scale : 1.0f;
    ctx.mapOpen = mumble->Context.IsMapOpen != 0;
    ctx.compassTopRight = mumble->Context.IsCompassTopRight != 0;
    ctx.compassRotationEnabled = mumble->Context.IsCompassRotating != 0;
    ctx.hasMumbleCompass = ctx.rawCompassWidth > 0 && ctx.rawCompassHeight > 0;
    return ctx;
}

ImRectLike MapBounds(const ScreenContext& ctx) {
    return MapBounds(ctx, false);
}

ImRectLike MapBounds(const ScreenContext& ctx, bool layoutPreview) {
    if (ctx.compassWidth < 1.0f || ctx.compassHeight < 1.0f) {
        if (layoutPreview && ctx.width > 0.0f && ctx.height > 0.0f) {
            return DefaultPreviewMapBounds(ctx);
        }
        return {};
    }

    if (ctx.mapOpen) {
        return {0.0f, 0.0f, ctx.width, ctx.height};
    }

    return MinimapRect(ctx.width, ctx.height, ctx.compassWidth, ctx.compassHeight,
                       BottomSeparation(ctx.uisz, ctx.screenScale), ctx.compassTopRight);
}

ScreenPoint ChestScreenPos(ChestPosition location,
                           const ScreenContext& screen,
                           const ImRectLike& mapBounds) {
    const float w = kChestSize;
    const float h = kChestSize;

    auto aboveTop = [&](float x, float yTop) -> ScreenPoint {
        return {x, yTop - h};
    };

    switch (location) {
        case ChestPosition::MinimapTopRight:
            return aboveTop(mapBounds.x + mapBounds.w - 20.0f - w, mapBounds.y);
        case ChestPosition::ScreenBottomRight:
            return {screen.width - w - 20.0f, screen.height - h - 20.0f};
        case ChestPosition::MinimapInsideTopLeft:
            return {mapBounds.x, mapBounds.y};
        case ChestPosition::MinimapInsideTopRight:
            return {mapBounds.x + mapBounds.w - w, mapBounds.y};
        case ChestPosition::MinimapInsideBottomLeft:
            return {mapBounds.x, mapBounds.y + mapBounds.h - h};
        case ChestPosition::MinimapInsideBottomRight:
            return {mapBounds.x + mapBounds.w - w, mapBounds.y + mapBounds.h - h};
        case ChestPosition::MinimapTopLeft:
        default:
            return aboveTop(mapBounds.x, mapBounds.y);
    }
}

MinimapLayout ComputeMinimapLayout(ChestPosition location,
                                   bool layoutPreview,
                                   const Mumble::Data* mumble,
                                   const NexusLinkData_t* nexus) {
    MinimapLayout layout{};
    if (!nexus || nexus->Width < 1 || nexus->Height < 1) {
        return layout;
    }

    const ScreenContext screen = BuildScreenContext(mumble, nexus);
    layout.rawCompassWidth = screen.rawCompassWidth;
    layout.rawCompassHeight = screen.rawCompassHeight;
    layout.mumbleTick = screen.mumbleTick;
    layout.screenScale = screen.screenScale;
    layout.fromMumble = screen.hasMumbleCompass;

    layout.mapBounds = MapBounds(screen, layoutPreview);
    if (layout.mapBounds.w < 1.0f || layout.mapBounds.h < 1.0f) {
        return layout;
    }

    layout.chestPos = ChestScreenPos(location, screen, layout.mapBounds);
    layout.valid = true;
    return layout;
}

}  // namespace hr
