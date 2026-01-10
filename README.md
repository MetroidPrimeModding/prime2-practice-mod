# Metroid Prime 2: Echoes Practice Mod
This project can produce a release & patch an Echoes iso to be the practice mod

## How to install (Windows)
- Get an Echoes .iso (i.e. using a hacked wii and cleanrip)
- Copy an Echoes iso to 'prime2.iso' in this directory
- Run `patch.bat`

## How to install (Non-windows)
- Make sure you have python3 installed (it should come with most linux distros/macOS)
- Get an Echoes .iso (i.e. using a hacked wii and cleanrip)
- Copy an Echoes iso to 'prime2.iso' in this directory
- Run `patch.sh` from a terminal
  - You can drag and drop the script into a terminal to get the full path

## Usage
- Launch the iso in Dolphin
- Copy to your wii, and launch using Nintendont
  - If your Nintendont is really old, update Nintendont
  - Note this mod is not currently compatible with some settings, at the very least:
     - Cheats
     - Force progressive
     - Force Widescreen
  - There may be others, not a lot of testing has been done yet
  
## Controls
- `Z` on pause screen - Open menu
- `left analog`/`dpad` - Navigate menu
- `c-stick` - Scroll
- `a` - Select item in menu
- `b` - Go back a menu

## Known Issues
- Rare crashes (so far, inconsistent)
- Warps to a couple rooms will put you out of bounds

## Menus
- Player
  - Save/load position
  - Edit position
  - Light Show/IS toggling
- Inventory
  - Edit your item inventory
- Settings
  - On screen display settings
  - RAM Dump to screen (no decoder yet)
- Warps
  - Warp to any room
- Version number
  - QR code links to releases and the discord
  
## OSD
- Current IGT
- Input Display
- Player Information
  - Current position (x, y, z)
  - Current speed (x, y, z, horizontal total)
  - Currrent rotational speed (x, y, z, total)
- Room timers
  - Current room IGT, in seconds
  - Current room IGT, in frames
  - Previous room IGT, in seconds
  - Previous room IGT, in frames
- Frame time graph, as measured by the game

## Changelog

### 1.2.0
- Add support for jump state in monitor window
- Add ability to move windows around (hold "X" and use c-stick to move)
- Hold L/R to increase/decrease how quickly numbers change in the edit position menu

### 1.1.3
- Fix nintendont? Seems to work more consistently now

### 1.1.2
- Fix warps with player menu open
- Re-order keys to be cleaner

### 1.1.1
- Fix warp to Dark Torvus
- Fix X/Y to be correct in the display

### 1.1.0
- Fix some missing items
- Fix item presets
- Split warp menu into dark/light world
- Automatically show the crash screen on crash

### 1.0.0
 - Initial Release
