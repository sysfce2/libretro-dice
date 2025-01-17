# Multiple mice (advanced)

## Overview
On some platforms (Linux, Windows, Mac), DICE can be compiled with support for multiple mice.

This support uses 
* https://github.com/icculus/manymouse

and shares all the same system requirements.  
* In particular, see https://github.com/icculus/manymouse/issues/10 if running Wayland on Linux.

Using `detect_mice` from that repo can be very helpful when debugging a manymouse setup;
if that can't find your mice, neither will DICE.

## Compiling
Set `MANYMOUSE=1` in `Makefile.libretro` when compiling per README.md.

## Axis setup

Axes are configured in "Core Options", rather than the usual controls setup.

The "usual mouse" is index 0.

(Mouse buttons can be configured using the usual RetroArch systems.)

### One player using a mouse
| Option           | Value | Notes                                       |
| ---------------- | ----- | ------------------------------------------- |
| Mouse-Paddle 1   | ON    | Enable mouse as paddle.                     |
| Mouse-Paddle 1 x | 0x    | Used for "horizontal motion" like Breakout  |
| Mouse-Paddle 1 y | 0y    | Used for "vertical motion" like Pong        |

### Two players each using mice
| Option           | Value |
| ---------------- | ----- |
| Mouse-Paddle 1   | ON    |
| Mouse-Paddle 1 x | 0x    |
| Mouse-Paddle 1 y | 0y    |
| Mouse-Paddle 2   | ON    |
| Mouse-Paddle 2 x | 1x    |
| Mouse-Paddle 2 y | 1y    |

### Other controllers
Other controllers map motion to different mouse axes.

#### Thunderstick GRS and similar spinners
_(Untested)_

Each spinner appears as a separate mouse, and most can switch between axes with a button-hold or similar.

Two players each using single-axis spinners like the Thunderstick GRS (and neither using the usual Mouse 0):
| Option           | Value |
| ---------------- | ----- |
| Mouse-Paddle 1 x | 1x    |
| Mouse-Paddle 1 y | 1x    |
| Mouse-Paddle 2 x | 1y    |
| Mouse-Paddle 2 y | 1y    |

#### Stelladaptor 2600 and Atari paddles
_(Untested)_

On Stelladaptor shows a single mouse, with each axis controlled by a different paddle controller.

Two players using a shared Stelladaptor 2600 with two Atari paddles (and neither using the usual Mouse 0):
| Option           | Value |
| ---------------- | ----- |
| Mouse-Paddle 1 x | 1x    |
| Mouse-Paddle 1 y | 1x    |
| Mouse-Paddle 2 x | 1y    |
| Mouse-Paddle 2 y | 1y    |
