# Roadmap

This project explores an original browser-based 3D engine using C++17, WebAssembly and WebGL 2. The repository already contains early math/camera, world, collision/physics and vehicle prototypes; the milestones below distinguish those prototypes from production-ready systems.

Estimates assume one developer at 8–12 focused hours per week.

## M0 — Browser and rendering bootstrap (implemented)
- [x] C++17 + CMake / Emscripten build
- [x] WebGL 2 rendering path and browser shell
- [x] Resize-aware canvas
**Exit evidence:** current build workflow and browser smoke test.

## M1 — Core engine vertical slice (partially implemented; 4–8 weeks)
- [x] Basic math, camera and transform primitives
- [x] Minimal world/entity model
- [x] AABB collision and simple rigid-body integration
- [x] Vehicle input/state prototype
- [ ] Unit tests for math, world, collision and vehicle behavior
- [ ] Stable fixed-step simulation and deterministic test scene
- [ ] Renderer/resource ownership and lifecycle rules
**Exit gate:** repeatable browser scene with tested movement, collision and camera behavior.

## M2 — World and resource streaming (2–4 months)
- [ ] Formal world-coordinate and sector format
- [ ] Resource manifest and async loading pipeline
- [ ] Spatial partitioning and bounded memory/streaming budgets
- [ ] LOD policy and diagnostics
**Exit gate:** synthetic world streams predictably without unbounded memory growth.

## M3 — Playable original vertical slice (3–6 months)
- [ ] Third-person player controller and interaction
- [ ] One small original test environment
- [ ] Collision and save/load for project-owned data
- [ ] Basic NPC state simulation
**Exit gate:** short, repeatable gameplay loop in original test content.

## M4 — Simulation systems (6–12 months)
- [ ] Vehicle handling validation and tuning
- [ ] Traffic/pedestrian scheduling prototype
- [ ] Mission state machine
- [ ] Audio and animation runtime
- [ ] Performance budgets for target browsers
**Exit gate:** selected systems run together in a bounded demonstration scene.

## M5 — Browser hardening (2–4 months initial pass; ongoing)
- [ ] Worker-based background jobs where beneficial
- [ ] Memory and allocation profiling
- [ ] Compressed resource pipeline
- [ ] Browser compatibility and performance matrix
- [ ] Production deployment and artifact-size budgets
- [ ] Evaluate WebGPU only after WebGL 2 path is stable
**Exit gate:** reproducible builds and documented performance envelope.

## Near-term schedule

| Period | Focus | Deliverable |
|---|---|---|
| Weeks 1–4 | M1 test coverage and simulation stability | Tested vehicle/collision scene |
| Weeks 5–8 | M1 renderer/resource lifecycle | Stable engine interfaces |
| Months 3–5 | M2 | Synthetic streamed world |
| Months 6–10 | M3 | Playable original vertical slice |
| Months 11–18 | M4–M5 | Selected simulation demo and browser hardening |

This is an engine prototype schedule, not a promise of commercial-game feature parity or compatibility. Re-estimate after each working vertical slice.
