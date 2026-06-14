#include "ui/BouncyChestPanel.h"

#include "core/Types.h"
#include "ui/CompassLayout.h"
#include "ui/DrawUtils.h"
#include "ui/TextureCatalog.h"

#include <cmath>
#include <imgui.h>

namespace hr {

namespace {

constexpr float kChestSize = 64.0f;

ImVec2 CalcChestScreenPos(const AppState& state,
                          const ScreenContext& screen,
                          const ImRectLike& mapBounds) {
    const float w = kChestSize;
    const float h = kChestSize;

    switch (state.settings.chestLocation) {
        case ChestPosition::MinimapTopRight:
            return {mapBounds.x + mapBounds.w - 20.0f - w, mapBounds.y - h};
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
            return {mapBounds.x, mapBounds.y - h};
    }
}

}  // namespace

bool BouncyChestPanel::ShouldShow(const AppState& state,
                                  const Mumble::Data* mumble,
                                  const NexusLinkData_t* nexus) {
    if (!state.IsChestDisplayed()) return false;

    if (state.previewChestForConfiguration) {
        return nexus != nullptr && nexus->Width > 0 && nexus->Height > 0;
    }

    if (!state.chestVisible) return false;
    if (!nexus || !nexus->IsGameplay) return false;
    if (!mumble) return false;
    if (Mumble::IsMapOpen(mumble)) return false;
    return true;
}

void BouncyChestPanel::Render(AppState& state,
                              const Mumble::Data* mumble,
                              const NexusLinkData_t* nexus) {
    if (!ShouldShow(state, mumble, nexus)) return;

    const auto& textures = TextureCatalog::Instance();
    if (!textures.Ready()) return;

    const ScreenContext screen = BuildScreenContext(mumble, nexus);
    const ImRectLike mapBounds = MapBounds(screen, state.previewChestForConfiguration);
    const ImVec2 pos = CalcChestScreenPos(state, screen, mapBounds);

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize({kChestSize, kChestSize}, ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::Begin("##HR_BouncyChest",
                 nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav |
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                     ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);

    const ImVec2 cursor = ImGui::GetCursorScreenPos();
    const ImVec2 center{cursor.x + kChestSize * 0.5f, cursor.y + kChestSize * 0.5f};
    ImDrawList* draw = ImGui::GetWindowDrawList();

    const bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    const ImTextureID shine = textures.Shine();
    const ImTextureID chestTex =
        (hovered || state.chestOpen) ? textures.ChestOpened() : textures.ChestClosed();

    if (state.settings.shouldShine && shine) {
        const float angle = static_cast<float>(ImGui::GetTime()) * -1.3f;
        const ImVec2 shineHalf{48.0f, 48.0f};  // 1.5x the 64px chest bounds
        DrawUtils::DrawRotatedImage(draw, shine, center, shineHalf, angle,
                                    IM_COL32(255, 255, 255, 204));
    }

    const ImVec2 chestHalf{kChestSize * 0.5f, kChestSize * 0.5f};
    const float chestAngle = (hovered || state.chestOpen) ? 0.0f : state.chestRotation;
    DrawUtils::DrawRotatedImage(draw, chestTex, center, chestHalf, chestAngle);

    const bool preview = state.previewChestForConfiguration;
    if (ImGui::InvisibleButton("chest", ImVec2(kChestSize, kChestSize))) {
        if (!preview) {
            state.chestOpen = true;
            if (state.onChestClicked) {
                state.onChestClicked();
            }
        }
    }
    if (!preview && ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
        state.chestOpen = true;
        if (state.onChestClicked) {
            state.onChestClicked();
        }
    }

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

}  // namespace hr
