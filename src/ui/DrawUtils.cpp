#include "ui/DrawUtils.h"

#include <algorithm>
#include <cmath>

namespace hr {
namespace DrawUtils {

void DrawRotatedImage(ImDrawList* draw,
                      ImTextureID texture,
                      ImVec2 center,
                      ImVec2 halfSize,
                      float angleRadians,
                      ImU32 color) {
    if (!draw || !texture) return;

    const float c = std::cos(angleRadians);
    const float s = std::sin(angleRadians);
    const auto corner = [&](float lx, float ly) -> ImVec2 {
        return {center.x + lx * c - ly * s, center.y + lx * s + ly * c};
    };

    const ImVec2 p1 = corner(-halfSize.x, -halfSize.y);
    const ImVec2 p2 = corner(halfSize.x, -halfSize.y);
    const ImVec2 p3 = corner(halfSize.x, halfSize.y);
    const ImVec2 p4 = corner(-halfSize.x, halfSize.y);
    draw->AddImageQuad(texture, p1, p2, p3, p4, ImVec2(0, 0), ImVec2(1, 0), ImVec2(1, 1),
                       ImVec2(0, 1), color);
}

float EaseBounceInOut(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    if (t < 0.5f) {
        return 0.5f * (1.0f - std::cos(t * 3.1415926535f));
    }
    return 0.5f + 0.5f * std::sin((t - 0.5f) * 3.1415926535f);
}

}  // namespace DrawUtils
}  // namespace hr
