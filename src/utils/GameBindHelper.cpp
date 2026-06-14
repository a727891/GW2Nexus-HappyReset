#include "utils/GameBindHelper.h"

namespace hr {

void PressWizardsVault(AddonAPI_t* api) {
    if (!api || !api->GameBinds_InvokeAsync) return;
    api->GameBinds_InvokeAsync(GB_UiSeasonalObjectivesShop, 50);
}

}  // namespace hr
