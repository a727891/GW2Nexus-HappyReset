#pragma once

#include <cstdint>

namespace hr {

// Nexus signature 0x60018004 (HappyReset, index 04)
inline constexpr int32_t kSignature = 0x60018004;
inline constexpr const char* kDisplayName = "Happy Reset";
inline constexpr const char* kLogTag = "HappyReset";
inline constexpr const char* kAuthor = "Soeed.4160";
inline constexpr const char* kDescription =
    "Bring back the daily login chest to open the Wizard's Vault.";
inline constexpr const char* kPatchNotesUrl = "https://github.com/a727891/BlishHud-HappyReset";
inline constexpr const char* kUpdateLink = "https://github.com/a727891/GW2Nexus-HappyReset";

// Set true to fire a "daily reset" every minute (for local testing).
inline constexpr bool kDebugMinuteResets = false;

}  // namespace hr
