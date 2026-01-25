# sunvox-max

A Max/MSP external that embeds the [SunVox](https://www.warmplace.ru/soft/sunvox/sunvox_lib.php) modular synthesizer engine.

## Status

Cross-platform external (macOS and Windows) with comprehensive SunVox API coverage (60+ messages).

## Features

- Load and play SunVox projects (.sunvox files)
- Full playback control (play, pause, stop, rewind)
- Real-time note/event triggering
- Module creation, connection, and parameter control
- Pattern creation and editing
- Sample loading into Sampler modules
- Song info queries via info outlet
- Stereo audio input/output with proper interleaving

## Installation

### macOS

> **Note**: On macOS, `make setup` creates a symbolic link from your Max Packages folder to this repository. This means:
> - Changes to the repo are immediately reflected in Max
> - You must keep this repository in place (do not move or delete it)
>
> If you prefer traditional installation (copying files instead of symlinks), use `make submodules` instead of `make setup`, then copy the built package manually.

```bash
git clone https://github.com/yourusername/sunvox-max.git
cd sunvox-max
make setup   # Initialize submodules, create symlinks to Max Packages
make all     # Clean and build (Xcode signs automatically)
```

**Requirements**: Xcode Command Line Tools, CMake (`brew install cmake`)

**Output**: `externals/sunvox~.mxo`

### Windows

```bash
git clone https://github.com/yourusername/sunvox-max.git
cd sunvox-max
make setup   # Initialize submodules
make all     # Clean and build
```

After building, manually copy the entire repository folder to your Max Packages directory:
- `%USERPROFILE%\Documents\Max 8\Packages\` (or Max 9)

Alternatively, copy just the external (`externals\sunvox~.mxe64`) and help file (`help\sunvox~.maxhelp`) to your Max externals folder.

**Requirements**: Visual Studio 2022 with C++ workload, CMake (from [cmake.org](https://cmake.org/download/) or via `winget install cmake`)

**Output**: `externals/sunvox~.mxe64`

### Manual Installation (Alternative)

If you prefer not to use the Makefile or symlinks:

1. Initialize submodules: `git submodule update --init`
2. Build with CMake manually
3. Copy the entire repository folder to your Max Packages directory, or copy just the external and help file

## Build Targets

```bash
make setup      # Initialize submodules (+ symlinks on macOS)
make build      # Configure and build the external
make clean      # Remove build artifacts
make sign       # Code sign (macOS only, for non-Xcode builds)
make all        # Clean and build
make help       # Show all available targets
```

## Usage

### Basic Example

```
[loadbang]
|
[load song.sunvox]
|
[sunvox~]
|   |   |
[dac~] [print info]
```

### Outlets

| Outlet | Type | Description |
|--------|------|-------------|
| 0 | signal | Left audio output |
| 1 | signal | Right audio output |
| 2 | anything | Info/query responses |

### Inlets

| Inlet | Type | Description |
|-------|------|-------------|
| 0 | signal | Left audio input (to SunVox Input module) |
| 1 | signal | Right audio input |

## Messages

### File I/O

| Message | Arguments | Description |
|---------|-----------|-------------|
| `load` | `<filename>` | Load a SunVox project |
| `read` | `<filename>` | Alias for load |
| `save` | `<filename>` | Save current project |
| `write` | `<filename>` | Alias for save |

### Playback Control

| Message | Arguments | Description |
|---------|-----------|-------------|
| `play` | - | Play from current position |
| `playfrom` | - | Play from beginning |
| `stop` | - | Stop playback |
| `pause` | - | Pause playback |
| `resume` | - | Resume playback |
| `rewind` | `<line>` | Rewind to line number |
| `volume` | `<0-256>` | Set master volume |
| `autostop` | `<0\|1>` | Enable/disable auto-stop at song end |

### Song Info Queries

| Message | Arguments | Output (via info outlet) |
|---------|-----------|--------------------------|
| `getinfo` | - | Dictionary with all song info |
| `getname` | - | `name <songname>` |
| `setname` | `<name>` | - |
| `getbpm` | - | `bpm <value>` |
| `gettpl` | - | `tpl <value>` |
| `getlength` | - | `length <frames> <lines>` |
| `getline` | - | `line <value>` |
| `getlevel` | `<chan>` | `level <chan> <value>` |
| `endofsong` | - | `endofsong <0\|1>` |

### Events

| Message | Arguments | Description |
|---------|-----------|-------------|
| `note` | `<mod> <note> [vel] [track]` | Send note (mod can be number or name) |
| `noteoff` | `<mod> [track]` | Send note off |
| `allnotesoff` | - | All notes off on all modules |
| `cleansynths` | - | Clear all module buffers |
| `event` | `<track> <note> <vel> <mod> <ctl> <ctl_val>` | Raw event |
| `ctl` | `<mod> <ctl> <val> [scaled]` | Set controller value |

### Module Management

| Message | Arguments | Description |
|---------|-----------|-------------|
| `newmodule` | `<type> <name> [x y z]` | Create module |
| `removemodule` | `<mod>` | Remove module |
| `connect` | `<src> <dst>` | Connect modules |
| `disconnect` | `<src> <dst>` | Disconnect modules |
| `loadmodule` | `<file> [x y z]` | Load module from file |
| `findmodule` | `<name>` | Find module by name |
| `getmodule` | `<mod>` | Get module info (dictionary) |
| `modules` | - | List all modules |
| `setmodulename` | `<mod> <name>` | Set module name |
| `setmodulexy` | `<mod> <x> <y>` | Set module position |
| `setmodulecolor` | `<mod> <color>` | Set module color (0xBBGGRR) |
| `setfinetune` | `<mod> <val>` | Set module finetune |
| `setrelnote` | `<mod> <val>` | Set module relative note |

### Controller Queries

| Message | Arguments | Output |
|---------|-----------|--------|
| `getctl` | `<mod> <ctl> [scaled]` | `ctl <mod> <ctl> <name> <val> <min> <max> <type>` |
| `getctls` | `<mod>` | All controllers for module |

### Sampler/MetaModule

| Message | Arguments | Description |
|---------|-----------|-------------|
| `samplerload` | `<mod> <file> [slot]` | Load sample into Sampler |
| `metamoduleload` | `<mod> <file>` | Load project into MetaModule |
| `vplayerload` | `<mod> <file>` | Load OGG into Vorbis Player |

### Pattern Management

| Message | Arguments | Description |
|---------|-----------|-------------|
| `newpattern` | `<clone> <x> <y> <tracks> <lines> <name>` | Create pattern |
| `removepattern` | `<pat>` | Remove pattern |
| `findpattern` | `<name>` | Find pattern by name |
| `getpattern` | `<pat>` | Get pattern info (dictionary) |
| `patterns` | - | List all patterns |
| `setpatternxy` | `<pat> <x> <y>` | Set pattern position |
| `setpatternsize` | `<pat> <tracks> <lines>` | Resize pattern |
| `setpatternname` | `<pat> <name>` | Set pattern name |
| `patternmute` | `<pat> <0\|1>` | Mute/unmute pattern |
| `setpatternevent` | `<pat> <track> <line> <nn> <vv> <mm> <ccee> <xxyy>` | Set event |
| `getpatternevent` | `<pat> <track> <line>` | Get event |

### Utility

| Message | Arguments | Description |
|---------|-----------|-------------|
| `log` | `[size]` | Print SunVox log |
| `samplerate` | - | Get current sample rate |
| `version` | - | Get version info |

## Attributes

| Attribute | Type | Default | Description |
|-----------|------|---------|-------------|
| `@volume` | int | 256 | Master volume (0-256) |
| `@autostop` | int | 0 | Auto-stop at song end |

## Module Types

Common module types for `newmodule`:

- `Analog Generator`, `Generator`, `FM`, `FMX`
- `Sampler`, `SpectraVoice`, `DrumSynth`
- `Filter`, `Amplifier`, `Distortion`, `Echo`, `Reverb`
- `LFO`, `ADSR`, `MultiCtl`, `Velocity2Ctl`
- `MetaModule`, `Sound2Ctl`, `Pitch2Ctl`

See SunVox documentation for complete list.

## License

MIT License. SunVox library is copyright Alexander Zolotov (WarmPlace.ru).

## Links

- [SunVox](https://www.warmplace.ru/soft/sunvox/)
- [SunVox Library Documentation](https://www.warmplace.ru/soft/sunvox/sunvox_lib.php)
