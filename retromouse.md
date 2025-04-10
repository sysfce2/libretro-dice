# Multiple mice using libretro (somewhat advanced)

## Overview
On some platforms (Linux, Windows) and certain input drivers (Linux udev, Windows raw),
DICE supports multiple mice.

This support uses 
* https://docs.libretro.com/guides/input-controller-drivers/

and shares all the same system requirements.  

Use "Settings > Input > RetroPad Binds > Port 1 Controls > Mouse Index"
to check if you have a working multiple-mouse setup --- descriptors
like "wireless 2.4G Mouse" or "Logitech K400" are good, "Default" and "N/A (#2)"
are bad.

## Axis setup
Mouse choice for each player is in "Settings > Input > RetroPad Binds > Port 1/2/.. Controls > Mouse Index".

Enabling mouse paddle for each player is in "Core Options > Retromouse".

Axes overrides are also configured in "Core Options > Retromouse", rather than the usual input setup.
The defaults work for most actual mice --- these overrides are mostly used for
mouse-like spinners.
You bind either the X or Y axis of your mouse to the single axis of the virtual paddle controller.

(Mouse buttons can be configured using the usual RetroArch systems.)

### One player using a mouse
| Option                        | Value | Notes                                                                       |
| ----------------------------- | ----- | --------------------------------------------------------------------------- |
| Retromouse > Mouse-Paddle 1   | ON    | Enable mouse as paddle.                                                     |
| Retromouse > Mouse-Paddle 1 x | x     | Control "horizontal motion" like Breakout using horizontal mouse movements. |
| Retromouse > Mouse-Paddle 1 y | y     | Control "vertical motion" like Pong using vertical mouse movements.         |


| Bind                                           | Value   | Notes  |
| ---------------------------------------------- | ------- | ------ |
| Retropad Binds > Port 1 Controls > Mouse Index | mouse 1 |        |

### Two players each using mice
| Option                        | Value |
| ----------------------------- | ----- |
| Retromouse > Mouse-Paddle 1   | ON    |
| Retromouse > Mouse-Paddle 1 x | x     |
| Retromouse > Mouse-Paddle 1 y | y     |
| Retromouse > Mouse-Paddle 2   | ON    |
| Retromouse > Mouse-Paddle 2 x | x     |
| Retromouse > Mouse-Paddle 2 y | y     |

| Bind           | Value | Notes                                                                       |
| ---------------- | ----- | --------------------------------------------------------------------------- |
| Retropad Binds > Port 1 Controls > Mouse Index | mouse 1 | |
| Retropad Binds > Port 2 Controls > Mouse Index | mouse 2 | |

### Other controllers
Other controllers map motion to different mouse axes.

#### Thunderstick GRS spinners
Each spinner appears as a separate mouse, and can switch between axes with a button-hold or similar.

Two players each using single-axis spinners like the Thunderstick GRS (and neither using the usual Mouse 0):
| Option           | Value |
| ---------------- | ----- |
| Mouse-Paddle 1 x | x     |
| Mouse-Paddle 1 y | x     |
| Mouse-Paddle 2 x | x     |
| Mouse-Paddle 2 y | x     |

| Bind           | Value | Notes                                                                       |
| ---------------- | ----- | --------------------------------------------------------------------------- |
| Retropad Binds > Port 1 Controls > Mouse Index | Thunderstick ... 1 | |
| Retropad Binds > Port 2 Controls > Mouse Index | Thunderstick ... 2 | |

#### Stelladaptor 2600 and Atari paddles
_(Untested)_

One Stelladaptor appears as a single mouse, with each axis controlled by a different paddle controller.

Two players using a shared Stelladaptor 2600 with two Atari paddles (and neither using the usual Mouse 0):
| Option           | Value |
| ---------------- | ----- |
| Mouse-Paddle 1 x | x     |
| Mouse-Paddle 1 y | x     |
| Mouse-Paddle 2 x | y     |
| Mouse-Paddle 2 y | y     |

| Bind           | Value | Notes                                                                       |
| ---------------- | ----- | --------------------------------------------------------------------------- |
| Retropad Binds > Port 1 Controls > Mouse Index | Stelladaptor 1 | |
| Retropad Binds > Port 2 Controls > Mouse Index | Stelladaptor 1 | Note
duplicate |
