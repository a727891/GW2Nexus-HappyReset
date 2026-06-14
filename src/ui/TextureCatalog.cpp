#include "ui/TextureCatalog.h"

#include "EmbeddedTextures.h"

namespace hr {

TextureCatalog& TextureCatalog::Instance() {
    static TextureCatalog catalog;
    return catalog;
}

void TextureCatalog::Initialize(AddonAPI_t* api) {
    api_ = api;
    if (!api_ || !api_->Textures_GetOrCreateFromMemory) return;

    const auto load = [this](const char* assetId, const char* identifier) -> ImTextureID {
        const auto* asset = EmbeddedTextures::Find(assetId);
        if (!asset) return nullptr;
        Texture_t* texture = api_->Textures_GetOrCreateFromMemory(
            identifier, const_cast<unsigned char*>(asset->data),
            static_cast<uint32_t>(asset->size));
        if (!texture || !texture->Resource) return nullptr;
        return reinterpret_cast<ImTextureID>(texture->Resource);
    };

    chestClosed_ = load("925822", "HR_TEX_925822");
    chestOpened_ = load("925823", "HR_TEX_925823");
    shine_ = load("965696", "HR_TEX_965696");
}

void TextureCatalog::Shutdown() {
    chestClosed_ = nullptr;
    chestOpened_ = nullptr;
    shine_ = nullptr;
    api_ = nullptr;
}

}  // namespace hr
