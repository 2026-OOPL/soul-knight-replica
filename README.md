# Soul Knight Replica

This is a [PTSD](https://github.com/ntut-open-source-club/practical-tools-for-simple-design) framework template for students taking OOPL2024s.

## Quick Start

1. Use this template to create a new repository
   ![github screenshot](https://github.com/ntut-rick/ptsd-template/assets/126899559/ef62242f-03ed-481d-b858-12b730c09beb)

2. Clone your repository

   ```bash
   git clone YOUR_GIT_URL --recursive
   ```

3. Build your project

  > [!WARNING]
  > Please build your project in `Debug` because our `Release` path is broken D:
   
   ```sh
   cmake -DCMAKE_BUILD_TYPE=Debug -B build # -G Ninja
   ```
   better read [PTSD README](https://github.com/ntut-open-source-club/practical-tools-for-simple-design)

## Controls

| Key | Function |
| --- | --- |
| `W` | Move up |
| `A` | Move left |
| `S` | Move down |
| `D` | Move right |
| `Space` | Hold to melee attack when close to a target; otherwise fire the current weapon |
| `Q` | Switch between the two carried weapons |
| `E` | Pick up a dropped weapon when standing near it; if both weapon slots are full, drop the current weapon |
| `Left Mouse Button` | Click menu buttons |
| `Esc` | Exit the game |

### Debug Controls

| Key | Function |
| --- | --- |
| `R` | Toggle collision debug display |
| `P` | Clear the current fight room for debugging |

## Documentation

- [PTSD framework guide (zh-TW)](docs/PTSD_FRAMEWORK_GUIDE.zh-TW.md)
