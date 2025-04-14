# Multiple mice using ManyMouse (advanced)

## Overview
On some platforms (Linux, Windows, Mac), DICE can be compiled with support for multiple mice.

This support uses 
* https://github.com/icculus/manymouse

and shares all the same system requirements.  
* In particular, see https://github.com/icculus/manymouse/issues/10 if running Wayland on Linux.

Using `detect_mice` from that repo can be very helpful when debugging a manymouse setup;
if that can't find your mice, neither will DICE.

This setup is more complicated than [retromouse.md](retromouse.md) --- you might
want to try that first.
## Compiling
Set `MANYMOUSE=1` in `Makefile.libretro` when compiling per [README.md](README.md#compiling).

## Axis setup

Mouse selection and axes are configured in "Core Options", rather than the usual input setup.
You bind either the X or Y axis of your mouse to the single axis of the virtual paddle controller.

The "usual mouse" is often (but not always) index 0; use `detect_mice` to find your system's index assignments.

(Mouse buttons can be configured using the usual RetroArch systems.)

### One player using a mouse
| Option                       | Value | Notes                                                                       |
| ---------------------------- | ----- | --------------------------------------------------------------------------- |
| ManyMouse > Mouse-Paddle 1   | ON    | Enable mouse as paddle.                                                     |
| ManyMouse > Mouse-Paddle 1 x | 0x    | Control "horizontal motion" like Breakout using horizontal mouse movements. |
| ManyMouse > Mouse-Paddle 1 y | 0y    | Control "vertical motion" like Pong using vertical mouse movements.         |

### Two players each using mice
| Option                       | Value |
| ---------------------------- | ----- |
| ManyMouse > Mouse-Paddle 1   | ON    |
| ManyMouse > Mouse-Paddle 1 x | 0x    |
| ManyMouse > Mouse-Paddle 1 y | 0y    |
| ManyMouse > Mouse-Paddle 2   | ON    |
| ManyMouse > Mouse-Paddle 2 x | 1x    |
| ManyMouse > Mouse-Paddle 2 y | 1y    |

### Other controllers
Other controllers map motion to different mouse axes.

#### Thunderstick GRS spinners
Each spinner appears as a separate mouse, and can switch between axes with a button-hold or similar.

Two players each using single-axis spinners like the Thunderstick GRS (and neither using the usual Mouse 0):
| Option                       | Value |
| ---------------------------- | ----- |
| ManyMouse > Mouse-Paddle 1 x | 1x    |
| ManyMouse > Mouse-Paddle 1 y | 1x    |
| ManyMouse > Mouse-Paddle 2 x | 2x    |
| ManyMouse > Mouse-Paddle 2 y | 2x    |

#### Stelladaptor 2600 and Atari paddles
_(Untested)_

One Stelladaptor appears as a single mouse, with each axis controlled by a different paddle controller.

Two players using a shared Stelladaptor 2600 with two Atari paddles (and neither using the usual Mouse 0):
| Option                       | Value |
| ---------------------------- | ----- |
| ManyMouse > Mouse-Paddle 1 x | 1x    |
| ManyMouse > Mouse-Paddle 1 y | 1x    |
| ManyMouse > Mouse-Paddle 2 x | 1y    |
| ManyMouse > Mouse-Paddle 2 y | 1y    |
