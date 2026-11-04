# gta4-web

A clean-room research project exploring how an original large-scale 3D engine could be structured for browsers using C++17, WebAssembly, Emscripten and WebGL 2.

> This repository is an engine-technology experiment. It does not contain Rockstar Games source code or proprietary GTA IV assets, maps, audio, textures, models or other game data. It is not a GTA IV port and does not claim game compatibility.

## Current status

**Milestone 0 — browser bootstrap.** The initial scaffold includes:
- C++17 and CMake application skeleton.
- Emscripten/WebAssembly build.
- WebGL 2 rendering smoke test with an animated cube.
- Resize-aware browser canvas and shell.
- GitHub Actions build.

The renderer and gameplay systems described in the roadmap are planned, not implemented.

## Requirements

- Git
- CMake 3.20+
- Emscripten SDK (`emcc`, `emcmake`)
- A modern browser with WebGL 2

## Build

```bash
emcmake cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
python3 -m http.server 8080 -d build/web
```

Open `http://localhost:8080/`.

Convenience scripts are available in `scripts/`.

## Architecture and roadmap

The planned systems include renderer and resource abstractions, world streaming, entity simulation, vehicles, missions and browser performance tooling. See [docs/ROADMAP.md](docs/ROADMAP.md) for milestone gates and time estimates.

Estimates assume one developer at 8–12 focused hours per week and describe an engine prototype, not production-scale open-world content.

## Clean-room boundary

All implementation code must be independently authored. Do not copy proprietary source code or distribute copyrighted game data. Keep any external test data outside the repository unless it is clearly redistributable and appropriately licensed.

## License

Original project code is released under the MIT License. See [LICENSE](LICENSE).
