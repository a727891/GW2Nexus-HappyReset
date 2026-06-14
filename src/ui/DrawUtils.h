#pragma once

#include <imgui.h>

namespace hr {
namespace DrawUtils {

void DrawRotatedImage(ImDrawList* draw,
                      ImTextureID texture,
                      ImVec2 center,
                      ImVec2 halfSize,
                      float angleRadians,
                      ImU32 color = IM_COL32_WHITE);

void DrawRotatedImageAdditive(ImDrawList* draw,
                              ImTextureID texture,
                              ImVec2 center,
                              ImVec2 halfSize,
                              float angleRadians,
                              ImU32 color = IM_COL32_WHITE);

float EaseBounceInOut(float t);

}  // namespace DrawUtils
}  // namespace hr
