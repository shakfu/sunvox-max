# Changelog

All notable changes to sunvox-max will be documented in this file.

## [1.0.2] - 2025-01-25

### Added
- **Windows support** - Makefile now builds on both macOS and Windows
  - Platform detection via `$(OS)` and `uname -s`
  - Windows uses Visual Studio 2022 generator, `.mxe64` output
  - macOS uses Xcode generator, `.mxo` output

### Changed
- **Makefile refactored for cross-platform builds**
  - Platform-specific variables for paths, generators, and commands
  - Code signing removed from default `make all` target (Xcode signs automatically)
  - `make sign` still available for non-Xcode generator builds
  - Symlink creation (`make link`) skipped on Windows (requires admin privileges)
  - `make setup` on Windows only initializes submodules; users copy package manually
- **README updated** with separate macOS and Windows installation instructions

## [1.0.1] - 2025-01-24

### Fixed
- **Crash on audio start** - Fixed segfault when enabling audio in Max/MSP
  - Root cause: `g_app_config` array in SunVox library was terminated with `"0"` (string) instead of `NULL` (null pointer)
  - The config loop would read past array bounds, passing garbage pointer `0x1` to `smem_strlen()`, causing SIGSEGV
  - Fix: Changed sentinel from `"0"` to `NULL` in `sunvox_lib.cpp`

## [1.0.0] - 2025-01-20

### Added

#### Core Features
- Info outlet (3rd outlet) for query responses
- Attributes system with `@volume` and `@autostop`
- Proper attribute processing and persistence

#### Playback Control
- `play` - Play from current position
- `playfrom` - Play from beginning (replaces old `play` behavior)
- `pause` - Pause audio stream
- `resume` - Resume audio stream
- `rewind <line>` - Rewind to specific line
- `autostop <0|1>` - Enable/disable auto-stop at song end

#### File I/O
- `read` - Alias for load
- `save <filename>` - Save current project
- `write` - Alias for save

#### Song Info Queries
- `getinfo` - Get comprehensive song info as dictionary
- `getname` / `setname` - Get/set song name
- `getbpm` - Get beats per minute
- `gettpl` - Get ticks per line
- `getlength` - Get song length in frames and lines
- `getline` - Get current playback line
- `getlevel <chan>` - Get signal level (0-255)
- `endofsong` - Check if song has ended

#### Events
- `noteoff <mod> [track]` - Send note off to module
- `allnotesoff` - All notes off on all modules
- `cleansynths` - Clear all module buffers
- `event <track> <note> <vel> <mod> <ctl> <ctl_val>` - Send raw event
- `ctl <mod> <ctl> <val> [scaled]` - Set controller value

#### Module Management
- `newmodule <type> <name> [x y z]` - Create new module
- `removemodule <mod>` - Remove module
- `connect <src> <dst>` - Connect modules
- `disconnect <src> <dst>` - Disconnect modules
- `loadmodule <file> [x y z]` - Load module from file
- `findmodule <name>` - Find module by name
- `getmodule <mod>` - Get module info as dictionary
- `modules` - List all modules
- `setmodulename <mod> <name>` - Set module name
- `setmodulexy <mod> <x> <y>` - Set module position
- `setmodulecolor <mod> <color>` - Set module color
- `setfinetune <mod> <val>` - Set module finetune
- `setrelnote <mod> <val>` - Set module relative note

#### Controller Queries
- `getctl <mod> <ctl> [scaled]` - Get controller value and info
- `getctls <mod>` - Get all controllers for module

#### Sampler/MetaModule Support
- `samplerload <mod> <file> [slot]` - Load sample into Sampler
- `metamoduleload <mod> <file>` - Load project into MetaModule
- `vplayerload <mod> <file>` - Load OGG into Vorbis Player

#### Pattern Management
- `newpattern <clone> <x> <y> <tracks> <lines> <name>` - Create pattern
- `removepattern <pat>` - Remove pattern
- `findpattern <name>` - Find pattern by name
- `getpattern <pat>` - Get pattern info as dictionary
- `patterns` - List all patterns
- `setpatternxy <pat> <x> <y>` - Set pattern timeline position
- `setpatternsize <pat> <tracks> <lines>` - Resize pattern
- `setpatternname <pat> <name>` - Set pattern name
- `patternmute <pat> <0|1>` - Mute/unmute pattern
- `setpatternevent <pat> <track> <line> <nn> <vv> <mm> <ccee> <xxyy>` - Set event
- `getpatternevent <pat> <track> <line>` - Get event data

#### Utility
- `log [size]` - Print SunVox engine log
- `samplerate` - Get current sample rate
- `version` - Get version info

### Changed
- `play` now continues from current position (use `playfrom` for play-from-beginning)
- Enhanced `note` message now accepts module name or number
- Improved inlet/outlet assist strings with descriptive labels

### Fixed
- **Audio interleaving bug** - Fixed incorrect buffer interleaving that caused distorted output
  - Now uses explicit index-based interleaving: `buffer[i * channels + chan]`
  - Clear output buffer before processing
  - Removed problematic pointer arithmetic

### Technical
- Added `buffer_size` tracking to struct
- Proper thread safety with `sv_lock_slot()`/`sv_unlock_slot()` for structural changes
- Helper function `svm_resolve_module()` for name-to-number resolution
- Helper function `svm_locate_file()` for Max search path integration
- Status messages output to info outlet (loaded, playing, stopped, paused, etc.)

## [0.1.0] - 2024-12-XX

### Initial Release
- Basic proof-of-concept
- `load <filename>` - Load SunVox project
- `play` - Play from beginning
- `stop` - Stop playback
- `volume <0-256>` - Set volume
- `note <mod> <note> [vel] [track]` - Send note
- Stereo audio input/output
- macOS only
