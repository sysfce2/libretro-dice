# dice-libretro

![dice logo](ui/icon.png)

DICE is a Discrete Integrated Circuit Emulator. It emulates computer systems
that lack any type of CPU, consisting only of discrete logic components.

dice-libretro is a Libretro port of DICE, to run in RetroArch.

Upstream, standalone DICE has some nice screenshots:
* https://adamulation.blogspot.com/

## Usage

Some games use ROMs and launch similarly to MAME or FBNeo ---
filename is important.

Please do not attempt to contact the DICE team
to request ROM files.

Some games (pong, breakout, pinpong, etc) do not have any
ROM on the board at all.  For these, copy the dummy launcher
file from dummy_files to your ROM folder; these have a
correct name (ie, pong.dmy) that will get RetroArch to set up
lr-dice for the correct game.

### Games
* antiaircraft  (rom needed)
* attack  (rom needed)
* breakout
* cleansweep  (rom needed)
* crashnscore  (rom needed)
* crossfire
* gotcha
* jetfighter  (rom needed)
* hiway
* indy4  (rom needed)
* pinpong
* pong
* pongdoubles
* quadrapong
* rebound
* sharkjaws  (rom needed)
* spacerace
* steeplechase  (rom needed)
* stuntcycle  (rom needed)
* tvbasketball
* wipeout  (rom needed)

## Compiling
Working on getting this integrated with libretro's CI/CD infrastructure.  Core is tagged "experimental" so won't show up in most by default.

Pre-built binaries for a few platforms are available at:
* [Build artifacts](https://git.libretro.com/libretro/dice-libretro/-/artifacts)

For other platforms, use:
* https://docs.libretro.com/development/retroarch/compilation/linux-and-bsd/#building-libretro-cores
or similar.

# Contact Information
* Github: https://github.com/mittonk/dice-libretro

For upstream standalone DICE (inactive):
* Project Page: http://sourceforge.net/projects/dice/
* Email: dice.emulator@gmail.com


# License

Copyright (C) 2008-2025 DICE Team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

