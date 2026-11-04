# gta4-web

A clean-room research project exploring an original browser-based 3D engine using C++17, WebAssembly, Emscripten and WebGL 2.

> This is an engine-technology experiment, not a GTA IV port. It contains no Rockstar Games source code or proprietary GTA IV game data. All runtime code and test content must be independently authored or properly licensed.

## Current status

**Early engine prototype (0.1.x).** The repository has progressed beyond the initial WebGL cube bootstrap. The current CMake target includes:

- C++17/Emscripten application and WebGL 2 rendering path.
- Vector/matrix math and a third-person camera.
- Basic entity/world and transform model.
- Sector and world-streaming scaffolding.
- AABB collision, rigid-body integration and a simple physics world.
- Vehicle state, input and a browser-driven vehicle demonstration.

Audio, animation, production-grade streaming, NPC/traffic simulation, missions, weapons and save/load are not yet established as complete systems. Subsystem README files describe planned areas and should not be interpreted as proof of implementation.

## Requirements

- Git
- CMake 3.20+
- Emscripten SDK (emcc, emcmake)
- A modern browser with WebGL 2

## Build

```bash
emcmake cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
python3 -m http.server 8080 -d build/web
```

Open http://localhost:8080/.

Convenience scripts are available in scripts/.

## Repository map

- src/ — application and browser rendering loop.
- engine/core/ — world/entity primitives and fixed timestep.
- engine/renderer/ — math, camera and renderer interfaces.
- engine/physics/ — collision and rigid-body prototype.
- engine/world/ — sectors and streaming prototype.
- engine/vehicles/ — vehicle dynamics and input.
- game/ — planned gameplay systems.
- web/ and wasm/ — browser shell and WebAssembly integration.
- docs/ROADMAP.md — staged plan, estimates and exit criteria.

## Roadmap and estimates

The near-term goal is a robust, testable vertical slice rather than broad feature claims. The roadmap assumes one developer at approximately 8–12 focused hours per week; production-scale open-world content is a separate multi-year effort.

## Clean-room boundary

Do not copy proprietary game code or distribute copyrighted game data. Keep external test data outside the repository unless it is clearly redistributable and appropriately licensed.

## License

Original project code is released under the MIT License. See LICENSE.
