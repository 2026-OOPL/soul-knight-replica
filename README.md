# Soul Knight Replica

This is a 《Soul Knight》game clone written in C++ with [PTSD](https://github.com/ntut-open-source-club/practical-tools-for-simple-design) framework.

## Quick Start
1. Clone your repository
   ```bash
   git clone YOUR_GIT_URL --recursive
   ```
   
2. Build your project
   ```sh
   cmake -DCMAKE_BUILD_TYPE=Debug -B build # -G Ninja
   ```
   For more information, please read [PTSD README](https://github.com/ntut-open-source-club/practical-tools-for-simple-design)

## Game Controls

| Key | Function |
| --- | --- |
| `W` | Move up |
| `A` | Move left |
| `S` | Move down |
| `D` | Move right |
| `Space` | Hold to melee attack when close to a target; otherwise fire the current weapon |
| `Q` | Switch between the two carried weapons |
| `E` | Pick up a dropped weapon when standing near it; if both weapon slots are full, drop the current weapon |
| `T` | Toggle invincible mode; while enabled, the player takes no damage and weapon ammo is not consumed |
| `Left Mouse Button` | Click menu buttons |
| `Esc` | Exit the game |

### Game Debug Controls

| Key | Function |
| --- | --- |
| `R` | Toggle collision debug display |
| `P` | Clear the current fight room for debugging |

## Documentation

- [PTSD framework guide (zh-TW)](docs/PTSD_FRAMEWORK_GUIDE.zh-TW.md)
