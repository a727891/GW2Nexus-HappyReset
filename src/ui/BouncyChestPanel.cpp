#include "ui/BouncyChestPanel.h"

#include "core/Types.h"
#include "ui/CompassLayout.h"
#include "ui/DrawUtils.h"
#include "ui/TextureCatalog.h"

#include <imgui.h>

namespace hr {

namespace {

constexpr float kChestSize = 64.0f;
constexpr float kChestHalf = kChestSize * 0.5f;
constexpr float kShineScale = 1.5f;
constexpr float kShineHalf = kChestSize * kShineScale * 0.5f;
constexpr float kRotatedHalfScale = 1.41421356237f;  // sqrt(2), worst-case rotated square
constexpr float kClipPad = 2.0f;

constexpr ImGuiWindowFlags kChestWindowFlags =
    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
    ImGuiWindowFlags_NoBringToFrontOnFocus;

float OverlayHalfSize(bool drawShine) {
    float half = kChestHalf;
    if (drawShine) {
        half = std::max(half, kShineHalf * kRotatedHalfScale);
    }
    return half + kClipPad;
}

bool HitTestChest(const ImVec2& center, const ImVec2& mouse) {
    return mouse.x >= center.x - kChestHalf && mouse.x <= center.x + kChestHalf &&
           mouse.y >= center.y - kChestHalf && mouse.y <= center.y + kChestHalf;
}

}  // namespace

bool BouncyChestPanel::ShouldShow(const AppState& state,
                                  const Mumble::Data* mumble,
                                  const NexusLinkData_t* nexus) {
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

    const MinimapLayout layout = ComputeMinimapLayout(
        state.settings.chestLocation, state.settings.showChestForConfiguration, mumble, nexus);
    if (!layout.valid) return;

    const ImVec2 pos{layout.chestPos.x, layout.chestPos.y};
    const ImVec2 center{pos.x + kChestHalf, pos.y + kChestHalf};
    const bool drawShine = state.settings.shouldShine && textures.Shine();
    const float overlayHalf = OverlayHalfSize(drawShine);
    const ImVec2 windowPos{center.x - overlayHalf, center.y - overlayHalf};
    const ImVec2 windowSize{overlayHalf * 2.0f, overlayHalf * 2.0f};

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));
    ImGui::Begin("##HappyResetChest", nullptr, kChestWindowFlags);

    ImDrawList* draw = ImGui::GetWindowDrawList();
    const ImVec2 mouse = ImGui::GetIO().MousePos;
    const bool chestHovered = HitTestChest(center, mouse);
    const ImTextureID shine = textures.Shine();
    const ImTextureID chestTex =
        (chestHovered || state.chestOpen) ? textures.ChestOpened() : textures.ChestClosed();

    if (drawShine) {
        const float angle = static_cast<float>(ImGui::GetTime()) * -1.3f;
        DrawUtils::DrawRotatedImageAdditive(draw, shine, center, {kShineHalf, kShineHalf}, angle,
                                            IM_COL32(255, 255, 255, 180));
    }

    const float chestAngle = (chestHovered || state.chestOpen) ? 0.0f : state.chestRotation;
    DrawUtils::DrawRotatedImage(draw, chestTex, center, {kChestHalf, kChestHalf}, chestAngle);

    ImGui::SetCursorPos({overlayHalf - kChestHalf, overlayHalf - kChestHalf});
    ImGui::InvisibleButton("##hit", {kChestSize, kChestSize});
    const bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left) ||
                         ImGui::IsItemClicked(ImGuiMouseButton_Right);
    if (clicked) {
        state.chestOpen = true;
        if (state.onChestClicked) {
            state.onChestClicked();
        }
    }

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

}  // namespace hr
