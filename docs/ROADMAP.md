# Roadmap

The project explores an original browser-based open-world engine architecture using C++17, WebAssembly and WebGL 2. It does not include or reproduce proprietary game code or assets.

Estimates assume one developer at roughly 8–12 focused hours per week. A full open-world game is far beyond the current bootstrap and is not implied by these milestones.

## M0 — Browser bootstrap (complete)
- [x] C++17 + CMake project
- [x] Emscripten/WebAssembly build
- [x] WebGL 2 rendering smoke test
- [x] Browser shell and resize handling
- [x] GitHub Actions build

**Exit gate:** browser loads the build and renders the smoke-test scene.

## M1 — Engine foundation (4–8 weeks)
- [ ] Math library, transforms and camera
- [ ] Renderer abstraction and resource lifetime
- [ ] Input abstraction and frame timing
- [ ] Minimal world/entity model
- [ ] Debug overlay and profiling counters

**Exit gate:** a deterministic test scene renders with input and stable frame timing.

## M2 — World representation and streaming prototype (2–4 months)
- [ ] Spatial partitioning and world-coordinate conventions
- [ ] Resource manifest and asynchronous loading
- [ ] Basic LOD and streaming budget
- [ ] Collision query prototype
- [ ] Memory and loading diagnostics

**Exit gate:** a synthetic test world streams in/out without unbounded memory growth.

## M3 — Playable vertical slice (3–6 months)
- [ ] Third-person camera and player controller
- [ ] Basic collision and interaction
- [ ] One small original test environment
- [ ] Minimal NPC state simulation
- [ ] Save/load of project-owned test data

**Exit gate:** a short, repeatable playable loop in an original test scene.

## M4 — Simulation systems (6–12 months)
- [ ] Vehicle movement prototype
- [ ] Traffic/pedestrian scheduling experiments
- [ ] Mission state machine
- [ ] Audio runtime and time/weather model
- [ ] Performance budgets for browser targets

**Exit gate:** selected systems run together in a bounded demonstration scene.

## M5 — Browser hardening (ongoing; 2–4 months initial pass)
- [ ] Worker-based background jobs where beneficial
- [ ] Memory and allocation profiling
- [ ] Compressed resource pipeline
- [ ] Browser compatibility matrix
- [ ] Deployment and artifact-size budgets
- [ ] Evaluate WebGPU only after WebGL 2 path is stable

**Exit gate:** reproducible browser builds and documented performance envelope.

## Near-term schedule

| Period | Focus | Deliverable |
|---|---|---|
| Weeks 1–4 | M1 math, camera, input | Interactive 3D test scene |
| Weeks 5–8 | M1 renderer/resource interfaces | Stable rendering foundation |
| Months 3–5 | M2 | Streaming synthetic world |
| Months 6–10 | M3 | Playable original vertical slice |
| Months 11–18 | M4–M5 | Selected simulation demo and browser hardening |

This schedule is deliberately for an engine prototype. Production-scale open-world content, mission parity with any commercial game, and full asset compatibility are out of scope unless separately specified and legally cleared.
