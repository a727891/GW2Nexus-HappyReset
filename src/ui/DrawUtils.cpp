#include "ui/DrawUtils.h"

#include <algorithm>
#include <cmath>

#ifdef _WIN32
#include <d3d11.h>
#endif

namespace hr {
namespace DrawUtils {

namespace {

#ifdef _WIN32
struct AdditiveBlendState {
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    ID3D11BlendState* blendState = nullptr;
};

AdditiveBlendState g_additive;

void EnsureAdditiveBlendState(ImTextureID texture) {
    if (g_additive.blendState || !texture) return;

    auto* srv = reinterpret_cast<ID3D11ShaderResourceView*>(texture);
    ID3D11Device* device = nullptr;
    srv->GetDevice(&device);
    if (!device) return;

    ID3D11DeviceContext* context = nullptr;
    device->GetImmediateContext(&context);
    if (!context) return;

    D3D11_BLEND_DESC desc{};
    desc.RenderTarget[0].BlendEnable = TRUE;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    ID3D11BlendState* blendState = nullptr;
    if (FAILED(device->CreateBlendState(&desc, &blendState))) return;

    g_additive.device = device;
    g_additive.context = context;
    g_additive.blendState = blendState;
}

void EnableAdditiveBlendCallback(const ImDrawList*, const ImDrawCmd*) {
    if (!g_additive.context || !g_additive.blendState) return;
    const float blendFactor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    g_additive.context->OMSetBlendState(g_additive.blendState, blendFactor, 0xffffffff);
}
#endif

}  // namespace

void DrawRotatedImage(ImDrawList* draw,
                      ImTextureID texture,
                      ImVec2 center,
                      ImVec2 halfSize,
                      float angleRadians,
                      ImU32 color) {
    if (!draw || !texture) return;

    const float c = std::cos(angleRadians);
    const float s = std::sin(angleRadians);
    const auto corner = [&](float lx, float ly) -> ImVec2 {
        return {center.x + lx * c - ly * s, center.y + lx * s + ly * c};
    };

    const ImVec2 p1 = corner(-halfSize.x, -halfSize.y);
    const ImVec2 p2 = corner(halfSize.x, -halfSize.y);
    const ImVec2 p3 = corner(halfSize.x, halfSize.y);
    const ImVec2 p4 = corner(-halfSize.x, halfSize.y);
    draw->AddImageQuad(texture, p1, p2, p3, p4, ImVec2(0, 0), ImVec2(1, 0), ImVec2(1, 1),
                       ImVec2(0, 1), color);
}

void DrawRotatedImageAdditive(ImDrawList* draw,
                              ImTextureID texture,
                              ImVec2 center,
                              ImVec2 halfSize,
                              float angleRadians,
                              ImU32 color) {
    if (!draw || !texture) return;

#ifdef _WIN32
    EnsureAdditiveBlendState(texture);
    if (g_additive.blendState) {
        draw->AddCallback(EnableAdditiveBlendCallback, nullptr);
        DrawRotatedImage(draw, texture, center, halfSize, angleRadians, color);
        draw->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
        return;
    }
#endif

    DrawRotatedImage(draw, texture, center, halfSize, angleRadians, color);
}

float EaseBounceInOut(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    if (t < 0.5f) {
        return 0.5f * (1.0f - std::cos(t * 3.1415926535f));
    }
    return 0.5f + 0.5f * std::sin((t - 0.5f) * 3.1415926535f);
}

}  // namespace DrawUtils
}  // namespace hr
