#include "core/AppState.h"

#include "ui/DrawUtils.h"
#include "ui/TextureCatalog.h"

#include <algorithm>
#include <cmath>
#include <filesystem>

namespace hr {

namespace {

constexpr float kWiggleDelay = 1.4f;
constexpr float kWiggleDuration = 1.0f;
constexpr int kWiggleSteps = 15;
constexpr float kWiggleRotation = -0.78539816339f / 4.0f;  // -PiOver4 / 4

float ReflectPhase(float stepT) {
    stepT = std::clamp(stepT, 0.0f, 1.0f);
    if (stepT < 0.5f) return stepT * 2.0f;
    return (1.0f - stepT) * 2.0f;
}

}  // namespace

AppState& AppState::Instance() {
    static AppState state;
    return state;
}

std::string AppState::PersistencePath() const {
    return (std::filesystem::path(dataDir) / ResetPersistence::kFilename).string();
}

void AppState::Initialize(AddonAPI_t* apiPtr) {
    api = apiPtr;
    nexusLink = static_cast<NexusLinkData_t*>(api->DataLink_Get(DL_NEXUS_LINK));
    mumbleLink = static_cast<Mumble::Data*>(api->DataLink_Get(DL_MUMBLE_LINK));

    addonDir = api->Paths_GetAddonDirectory("NexusHappyReset");
    dataDir = (std::filesystem::path(addonDir) / "happyReset").string();
    std::filesystem::create_directories(dataDir);

    const auto settingsPath = (std::filesystem::path(addonDir) / "settings.json").string();
    settings.Load(settingsPath);
    persistence.Load(PersistencePath());

    TextureCatalog::Instance().Initialize(api);

    trackedDailyReset_ = resets.LastDailyReset();
    resets.SetDailyResetHandler([this](std::chrono::system_clock::time_point) { ShowChest(); });

    EvaluateChestVisibility();

    if (api) {
        api->Log(LOGL_INFO, "NexusHappyReset", "Initialized.");
    }
}

void AppState::Shutdown() {
    const auto settingsPath = (std::filesystem::path(addonDir) / "settings.json").string();
    settings.Save(settingsPath);
    persistence.Save(PersistencePath());
    TextureCatalog::Instance().Shutdown();
}

void AppState::EvaluateChestVisibility() {
    const auto lastSave = persistence.GetLastSave(accountName);
    if (lastSave < resets.LastDailyReset()) {
        ShowChest();
    } else {
        HideChest(false);
    }
}

void AppState::ShowChest() {
    chestVisible = true;
    chestOpen = false;
    wiggleInDelay_ = true;
    wiggleDelayTimer_ = 0.0f;
    wiggleStepTimer_ = 0.0f;
    wiggleStep_ = 0;
    wiggleDirection = 1;
    chestRotation = 0.0f;
}

void AppState::HideChest(bool save) {
    chestVisible = false;
    chestOpen = false;
    chestRotation = 0.0f;
    if (save) {
        persistence.SaveClear(accountName, PersistencePath());
    }
}

void AppState::Tick(float deltaSeconds) {
    resets.Update();

    if (resets.LastDailyReset() != trackedDailyReset_) {
        trackedDailyReset_ = resets.LastDailyReset();
        EvaluateChestVisibility();
    }

    if (!IsChestDisplayed()) return;

    if (!settings.wiggleChest || chestOpen) {
        chestRotation = 0.0f;
        return;
    }

    const float stepDuration = kWiggleDuration / static_cast<float>(kWiggleSteps);

    if (wiggleInDelay_) {
        wiggleDelayTimer_ += deltaSeconds;
        chestRotation = 0.0f;
        if (wiggleDelayTimer_ >= kWiggleDelay) {
            wiggleInDelay_ = false;
            wiggleDelayTimer_ = 0.0f;
            wiggleStepTimer_ = 0.0f;
            wiggleStep_ = 0;
            wiggleDirection = 1;
        }
        return;
    }

    wiggleStepTimer_ += deltaSeconds;
    while (wiggleStepTimer_ >= stepDuration && wiggleStep_ < kWiggleSteps) {
        wiggleStepTimer_ -= stepDuration;
        ++wiggleStep_;
        wiggleDirection *= -1;
    }

    if (wiggleStep_ >= kWiggleSteps) {
        wiggleInDelay_ = true;
        wiggleDelayTimer_ = 0.0f;
        wiggleStep_ = 0;
        wiggleStepTimer_ = 0.0f;
        chestRotation = 0.0f;
        return;
    }

    const float stepT = wiggleStepTimer_ / stepDuration;
    const float eased = DrawUtils::EaseBounceInOut(ReflectPhase(stepT));
    chestRotation = eased * kWiggleRotation * static_cast<float>(wiggleDirection);
}

}  // namespace hr
