# gta4-web

A clean-room research project exploring how a large-scale open-world game engine can be designed for the browser using C++17, WebAssembly, and WebGL 2.

> This repository does **not** contain Rockstar Games source code, GTA IV assets, maps, audio, textures, models, or other proprietary game data.

## Milestone 0.1

The initial bootstrap provides:

- C++17 engine/application skeleton
- CMake + Emscripten build
- WebAssembly output for the browser
- WebGL 2 rendering test with an animated 3D cube
- Resize-aware HTML canvas
- GitHub Actions CI build
- Documentation and a roadmap for future engine systems

## Requirements

- Git
- CMake 3.20+
- Emscripten SDK (`emcc`, `emcmake`)
- A modern browser with WebGL 2 support

## Build

```bash
emcmake cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The generated web application is placed in `build/web/`.

Run it through a local HTTP server:

```bash
python3 -m http.server 8080 -d build/web
```

Then open `http://localhost:8080/`.

Convenience scripts are available at `scripts/build-web.sh` and `scripts/build-web.ps1`.

## Project layout

```text
engine/                 Future renderer, physics, audio, animation, streaming and vehicle systems
 game/                  Future gameplay systems
 web/                   Browser shell and future launcher/controls
 wasm/                  WebAssembly-specific integration
 tools/                 Asset/data tooling for legally usable project data
 docs/                   Architecture and roadmap documentation
 src/                    Initial C++ application
 include/                Public C++ headers
 scripts/               Build helpers
 .github/workflows/     Continuous integration
```

## Architecture roadmap

1. **Web bootstrap** — WebAssembly, WebGL 2, input and browser lifecycle.
2. **Engine foundation** — renderer abstraction, math, ECS/world model, resource lifetime and profiling.
3. **World streaming** — spatial partitioning, async resource loading and level-of-detail.
4. **Gameplay** — player controller, NPC simulation, vehicles, weapons, missions and wanted-system abstractions.
5. **Open-world systems** — traffic, pedestrians, time/weather, audio and streaming budgets.
6. **Browser optimization** — workers, memory tuning, compressed resources and eventual WebGPU backend exploration.

## Clean-room / legal boundary

The implementation is original code intended for engine research and browser technology experiments. Do not add or distribute proprietary Rockstar Games source code or copyrighted GTA IV game data. If you experiment with external game data, ensure you have the necessary rights and keep such data outside this repository.

## License

The original code in this repository is released under the MIT License. See `LICENSE`.
