#include "core/AppState.h"
#include "core/Branding.h"
#include "Version.h"
#include "ui/BouncyChestPanel.h"
#include "ui/OptionsPanel.h"
#include "utils/GameBindHelper.h"

#include <imgui.h>
#include "mumble/Mumble.h"
#include "nexus/Nexus.h"
#include <windows.h>

namespace {

AddonAPI_t* g_api = nullptr;
float g_lastFrameTime = 0.0f;

void AddonLoad(AddonAPI_t* api);
void AddonUnload();
void AddonRender();
void AddonOptions();

void AddonLoad(AddonAPI_t* api) {
    g_api = api;
    api->Log(LOGL_INFO, hr::kLogTag, "AddonLoad starting.");

    ImGui::SetCurrentContext(static_cast<ImGuiContext*>(api->ImguiContext));
    ImGui::SetAllocatorFunctions(
        reinterpret_cast<void* (*)(size_t, void*)>(api->ImguiMalloc),
        reinterpret_cast<void (*)(void*, void*)>(api->ImguiFree));

    auto& state = hr::AppState::Instance();
    state.Initialize(api);

    state.onChestClicked = [&state]() {
        hr::PressWizardsVault(g_api);
        if (!state.settings.showChestForConfiguration) {
            state.HideChest(true);
        }
    };

    api->GUI_Register(RT_Render, AddonRender);
    api->GUI_Register(RT_OptionsRender, AddonOptions);

    api->Log(LOGL_INFO, hr::kLogTag, "Loaded.");
}

void AddonUnload() {
    if (!g_api) return;
    g_api->GUI_Deregister(AddonRender);
    g_api->GUI_Deregister(AddonOptions);
    hr::AppState::Instance().Shutdown();
    g_api->Log(LOGL_INFO, hr::kLogTag, "Unloaded.");
    g_api = nullptr;
}

void AddonRender() {
    auto& state = hr::AppState::Instance();
    const float now = static_cast<float>(ImGui::GetTime());
    const float delta = g_lastFrameTime > 0.0f ? now - g_lastFrameTime : 0.0f;
    g_lastFrameTime = now;

    state.Tick(delta);
    hr::BouncyChestPanel::Render(state, state.mumbleLink, state.nexusLink);
}

void AddonOptions() { hr::OptionsPanel::Render(hr::AppState::Instance()); }

}  // namespace

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) { return TRUE; }

extern "C" __declspec(dllexport) AddonDefinition_t* GetAddonDef() {
    static AddonDefinition_t def{};
    static bool initialized = false;
    if (!initialized) {
        def.Signature = hr::kSignature;
        def.APIVersion = NEXUS_API_VERSION;
        def.Name = hr::kDisplayName;
        def.Version = {V_MAJOR, V_MINOR, V_BUILD, V_REVISION};
        def.Author = "Soeed";
        def.Description = hr::kDescription;
        def.Load = AddonLoad;
        def.Unload = AddonUnload;
        def.Flags = AF_None;
        def.Provider = UP_None;
        initialized = true;
    }
    return &def;
}
