#include "ui/OptionsPanel.h"

#include "core/AppState.h"
#include "core/Branding.h"
#include "core/Types.h"

#include <imgui.h>

namespace hr {

void OptionsPanel::Render(AppState& state) {
    ImGui::TextUnformatted(kDisplayName);
    ImGui::Separator();
    ImGui::TextWrapped(
        "Click the daily chest to open the Wizard's Vault (in-game keybind is used via Nexus "
        "GameBinds).");
    ImGui::Spacing();

    ImGui::TextUnformatted("Configuration preview");
    if (ImGui::Checkbox("Show chest for configuration", &state.settings.showChestForConfiguration)) {
        state.chestOpen = false;
        state.SaveSettings();
    }
    if (state.settings.showChestForConfiguration) {
        ImGui::TextWrapped(
            "Preview is on: the chest stays visible so you can adjust location and display "
            "options. Clicking the chest opens the Wizard's Vault but does not change daily-reset "
            "progress.");
    } else if (state.chestVisible) {
        ImGui::TextWrapped(
            "The daily-reset chest is still active. Preview only controls the configuration "
            "overlay.");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextUnformatted("Display Options");
    int location = static_cast<int>(state.settings.chestLocation);
    if (ImGui::Combo("Location", &location,
                     "Above the minimap - Top Left\0Above the minimap - Top Right\0"
                     "Inside the minimap - Top Left\0Inside the minimap - Top Right\0"
                     "Inside the minimap - Bottom Left\0Inside the minimap - Bottom Right\0"
                     "Bottom Right of the screen\0")) {
        state.settings.chestLocation = static_cast<ChestPosition>(location);
        state.SaveSettings();
    }

    if (ImGui::Checkbox("Wiggle and bounce", &state.settings.wiggleChest)) {
        state.SaveSettings();
    }
    if (ImGui::Checkbox("Shiny background", &state.settings.shouldShine)) {
        state.SaveSettings();
    }
}

}  // namespace hr
