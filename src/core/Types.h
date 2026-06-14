#pragma once

namespace hr {

enum class ChestPosition {
    MinimapTopLeft,
    MinimapTopRight,
    MinimapInsideTopLeft,
    MinimapInsideTopRight,
    MinimapInsideBottomLeft,
    MinimapInsideBottomRight,
    ScreenBottomRight,
};

struct ImRectLike {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
};

inline const char* ChestPositionLabel(ChestPosition pos) {
    switch (pos) {
        case ChestPosition::MinimapTopLeft:
            return "Above the minimap - Top Left";
        case ChestPosition::MinimapTopRight:
            return "Above the minimap - Top Right";
        case ChestPosition::MinimapInsideTopLeft:
            return "Inside the minimap - Top Left";
        case ChestPosition::MinimapInsideTopRight:
            return "Inside the minimap - Top Right";
        case ChestPosition::MinimapInsideBottomLeft:
            return "Inside the minimap - Bottom Left";
        case ChestPosition::MinimapInsideBottomRight:
            return "Inside the minimap - Bottom Right";
        case ChestPosition::ScreenBottomRight:
            return "Bottom Right of the screen";
    }
    return "Unknown";
}

}  // namespace hr
