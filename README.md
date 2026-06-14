# Happy Reset (Nexus)

Nexus addon port of [BlishHud-HappyReset](https://github.com/a727891/BlishHud-HappyReset).

Brings back the daily login chest — click it to open the Wizard's Vault after each daily reset.

## Requirements

- [Nexus](https://raidcore.gg/gw2/nexus) in Guild Wars 2
- Windows x64 at runtime (DLL is cross-compiled from Linux with MinGW)

Optional: GW2 API key with **account** permission if you use multiple accounts (persistence is keyed by account name).

## Installation

Copy `NexusHappyReset.dll` to `<GW2>/addons/` (directly in `addons/`, not a subfolder).

Chest and shine textures are **embedded in the DLL** at build time — end users only need the single DLL file.

## Build

```bash
sudo dnf install mingw64-gcc-c++ mingw64-winpthreads-static cmake ninja git python3
chmod +x scripts/build-release.sh scripts/deploy-to-gw2.sh
./scripts/build-release.sh
```

Output: `dist/NexusHappyReset.dll` (stripped release export)

Local deploy:

```bash
./scripts/deploy-to-gw2.sh --release
```

## Settings

In Nexus addon options: chest location, wiggle/bounce, and shine. Clicking the chest opens the Wizard's Vault via the game's seasonal objectives shop bind (same effect as your in-game Wizard's Vault keybind).

Persistence (`happyReset/resets.json`) tracks when you last opened the chest per account.
