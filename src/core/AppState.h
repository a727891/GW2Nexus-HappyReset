#pragma once

#include "core/SettingsStore.h"
#include "services/ResetPersistence.h"
#include "services/ResetsWatcher.h"

#include "mumble/Mumble.h"
#include "nexus/Nexus.h"

#include <chrono>
#include <functional>
#include <string>

namespace hr {

class AppState {
public:
    static AppState& Instance();

    AddonAPI_t* api = nullptr;
    NexusLinkData_t* nexusLink = nullptr;
    Mumble::Data* mumbleLink = nullptr;

    std::string addonDir;
    std::string dataDir;
    std::string accountName = "default";

    SettingsStore settings;
    ResetPersistence persistence;
    ResetsWatcher resets;

    bool chestVisible = false;
    bool chestOpen = false;
    float chestRotation = 0.0f;
    int wiggleDirection = 1;

    std::function<void()> onChestClicked;

    void Initialize(AddonAPI_t* apiPtr);
    void Shutdown();
    void Tick(float deltaSeconds);

    std::string PersistencePath() const;
    void EvaluateChestVisibility();
    void ShowChest();
    void HideChest(bool save);

    bool IsChestDisplayed() const {
        return chestVisible || settings.showChestForConfiguration;
    }

private:
    AppState() = default;

    std::chrono::system_clock::time_point trackedDailyReset_;
    bool wiggleInDelay_ = true;
    float wiggleDelayTimer_ = 0.0f;
    float wiggleStepTimer_ = 0.0f;
    int wiggleStep_ = 0;
};

}  // namespace hr
