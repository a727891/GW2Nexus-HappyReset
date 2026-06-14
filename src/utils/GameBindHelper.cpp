#include "utils/GameBindHelper.h"

namespace hr {

void PressWizardsVault(AddonAPI_t* api) {
    if (!api) return;

    if (api->GameBinds_PressAsync && api->GameBinds_ReleaseAsync) {
        api->GameBinds_PressAsync(GB_UiSeasonalObjectivesShop);
        api->GameBinds_ReleaseAsync(GB_UiSeasonalObjectivesShop);
        return;
    }

    if (api->GameBinds_InvokeAsync) {
        api->GameBinds_InvokeAsync(GB_UiSeasonalObjectivesShop, 100);
    }
}

}  // namespace hr
