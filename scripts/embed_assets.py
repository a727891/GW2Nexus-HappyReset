#!/usr/bin/env python3
"""Generate C++ sources that embed Happy Reset PNG assets into the DLL."""

from __future__ import annotations

import argparse
import io
from pathlib import Path


def sanitize_symbol(name: str) -> str:
    return "".join(ch if ch.isalnum() else "_" for ch in Path(name).stem)


def format_bytes(data: bytes) -> str:
    lines = []
    row = []
    for byte in data:
        row.append(f"0x{byte:02x}")
        if len(row) == 16:
            lines.append("    " + ", ".join(row) + ",")
            row = []
    if row:
        lines.append("    " + ", ".join(row) + ",")
    return "\n".join(lines)


def key_black_to_transparent_png(data: bytes, threshold: int = 24) -> bytes:
    """GW2 shine assets ship with opaque black backgrounds; key them out for ImGui alpha."""
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        return data

    try:
        from PIL import Image

        image = Image.open(io.BytesIO(data)).convert("RGBA")
        pixels = image.load()
        width, height = image.size
        for y in range(height):
            for x in range(width):
                r, g, b, a = pixels[x, y]
                if a == 0:
                    continue
                if max(r, g, b) <= threshold:
                    pixels[x, y] = (0, 0, 0, 0)
        out = io.BytesIO()
        image.save(out, format="PNG")
        return out.getvalue()
    except ImportError:
        return data


def prepare_texture_bytes(path: Path) -> bytes:
    data = path.read_bytes()
    if path.stem == "965696":
        return key_black_to_transparent_png(data)
    return data


def write_textures(textures_dir: Path, out_cpp: Path, out_h: Path) -> None:
    files = sorted(textures_dir.glob("*.png"))
    if not files:
        raise SystemExit(f"no PNG files in {textures_dir}")

    blob_sections = []
    assets = []
    for png in files:
        data = prepare_texture_bytes(png)
        symbol = sanitize_symbol(png.name)
        blob_sections.append(
            f"alignas(4) const unsigned char kTextureBlob_{symbol}[] = {{\n"
            f"{format_bytes(data)}\n"
            f"}};\n"
            f"const size_t kTextureBlobSize_{symbol} = {len(data)};\n"
        )
        assets.append(
            f'    {{"{png.stem}", "HR_TEX_{symbol}", '
            f"kTextureBlob_{symbol}, kTextureBlobSize_{symbol}}},"
        )

    out_h.write_text(
        """#pragma once

#include <cstddef>

namespace hr {
namespace EmbeddedTextures {

struct Asset {
    const char* assetId;
    const char* identifier;
    const unsigned char* data;
    size_t size;
};

"""
        + "".join(blob_sections)
        + f"""
static const Asset kAssets[] = {{
{chr(10).join(assets)}
}};

inline const Asset* Find(const char* assetId) {{
    for (const auto& asset : kAssets) {{
        if (asset.assetId == assetId) {{
            return &asset;
        }}
    }}
    return nullptr;
}}

}}  // namespace EmbeddedTextures
}}  // namespace hr
"""
    )

    out_cpp.write_text(
        """#include "EmbeddedTextures.h"

namespace hr {
namespace EmbeddedTextures {
}  // namespace EmbeddedTextures
}  // namespace hr
"""
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--textures-dir", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)
    write_textures(
        args.textures_dir,
        args.output_dir / "EmbeddedTextures.cpp",
        args.output_dir / "EmbeddedTextures.h",
    )


if __name__ == "__main__":
    main()
