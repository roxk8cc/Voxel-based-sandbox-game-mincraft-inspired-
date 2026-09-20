# Blockverse — A Voxel Sandbox Game in C++

A Minecraft-inspired voxel sandbox built from scratch in C++ using [raylib](https://www.raylib.com/), featuring procedural terrain generation, a physically-simulated fluid system, and an entity/villager AI system.

> Featured as an official course showcase project on [The Programmer's Field Guide](https://programmers.guide/book/appendix/showcase/) (FIT1045, Monash University).

## Features

### 🌍 Procedural World Generation
- Custom **2D and 3D Perlin noise** implementation (fade curves, gradient functions, hashed pseudo-random coordinates) used to generate natural-looking terrain height and cave-like volume variation
- 100×50×100 voxel world stored in a flattened 1D array with fast 3D→1D index mapping
- Face-culling renderer — only exposed block faces (adjacent to air) are drawn, keeping the ~500,000-voxel world performant
- Procedural tree placement across generated terrain

### 💧 Fluid Simulation
- Custom water flow system built on **queue-based propagation** across the voxel grid (`FluidSimulator`)
- Tracks per-cell water level, source blocks, and pending updates using a `std::set` for scheduled simulation steps
- Timed refill logic (`REFILL_DELAY`) so water spreads and settles believably rather than updating instantly every frame

### 🧍 Entities & Villager AI
- Base `Entity` class with gravity, ground detection, and collision-aware movement shared across all creatures
- Villager state machine (`WANDER`, `FOLLOW_PLAYER`, `FLEE`) with flee-direction and flee-timer logic triggered by player proximity/aggression
- Hit-flash feedback system (`HIT_FLASH_DURATION`) for visual damage response

### 🎮 Player & Camera
- First-person camera and movement controller (`Camera3d`, `Player`) with configurable speed and mouse sensitivity
- Flying mode toggle, jump physics, and ground detection
- Block selection and placement/breaking

### 💾 Save / Load System
- Slot-based save system (`SaveSlot`) storing filename, display name, last-saved timestamp, and file size per slot
- Full game-state persistence (`SaveManager`) across `MAIN_MENU`, `PLAYING`, `PAUSE_MENU`, `SAVE_MENU`, and `LOAD_MENU` states

### 🖼️ Rendering & UI
- Textured block rendering (grass, dirt, stone, sand, wood, leaves, bedrock) with per-face UV mapping
- In-game menu system for save/load and pause flows

## Tech Stack

- **Language:** C++17
- **Graphics:** [raylib](https://www.raylib.com/) (immediate-mode OpenGL rendering)
- **Build:** Clang++, compiled and linked directly against raylib (see build command in `main.cpp`)

## Project Structure
main.cpp # Entry point
game.h / game.cpp # Core game loop and state management
world.h / world.cpp # Voxel world, terrain generation, Perlin noise
block.h / block.cpp # Block types, faces, textures
fluidsimulator.* # Queue-based water flow simulation
entity.h / entity.cpp# Base entity + villager AI
player.h / player.cpp# Player movement, camera, block selection
camera3d.* # First-person camera controller
savemanager.* # Save/load slot system
gamestate.h # Game state enum + save slot struct
menu.h / menu.cpp # Menu UI (main/pause/save/load)
vector3d.* # 3D vector math utilities
textures/ # Block textures (grass, dirt, stone, sand, wood, leaves, bedrock)


## Building

```bash
clang++ main.cpp vector3d.cpp player.cpp camera3d.cpp block.cpp world.cpp \
    game.cpp entity.cpp menu.cpp savemanager.cpp fluidsimulator.cpp \
    -o game \
    -I/opt/homebrew/include \
    -L/opt/homebrew/lib \
    -lraylib \
    -framework OpenGL -framework Cocoa -framework IOKit \
    -framework CoreAudio -framework CoreVideo \
    -std=c++17
```

*(Adjust include/library paths for your platform — the above assumes raylib installed via Homebrew on macOS.)*
