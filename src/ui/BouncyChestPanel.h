#pragma once

#include "core/AppState.h"

namespace hr {

class BouncyChestPanel {
public:
    static bool ShouldShow(const AppState& state,
                           const Mumble::Data* mumble,
                           const NexusLinkData_t* nexus);
    static void Render(AppState& state,
                       const Mumble::Data* mumble,
                       const NexusLinkData_t* nexus);
};

}  // namespace hr
