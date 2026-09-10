# AGENTS.md

## Project: gta4web

`gta4web` is a clean-room, browser-based open-world engine research and reimplementation project.

The goal is to independently implement the kinds of technical systems required by a large-scale 3D open-world game using modern, original code and original project data.

The project is inspired by observable behavior and general engine architecture found in large open-world games. It is **not** an extraction, port, or reproduction of proprietary game source code or proprietary game data.

---

# 1. Core Mission

Build `gta4web` into a playable browser-based open-world engine prototype.

Primary technologies:

- C++17
- CMake
- Emscripten
- WebAssembly
- WebGL2
- HTML/JavaScript shell
- GitHub

The engine should eventually support:

- real-time 3D rendering
- entities and components
- world sectors
- streaming
- collision detection
- rigid-body physics
- vehicle physics
- vehicle drivetrain
- player controller
- third-person camera
- NPCs
- navigation
- animation
- weapons
- wanted/police systems
- missions
- audio
- environmental simulation
- browser-oriented performance optimization

Prefer a modular architecture that allows individual systems to be replaced or improved without rewriting the entire engine.

---

# 2. Clean-Room Requirement

This is a strict project requirement.

Do **not**:

- copy Rockstar source code
- copy GTA IV source code
- extract executable code and translate it into this project
- decompile proprietary code for direct reproduction
- copy proprietary game scripts
- copy proprietary game data
- extract proprietary maps
- extract proprietary models
- extract proprietary textures
- extract proprietary animations
- extract proprietary sounds
- extract proprietary physics data
- extract proprietary configuration files
- redistribute proprietary assets
- use leaked source code
- use leaked development documentation
- use proprietary reverse-engineering dumps as implementation material

Do not introduce Rockstar/GTA proprietary assets into the repository.

The implementation must be independently written.

It is acceptable to implement:

- generally known engine techniques
- standard physics equations
- standard graphics techniques
- standard vehicle dynamics
- ECS-style architectures
- spatial partitioning
- streaming systems
- navigation algorithms
- animation systems
- observable gameplay behavior
- independently designed approximations

When implementing behavior inspired by an existing game, reproduce the **behavioral concept**, not proprietary implementation details.

---

# 3. General Engineering Principles

Prioritize:

1. correctness
2. deterministic behavior
3. maintainability
4. modularity
5. testability
6. performance
7. visual fidelity

Do not optimize prematurely.

Do not introduce unnecessary dependencies.

Prefer small, composable systems over large monolithic classes.

Avoid global mutable state unless there is a strong architectural reason.

Keep simulation state separate from rendering state where practical.

Avoid allocating memory every frame.

Avoid unnecessary virtual dispatch inside hot loops.

Prefer contiguous data structures for performance-critical systems.

---

# 4. Repository Structure

The expected high-level structure is:

```text
gta4-web/
├── engine/
│   ├── animation/
│   ├── audio/
│   ├── core/
│   ├── physics/
│   ├── renderer/
│   ├── streaming/
│   ├── vehicles/
│   └── world/
├── game/
│   ├── missions/
│   ├── npc/
│   ├── player/
│   ├── weapons/
│   └── wanted/
├── web/
├── wasm/
├── tools/
├── docs/
├── scripts/
├── include/
└── CMakeLists.txt
```

Respect existing organization.

If a subsystem grows substantially, create appropriate subdirectories instead of turning one source file into an oversized implementation.

---

# 5. Before Making Changes

Before modifying code:

1. Inspect the current repository.
2. Inspect relevant existing source files.
3. Inspect recent Git history.
4. Read `README.md`.
5. Read `docs/ROADMAP.md`.
6. Read `docs/PROGRESS.md` if present.
7. Determine which systems are already implemented.
8. Determine whether the requested feature already partially exists.
9. Avoid duplicating existing functionality.
10. Identify dependencies between systems.

Do not assume the repository is in the state described by an old task or conversation.

The repository itself is the source of truth.

---

# 6. Autonomous Development

Codex should work autonomously whenever the task is sufficiently specified.

Do not stop merely because a feature requires several implementation steps.

Break large tasks into logical milestones and complete as many as can safely be completed.

For example:

```text
Analyze
  ↓
Design
  ↓
Implement
  ↓
Compile
  ↓
Test
  ↓
Fix
  ↓
Document
  ↓
Commit
  ↓
Next milestone
```

Ask the user only when:

- a product decision cannot reasonably be inferred
- destructive action is required
- required proprietary material would be needed
- credentials or external authorization are required
- there are multiple materially different architectural choices with no sensible default
- the task conflicts with the clean-room requirements

Otherwise choose a reasonable implementation and continue.

---

# 7. Long-Running Tasks

For long-running development tasks:

1. Work in small coherent increments.
2. Keep each increment buildable when practical.
3. Commit completed milestones.
4. Update project documentation.
5. Record unresolved issues.
6. Continue with the next logical milestone.

Do not create one enormous unreviewable commit for an entire subsystem.

Preferred commit style:

```text
feat: add vehicle transmission state machine
feat: implement reverse drivetrain torque
fix: stabilize low-speed wheel slip
feat: add player vehicle interaction
refactor: separate tire forces from drivetrain
 test: add vehicle drivetrain regression cases
docs: update vehicle physics progress
```

---

# 8. Progress Tracking

Maintain:

`docs/ROADMAP.md`

for high-level milestones.

Maintain:

`docs/PROGRESS.md`

for implementation status when the project becomes large enough to require it.

`PROGRESS.md` should contain:

```text
# Current Status

## Completed
- ...

## In Progress
- ...

## Next
- ...

## Known Issues
- ...

## Build Status
- ...

## Test Status
- ...
```

Keep these files concise and factual.

Never claim a feature is complete if it has not been implemented and verified.

---

# 9. Build Requirements

The primary target is:

`Emscripten + WebAssembly + WebGL2`

The project should continue to build using the documented CMake/Emscripten workflow.

Expected build pattern:

```bash
emcmake cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Do not introduce native-only functionality into the core engine unless there is a clear abstraction allowing the browser build to remain functional.

---

# 10. Compiler Quality

Use strict compiler diagnostics where practical.

C++ code should generally compile cleanly under:

- `-Wall`
- `-Wextra`
- `-Wpedantic`

Do not leave new warnings unexplained.

Avoid undefined behavior.

Prefer explicit initialization.

Use `const` wherever appropriate.

Use references instead of unnecessary copies.

Use `std::array` for fixed-size collections.

Use `std::vector` for dynamic collections.

Avoid raw owning pointers.

Use RAII.

---

# 11. Engine Core

The engine core should remain independent of gameplay-specific code.

Core responsibilities may include:

- time
- fixed timestep
- entities
- transforms
- world
- events
- system scheduling
- simulation state

Avoid placing vehicle, NPC, weapon, or mission-specific behavior directly into the core.

---

# 12. Simulation and Fixed Timestep

Physics and gameplay simulation should use a fixed timestep where practical.

Rendering may run at a different frame rate.

Prefer:

```text
input
  ↓
fixed simulation
  ↓
physics
  ↓
gameplay
  ↓
render interpolation
  ↓
render
```

Avoid making physics behavior dependent on variable rendering frame time.

Clamp unreasonable timestep values.

Prevent numerical explosions after browser stalls or tab suspension.

---

# 13. Physics

Physics should be deterministic and numerically stable.

Prioritize:

- collision detection
- contact resolution
- rigid-body integration
- gravity
- friction
- suspension
- wheel contacts
- vehicle forces

Avoid creating a fake visual-only physics system once real simulation has begun.

Approximate models are acceptable, but their behavior should be physically coherent.

Document significant simplifications.

---

# 14. Vehicle Physics

Vehicle physics is a major subsystem.

The long-term target is a believable four-wheel vehicle model.

The vehicle architecture should support:

- chassis mass
- center of mass
- inertia
- 3D orientation
- pitch
- roll
- yaw
- yaw rate
- pitch rate
- roll rate
- suspension
- wheel colliders
- wheel angular velocity
- tire forces
- longitudinal slip
- lateral slip
- steering
- braking
- handbrake
- traction
- load transfer
- anti-roll bars

Do not reduce the vehicle to a simple forward acceleration equation once more advanced systems exist.

---

# 15. Vehicle Drivetrain

The drivetrain should be modeled independently enough that it can evolve without rewriting the chassis.

The architecture should support:

```text
Input
  ↓
Transmission
  ↓
Clutch / coupling
  ↓
Differential
  ↓
Driven wheels
  ↓
Tire forces
  ↓
Vehicle chassis
```

Where appropriate, support:

- engine state
- engine RPM
- idle RPM
- redline RPM
- engine torque curve
- transmission gears
- neutral
- reverse
- gear ratios
- final drive
- differential
- differential lock
- drivetrain losses
- wheel torque
- wheel angular velocity

Reverse should be a genuine drivetrain state rather than simply applying an artificial braking force.

---

# 16. Transmission State

Use an explicit transmission state.

Conceptually:

```text
Reverse = -1
Neutral  =  0
Drive    = positive gear index
```

The exact implementation may differ if a better architecture is appropriate.

Direction changes should be safe.

Do not instantly switch from high forward speed to reverse torque.

Require the vehicle to slow sufficiently before changing direction, unless an explicit arcade mode is later introduced.

---

# 17. Differential

Differential behavior should account for differences between left and right driven wheels.

The implementation should eventually support:

- open differential behavior
- configurable locking
- torque distribution
- wheel-speed differences
- traction limitations

Avoid simply assigning identical force to both wheels regardless of wheel state.

---

# 18. Wheel Colliders

Each wheel should have independent state where practical.

Wheel state may include:

- contact
- suspension length
- suspension velocity
- normal load
- steer angle
- angular velocity
- slip ratio
- slip angle
- contact position

Wheel forces should feed back into the chassis.

---

# 19. Suspension

Suspension should support:

- rest length
- travel
- spring force
- damping
- compression
- wheel contact
- load calculation

Anti-roll bars should transfer load between left and right wheels on the same axle.

Avoid directly setting vehicle pitch/roll solely from steering input.

Pitch and roll should emerge from physical loads where practical.

---

# 20. Rendering

Renderer architecture should remain independent of gameplay logic.

Support progressively:

- camera
- meshes
- materials
- transforms
- lighting
- shadows
- batching
- instancing
- culling
- debug rendering

The renderer should not know what a mission or wanted level is.

---

# 21. Third-Person Camera

The camera should eventually support:

- follow target
- distance
- height
- look-ahead
- smoothing
- collision avoidance
- vehicle-relative orientation
- player-relative orientation
- camera shake
- pitch limits
- yaw limits

Avoid hardcoding camera behavior into the vehicle itself.

The vehicle should expose state; the camera should interpret it.

---

# 22. World and Streaming

The world should be divided into manageable spatial regions.

Streaming should support:

- sector lookup
- loading
- unloading
- activation
- deactivation
- streaming radius
- prioritization

Do not assume the entire future open world must remain resident in memory.

Design APIs so that world content can eventually be streamed asynchronously.

---

# 23. Entities

Entities should remain lightweight.

Avoid turning every entity into a huge inheritance hierarchy.

Prefer composition where appropriate.

Potential components:

```text
Transform
Renderable
RigidBody
Collider
Vehicle
Character
CameraTarget
AudioEmitter
NavigationAgent
Weapon
Health
MissionState
```

---

# 24. NPC Architecture

NPCs should eventually support:

- perception
- state machines
- navigation
- locomotion
- animation
- combat
- traffic behavior
- reactions
- scheduling

Do not implement every NPC behavior inside one giant update function.

Prefer modular state/behavior systems.

---

# 25. Navigation

Navigation should be independent from individual NPC behavior.

Potential systems:

- navigation mesh
- graph navigation
- pathfinding
- local avoidance
- traffic lanes
- pedestrian navigation

Start simple and expand incrementally.

---

# 26. Animation

Animation should eventually support:

- skeletal animation
- animation clips
- blending
- state machines
- locomotion
- upper/lower-body separation
- inverse kinematics where useful

Do not couple animation directly to renderer implementation.

---

# 27. Gameplay

Gameplay systems should remain separate from low-level engine systems.

Gameplay may include:

- player
- weapons
- missions
- wanted system
- police
- vehicles
- NPC interactions

Engine code should expose reusable primitives rather than GTA-specific hardcoding.

---

# 28. Missions

Mission runtime should eventually support data-driven logic.

Prefer:

```text
Mission
  → objectives
  → triggers
  → conditions
  → actions
  → state transitions
```

Avoid embedding an entire mission inside one giant C++ function.

---

# 29. Wanted / Police

The wanted system should be implemented as an independent gameplay subsystem.

Potential state:

```text
wanted level
crime events
police awareness
search state
pursuit state
cooldown
```

Police AI should consume world/gameplay events rather than being hardwired into the player controller.

---

# 30. Input

Input should be separated from simulation.

Conceptually:

```text
Browser Input
    ↓
Input State
    ↓
Gameplay Commands
    ↓
Simulation
```

Do not make physics depend directly on DOM events.

Normalize analog inputs.

Handle keyboard press/release correctly.

Avoid frame-dependent input behavior.

---

# 31. Browser Integration

The browser layer should remain thin.

JavaScript/HTML should primarily handle:

- canvas
- browser events
- startup
- UI
- input
- loading
- Web APIs

Core gameplay should remain in C++ where practical.

Do not move engine logic into JavaScript merely because it is easier in the short term.

---

# 32. Performance

Because the target is WebAssembly/WebGL2:

Avoid:

- per-frame heap allocations
- unnecessary string creation
- excessive logging
- large temporary object graphs
- expensive virtual calls in tight loops
- unnecessary GPU state changes

Prefer:

- data locality
- batching
- spatial culling
- fixed-size buffers
- object reuse
- predictable update loops

Do not optimize based on assumptions.

When performance becomes important, add measurements or profiling hooks.

---

# 33. Testing

Add tests for non-visual systems whenever practical.

Important candidates:

- math
- transforms
- collision
- physics
- fixed timestep
- sector lookup
- streaming
- drivetrain
- transmission
- differential
- wheel calculations
- input mapping

Physics regression tests should cover:

- stationary vehicle
- acceleration
- braking
- steering
- reverse
- forward/reverse transition
- wheel contact
- suspension
- differential behavior

---

# 34. Debug Instrumentation

For difficult systems, add optional debug information.

Useful vehicle debug values:

```text
speed
engine RPM
gear
throttle
brake
steering
wheel RPM
wheel slip
wheel load
suspension compression
yaw rate
pitch
roll
```

Useful physics debug rendering:

- collider shapes
- contact points
- contact normals
- wheel positions
- suspension rays
- center of mass

Debug systems must not become required for release rendering.

---

# 35. Error Handling

Fail clearly.

Do not silently ignore invalid state transitions.

For recoverable runtime conditions, prefer safe fallback behavior.

For impossible internal states, use assertions or explicit diagnostics where appropriate.

Avoid undefined behavior as a form of error handling.

---

# 36. Documentation

Update documentation when architecture changes.

Document:

- public APIs
- non-obvious algorithms
- important assumptions
- simplifications
- coordinate conventions
- units
- state machines

For physics:

- distances are meters unless explicitly stated
- time is seconds
- mass is kilograms
- force is Newtons
- velocity is meters/second
- angular quantities use radians unless explicitly stated

---

# 37. Coordinate System

Maintain a consistent world coordinate system.

The current project convention should be verified from existing math/renderer code before changing it.

Do not silently change:

- handedness
- forward axis
- up axis
- rotation conventions

If a coordinate convention must change, migrate the entire affected subsystem and document it.

---

# 38. Units

Use SI units internally wherever practical.

Preferred:

```text
distance → meters
time → seconds
mass → kilograms
force → Newtons
velocity → meters/second
acceleration → meters/second²
angles → radians
angular velocity → radians/second
```

Conversions should happen at external interfaces.

---

# 39. Git Workflow

Never overwrite unrelated user work.

Before editing, conceptually verify that the current repository state is understood.

Keep commits focused.

Do not amend existing commits unless explicitly requested.

Do not rewrite shared history.

Do not force-push.

Do not delete branches or tags without explicit authorization.

Use descriptive commit messages.

---

# 40. Commit Requirements

When a meaningful milestone is complete:

1. Build.
2. Run relevant tests.
3. Inspect changed files.
4. Review for accidental changes.
5. Update documentation.
6. Commit.

A commit should describe what changed.

Example:

```text
feat: add vehicle transmission and reverse drivetrain
```

Not:

```text
update stuff
```

---

# 41. CI

CI results must be reported accurately.

Never claim CI passed unless an actual CI run exists and succeeded.

If no workflow run exists, state that no workflow run was reported.

If CI fails:

- inspect the failure
- fix it if possible
- rerun
- report the final status

Never fabricate test or CI results.

---

# 42. Generated Files

Do not commit generated build directories unless explicitly required.

Do not commit:

```text
build/
node_modules/
temporary files
local caches
compiler output
```

unless repository configuration explicitly requires them.

---

# 43. Dependencies

Before adding a dependency:

1. Determine whether the standard library is sufficient.
2. Determine whether the dependency supports Emscripten/WebAssembly.
3. Consider bundle size.
4. Consider maintenance.
5. Consider license compatibility.
6. Consider whether the dependency is actually necessary.

Prefer existing project infrastructure.

---

# 44. Security

Never commit:

- API keys
- access tokens
- passwords
- private credentials
- GitHub tokens
- cloud credentials
- personal secrets

Do not print secrets into logs.

Do not add secret values to configuration files.

---

# 45. Handling Uncertainty

If implementation details are uncertain:

1. Prefer established engineering approaches.
2. Prefer the simplest extensible solution.
3. Document assumptions.
4. Avoid pretending to know proprietary internals.
5. Do not invent claims about original engine implementation.

Use language such as:

```text
This is an independent approximation.
```

rather than claiming:

```text
This is how the original engine implemented it.
```

---

# 46. Feature Development Pattern

For substantial features, use this pattern:

## Step 1 — Architecture

Determine:

- ownership
- state
- APIs
- update order
- dependencies

## Step 2 — Minimal Implementation

Implement the smallest coherent version.

## Step 3 — Integration

Connect it to existing systems.

## Step 4 — Verification

Build and test.

## Step 5 — Refinement

Improve stability and behavior.

## Step 6 — Documentation

Update relevant docs.

## Step 7 — Commit

Create a focused commit.

---

# 47. Do Not Rewrite Working Systems Without Reason

Existing systems may be incomplete, but avoid replacing them purely because a different implementation looks cleaner.

Before rewriting:

- understand current behavior
- identify actual limitations
- preserve compatible APIs where reasonable
- migrate incrementally

Prefer:

```text
extend → test → refactor
```

over:

```text
delete → rewrite everything
```

---

# 48. Current Vehicle Development Direction

The vehicle system is an important current development area.

The intended evolution is:

```text
Vehicle Input
      ↓
Transmission
      ↓
Engine
      ↓
Drivetrain
      ↓
Differential
      ↓
Wheel Torque
      ↓
Wheel/Tire Model
      ↓
Suspension
      ↓
Chassis Forces
      ↓
Vehicle Transform
      ↓
Camera / Renderer
```

Do not collapse these layers unnecessarily.

The vehicle should eventually be capable of believable:

- acceleration
- deceleration
- steering
- weight transfer
- wheel slip
- traction loss
- reverse driving
- gear changes
- differential behavior
- body pitch
- body roll

---

# 49. Current Development Priority

Unless the user explicitly specifies another priority, continue roughly in this order:

### Phase 1 — Engine Foundation

- math
- transforms
- entities
- timing
- renderer
- camera

### Phase 2 — Physics

- collision
- rigid bodies
- contacts
- fixed timestep
- debug visualization

### Phase 3 — Vehicles

- chassis
- wheel colliders
- suspension
- tire forces
- steering
- brakes
- anti-roll
- pitch/roll
- drivetrain
- transmission
- differential

### Phase 4 — Player

- character controller
- third-person camera
- vehicle interaction
- enter/exit vehicles

### Phase 5 — World

- sectors
- streaming
- procedural/test world
- spatial queries

### Phase 6 — NPCs

- navigation
- perception
- behavior
- animation

### Phase 7 — Gameplay

- weapons
- missions
- wanted
- police
- traffic

### Phase 8 — Polish

- rendering quality
- lighting
- shadows
- audio
- optimization
- loading
- browser UX

---

# 50. Definition of Done

A feature is not considered complete merely because its code compiles.

A feature should be considered complete when:

- implementation exists
- integration exists
- relevant tests exist where practical
- build succeeds
- no new compiler warnings remain
- documentation is updated where appropriate
- obvious regressions are addressed
- Git history contains a meaningful commit

For visual features, compilation alone is insufficient evidence.

For physics features, numerical stability should also be considered.

For browser features, the WebAssembly/WebGL2 target must remain valid.

---

# 51. When the User Says "Continue"

Interpret:

```text
continue
weiter
mach weiter
nächster Schritt
weiterentwickeln
```

as permission to inspect the current repository state and proceed with the next sensible milestone.

Do not ask for a redundant confirmation.

First inspect:

```text
README.md
docs/ROADMAP.md
docs/PROGRESS.md
recent commits
relevant source files
```

Then continue.

---

# 52. When the User Gives a Large Goal

For a goal such as:

```text
make the vehicle system realistic
```

do not attempt to implement every conceivable feature blindly.

Instead:

1. inspect the current implementation
2. identify the highest-value missing subsystem
3. implement it
4. verify it
5. commit it
6. continue to the next logical subsystem

Maintain a coherent progression.

---

# 53. Avoid Fake Completion

Never mark something complete just because:

- a class exists
- a method exists
- a TODO was removed
- code compiles
- a placeholder renders
- a variable was added

The system must actually participate in runtime behavior where applicable.

---

# 54. Prototype vs Production

It is acceptable for early implementations to be simplified.

When doing so:

- keep the API extensible
- avoid locking the project into the approximation
- document the limitation
- avoid unnecessary premature complexity

Example:

A simple planar tire model is acceptable before a more sophisticated combined-slip model exists.

A hardcoded test vehicle is acceptable before a data-driven vehicle definition system exists.

A simple sector loader is acceptable before asynchronous streaming exists.

---

# 55. Browser-First Philosophy

The project is not a native desktop engine with a browser wrapper.

The browser is a primary target.

Design with:

- WebAssembly memory constraints
- WebGL2 capabilities
- browser scheduling
- tab suspension
- input events
- loading latency
- asset streaming
- GPU limitations

in mind.

---

# 56. Final Rule

**Always leave the repository in a better, buildable, understandable state than you found it.**

Make progress.

Keep changes reversible.

Keep the architecture clean.

Keep the implementation independent.

Test what you can.

Document what you cannot yet verify.

Commit coherent milestones.

Continue toward the next meaningful system.
