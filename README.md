# Happy Reset (Nexus)

Nexus addon port of [BlishHud-HappyReset](https://github.com/a727891/BlishHud-HappyReset).

Brings back the daily login chest - click it to open the Wizard's Vault after each daily reset.

## Requirements

- [Nexus](https://raidcore.gg/gw2/nexus) in Guild Wars 2
- Windows x64 at runtime (DLL is cross-compiled from Linux with MinGW)

Optional: GW2 API key with **account** permission if you use multiple accounts (persistence is keyed by account name).

## Installation

Copy `HappyReset.dll` to `<GW2>/addons/` (directly in `addons/`, not a subfolder).


## Build

```bash
sudo dnf install mingw64-gcc-c++ mingw64-winpthreads-static cmake ninja git python3
chmod +x scripts/build-release.sh scripts/deploy-to-gw2.sh
./scripts/build-release.sh
```

Output: `dist/HappyReset.dll` (stripped release export)

Or configure and build directly:

```bash
cmake -B build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-w64-toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Local deploy:

```bash
./scripts/deploy-to-gw2.sh --release
```

## GitHub Actions (main → release build)

Pushes to `main` run `.github/workflows/release.yml`: MinGW release build, workflow
artifact upload, and GitHub release (see `.github/nexus-release.json`).

See [templates/github-actions/README.md](templates/github-actions/README.md).

## Settings

In Nexus addon options: chest location, wiggle/bounce, and shine. Clicking the chest opens the Wizard's Vault via the game's seasonal objectives shop bind (same effect as your in-game Wizard's Vault keybind).

Persistence (`HappyReset/data/resets.json`) tracks when you last opened the chest per account.
