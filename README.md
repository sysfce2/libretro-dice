# dice-libretro

![dice logo](ui/icon.png)

DICE is a Discrete Integrated Circuit Emulator. It emulates computer systems
that lack any type of CPU, consisting only of discrete logic components.

dice-libretro is a Libretro port of DICE, to run in RetroArch.

Upstream, standalone DICE has some nice screenshots:
* https://adamulation.blogspot.com/

## Usage

Some games use zipped ROMs and launch similarly to MAME or FBNeo ---
filename is important, see table below.

Please do not attempt to contact the DICE team
to request ROM files.

Some games (pong, breakout, pinpong, etc) do not have any
ROM on the board at all.  For these, copy the dummy launcher
file from dummy_files to your ROM folder; these have a
correct name (for example, pong.dmy) that will get RetroArch to set up
lr-dice for the correct game.

(If you use a ROM manager like RomVault or clrmamepro, it may suggest wrapping
pong.dmy in pong.zip --- that works fine as well.)

### Games

|	Name	|	Base filename	|	Publisher	|	Year |	Needs ROM?	|
| ----  | ------------- | --------- | ---- | -------- |
|	Anti-Aircraft	|	antiaircraft	|	Atari	|	1975	|	x	|
|	Attack	|	attack	|	Exidy	|	1977	|	x	|
|	Breakout	|	breakout	|	Atari	|	1976	|		|
|	Clean Sweep	|	cleansweep	|	Ramtek	|	1974	|	x	|
|	Crash 'N Score	|	crashnscore	|	Atari	|	1975	|	x	|
|	Crossfire	|	crossfire	|	Atari	|	1975	|		|
|	Gotcha	|	gotcha	|	Atari	|	1973	|		|
|	Hi-Way	|	hiway	|	Atari	|	1975	|		|
|	Indy 4	|	indy4	|	Atari	|	1976	|	x	|
|	Jet Fighter	|	jetfighter	|	Atari	|	1975	|	x	|
|	Pin Pong	|	pinpong	|	Atari	|	1974	|		|
|	Pong	|	pong	|	Atari	|	1972	|		|
|	Pong Doubles	|	pongdoubles	|	Atari	|	1973	|		|
|	Quadrapong	|	quadrapong	|	Atari	|	1974	|		|
|	Rebound	|	rebound	|	Atari	|	1974	|		|
|	Shark Jaws	|	sharkjaws	|	Atari	|	1975	|	x	|
|	Space Race	|	spacerace	|	Atari	|	1973	|		|
|	Steeplechase	|	steeplechase	|	Atari	|	1975	|	x	|
|	Stunt Cycle	|	stuntcycle	|	Atari	|	1976	|	x	|
|	TV Basketball	|	tvbasketball	|	Midway	|	1974	|		|
|	Wipe Out	|	wipeout	|	Ramtek	|	1974	|	x	|

Several of these (including Pong and Breakout) are now supported by MAME, but several more are not per the list on http://nonmame.retrogames.com/#discrete_circuitry_miscellaneous .

## Mouse support
* Easy: One mouse can be used for Paddle 1.  Use "Core Options -> Use mouse pointer for paddle 1".  You'll still want a keyboard or gamepad handy to have enough buttons.

  (Mouse buttons can be configured using the usual RetroArch binding and remapping, https://docs.libretro.com/guides/input-and-controls/ ).

* Somewhat advanced: Multiple mice are supported using certain libretro drivers on
Linux and Windows, see [retromouse.md](retromouse.md).

* More advanced: Multiple mice are a compile-time option on Linux, Windows, and Mac, see 
[manymouse.md](manymouse.md).

## DIP Switches

Settable in "Core Options," see [dipswitches.md](dipswitches.md).

## Compiling
Prebuilt binaries for several platforms are available through RetroArch's "Online Updater -> Core Downloader".  You may need to do "Online Updater -> Update Core Info Files" first.

For other platforms, use:
* https://docs.libretro.com/development/retroarch/compilation/linux-and-bsd/#building-libretro-cores
or similar.

# Contact Information
* Github: https://github.com/mittonk/dice-libretro

For upstream standalone DICE (inactive):
* Project Page: http://sourceforge.net/projects/dice/
* Email: dice.emulator@gmail.com

For a fork of standalone DICE running on modern Linux systems:
* https://github.com/DirtBagXon/DICE

# Credits

Many thanks to _Adam B_ for his work on `DICE`: https://adamulation.blogspot.com/

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

