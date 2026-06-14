#include "ui/CompassLayout.h"

#include "core/Types.h"

#include <cmath>

namespace hr {

namespace {

constexpr int kMinCompassWidth = 170;
constexpr int kMaxCompassWidth = 362;
constexpr int kMinCompassHeight = 170;
constexpr int kMaxCompassHeight = 338;
constexpr int kMinCompassOffset = 19;
constexpr int kMaxCompassOffset = 40;
constexpr int kCompassSeparation = 40;
constexpr double kBlishScale = 1.0 / 0.897;

int CompassOffset(int value, int minValue, int maxValue) {
    if (maxValue <= minValue) return kMinCompassOffset;
    const double t =
        (static_cast<double>(value) - minValue) / (static_cast<double>(maxValue - minValue));
    return static_cast<int>(std::lround(kMinCompassOffset +
                                        t * (kMaxCompassOffset - kMinCompassOffset)));
}

float ScaleMap(double mapScale) {
    if (mapScale <= 0.0) return static_cast<float>(kBlishScale);
    return static_cast<float>(kBlishScale / mapScale);
}

ImRectLike DefaultPreviewMapBounds(const ScreenContext& ctx) {
    const int offsetWidth = 28;
    const int offsetHeight = 28;
    const float compassW = 200.0f;
    const float compassH = 200.0f;
    ImRectLike rect{};
    rect.w = compassW + static_cast<float>(offsetWidth);
    rect.h = compassH + static_cast<float>(offsetHeight);
    rect.x = ctx.width - compassW - static_cast<float>(offsetWidth);
    rect.y = ctx.height - compassH - static_cast<float>(offsetHeight) -
             static_cast<float>(kCompassSeparation);
    return rect;
}

}  // namespace

ScreenContext BuildScreenContext(const Mumble::Data* mumble, const NexusLinkData_t* nexus) {
    ScreenContext ctx;
    if (nexus) {
        ctx.width = static_cast<float>(nexus->Width);
        ctx.height = static_cast<float>(nexus->Height);
    }

    if (!mumble || mumble->LinkedMemory.context_len < sizeof(Mumble::Context)) {
        return ctx;
    }

    const auto* raw = mumble->LinkedMemory.context;
    const auto* c = reinterpret_cast<const Mumble::Context*>(raw);
    ctx.compassWidth = c->compassWidth;
    ctx.compassHeight = c->compassHeight;
    ctx.compassRotation = c->compassRotation;
    ctx.mapScale = c->mapScale > 0.0f ? c->mapScale : 1.0f;
    ctx.mapOpen = Mumble::IsMapOpen(mumble);
    ctx.compassTopRight = (c->uiState & 2u) != 0;
    ctx.compassRotationEnabled = (c->uiState & 4u) != 0;
    (void)ctx.compassRotationEnabled;
    (void)ScaleMap(ctx.mapScale);
    return ctx;
}

ImRectLike MapBounds(const ScreenContext& ctx) {
    return MapBounds(ctx, false);
}

ImRectLike MapBounds(const ScreenContext& ctx, bool layoutPreview) {
    ImRectLike rect{};
    if (ctx.compassWidth < 1 || ctx.compassHeight < 1) {
        if (layoutPreview && ctx.width > 0 && ctx.height > 0) {
            return DefaultPreviewMapBounds(ctx);
        }
        return rect;
    }

    if (ctx.mapOpen) {
        rect.x = 0;
        rect.y = 0;
        rect.w = ctx.width;
        rect.h = ctx.height;
        return rect;
    }

    const int offsetWidth =
        CompassOffset(static_cast<int>(ctx.compassWidth), kMinCompassWidth, kMaxCompassWidth);
    const int offsetHeight =
        CompassOffset(static_cast<int>(ctx.compassHeight), kMinCompassHeight, kMaxCompassHeight);

    rect.w = static_cast<float>(ctx.compassWidth + offsetWidth);
    rect.h = static_cast<float>(ctx.compassHeight + offsetHeight);
    rect.x = ctx.width - static_cast<float>(ctx.compassWidth) -
             static_cast<float>(offsetWidth);
    if (ctx.compassTopRight) {
        rect.y = 0.0f;
    } else {
        rect.y = ctx.height - static_cast<float>(ctx.compassHeight) -
                 static_cast<float>(offsetHeight) - static_cast<float>(kCompassSeparation);
    }
    return rect;
}

}  // namespace hr
