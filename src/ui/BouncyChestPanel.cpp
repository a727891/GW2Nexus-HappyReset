#include "ui/BouncyChestPanel.h"

#include "core/Types.h"
#include "ui/CompassLayout.h"
#include "ui/DrawUtils.h"
#include "ui/TextureCatalog.h"

#include <imgui.h>

namespace hr {

namespace {

constexpr float kChestSize = 64.0f;
constexpr float kShineScale = 1.5f;
constexpr float kShineHalf = kChestSize * kShineScale * 0.5f;

bool HitTestChest(const ImVec2& pos, const ImVec2& mouse) {
    return mouse.x >= pos.x && mouse.x <= pos.x + kChestSize && mouse.y >= pos.y &&
           mouse.y <= pos.y + kChestSize;
}

}  // namespace

bool BouncyChestPanel::ShouldShow(const AppState& state,
                                  const Mumble::Data* mumble,
                                  const NexusLinkData_t* nexus) {
    if (!state.IsChestDisplayed()) return false;

    if (state.settings.showChestForConfiguration) {
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

    const MinimapLayout layout =
        ComputeMinimapLayout(state.settings.chestLocation, state.settings.showChestForConfiguration,
                             mumble, nexus);
    if (!layout.valid) return;

    const ImVec2 pos{layout.chestPos.x, layout.chestPos.y};
    const ImVec2 center{pos.x + kChestSize * 0.5f, pos.y + kChestSize * 0.5f};

    ImDrawList* draw = ImGui::GetForegroundDrawList();
    const ImVec2 mouse = ImGui::GetIO().MousePos;
    const bool hovered = HitTestChest(pos, mouse);
    const ImTextureID shine = textures.Shine();
    const ImTextureID chestTex =
        (hovered || state.chestOpen) ? textures.ChestOpened() : textures.ChestClosed();

    if (state.settings.shouldShine && shine) {
        const float angle = static_cast<float>(ImGui::GetTime()) * -1.3f;
        DrawUtils::DrawRotatedImage(draw, shine, center, {kShineHalf, kShineHalf}, angle,
                                    IM_COL32(255, 255, 255, 204));
    }

    const ImVec2 chestHalf{kChestSize * 0.5f, kChestSize * 0.5f};
    const float chestAngle = (hovered || state.chestOpen) ? 0.0f : state.chestRotation;
    DrawUtils::DrawRotatedImage(draw, chestTex, center, chestHalf, chestAngle);

    const bool clicked = ImGui::GetIO().MouseClicked[0] || ImGui::GetIO().MouseClicked[1];
    if (clicked && hovered) {
        state.chestOpen = true;
        if (state.onChestClicked) {
            state.onChestClicked();
        }
    }
}

}  // namespace hr
