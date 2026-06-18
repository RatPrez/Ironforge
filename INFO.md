# OSRS Clone

A from-scratch OSRS-inspired game built in C++.

## Stack

| Layer | Library |
|-------|---------|
| Networking | [GameNetworkingSockets](https://github.com/ValveSoftware/GameNetworkingSockets) |
| ECS | [EnTT](https://github.com/skypjack/entt) |
| Rendering / Window / Input / Audio | [Raylib](https://www.raylib.com) |

## Dependencies

### GameNetworkingSockets

```bash
git clone https://github.com/ValveSoftware/GameNetworkingSockets
sudo dnf install protobuf-devel protobuf-compiler
cmake -B gns_build -DCMAKE_INSTALL_PREFIX=/opt/gns GameNetworkingSockets
sudo cmake --build gns_build --target install -j$(nproc)
rm -rf GameNetworkingSockets gns_build
```

### EnTT

Header-only — add via CMake FetchContent or drop the single header into `vendor/`.

### Raylib

```bash
sudo dnf install raylib-devel
```

Or build from source if you need the latest version.

## Building

```bash
./build.sh
```

Outputs:
- `build/server/server`
- `build/client/client`

A `compile_commands.json` symlink is created in the project root for IDE integration (Zed, clangd).

## Running

```bash
# terminal 1
./run_server.sh

# terminal 2
./run_client.sh [host]   # host defaults to 127.0.0.1
```

## Project Structure

```
osrs_clone/
├── client/         # game client
├── server/         # game server
└── shared/         # headers shared between client and server
```

## Architecture Notes

- Server is authoritative — clients send inputs, server owns all game state
- Networking is transport only (GNS) — game logic is decoupled from it
- ECS systems will handle send/receive as dedicated systems reading dirty-tagged components
- Rendering is a system that reads ECS state — swappable independently of game logic

## EnTT Basics

Create a registry, define components as plain structs, attach them to entities and query them with views:

```cpp
#include <entt/entt.hpp>

struct Position { float x, y; };
struct Health   { int current, max; };

entt::registry reg;

// create an entity and attach components
entt::entity player = reg.create();
reg.emplace<Position>(player, 0.0f, 0.0f);
reg.emplace<Health>(player, 100, 100);

// query all entities with both Position and Health
auto view = reg.view<Position, Health>();
for (auto entity : view) {
    auto &pos = view.get<Position>(entity);
    auto &hp  = view.get<Health>(entity);
    // ...
}

// tag component — zero size, presence is the flag
struct Me {};
reg.emplace<Me>(player);

// single entity query
auto me = reg.view<Me, Position>().front();

// update via patch so observers are notified
reg.patch<Position>(me, [](Position &p) { p.x += 1.0f; });

// remove a component
reg.remove<Health>(player);

// destroy entity and all its components
reg.destroy(player);
```

## Server Tick Loop

The server runs on a fixed 600ms tick (matching OSRS). Each tick:

```
receive → simulate → send → sleep(600ms - elapsed)
```

```cpp
while (running) {
    auto tick_start = now();

    network_receive_system(registry, net_ctx);  // drain GNS, write input components
    input_system(registry);                      // process PlayerInput components
    movement_system(registry);                   // update Transform from input
    combat_system(registry);                     // etc.
    network_send_system(registry, net_ctx);      // read dirty components, send state

    auto elapsed = now() - tick_start;
    sleep(600ms - elapsed);                      // burn remaining tick budget
}
```

- Receive first so simulation always has the freshest client inputs
- Send last so clients receive the result of this tick, not last tick
- GNS buffers incoming packets — nothing is lost while systems are running
- Log a warning if elapsed > 600ms — means ticks are being missed

## Position and Movement

OSRS positions are integer tiles — the server sends:

```cpp
uint16_t x;
uint16_t y;
uint16_t plane;  // floor level
```

6 bytes per entity. The client holds two positions:
- **Accurate** — real tile from the server, used for all game logic (combat range, interactions)
- **Visual** — where the model currently is, lerped smoothly toward accurate position

The server never knows or cares about the visual position. At 600ms per tick the lerp completes well before the next tick arrives, making jitter essentially irrelevant.

## Network Entity Architecture

### Entity ID Space

All entity IDs are `uint32_t`, split into two non-overlapping namespaces:

```cpp
constexpr EntityID LOCAL_ID_BASE = static_cast<uint32_t>(INT32_MAX) + 1u;

EntityID nextNetworkId = 1;            // server assigns these
EntityID nextLocalId   = LOCAL_ID_BASE; // client assigns these
```

- `[1 → 2³¹-1]` — server-assigned networked entities (players, NPCs, objects)
- `[2³¹ → 2³²-1]` — client-only entities (UI, particles, camera) — never sent over the wire

The server is the sole authority on network IDs. The client never assigns into the server namespace.

### Two Separate ECS Worlds

Server and client maintain independent ECS registries. The server holds only logical gameplay components. The client holds a superset — logical state received from the server plus visual/rendering components.

| Server components | Client components |
|---|---|
| Position (tile x,y) | RenderPosition (float3) |
| Health | Health (UI only) |
| Path | AnimationState |
| CombatState | NameTag |
| Inventory | Me |
| AggroTable | Camera, Particles |

### Components

Everything that can fight shares a single `CombatStats` component — both players and NPCs:

```cpp
struct CombatStats {
    uint16_t hitpoints;
    uint16_t max_hitpoints;
    uint16_t attack;
    uint16_t strength;
    uint16_t defence;
    uint16_t magic;
    uint16_t ranged;
    uint16_t prayer;
};
```

Player-only skills (Woodcutting, Fishing, Cooking etc.) live in a separate component that NPCs never carry:

```cpp
struct Skills {
    uint16_t woodcutting;
    uint16_t fishing;
    uint16_t cooking;
    // ...
};
```

### Self Entity and Login Handshake

The server assigns the client its own entity ID at login:

```cpp
struct LoginResponse {
    EntityID selfId;    // "you are entity 42"
    int spawnX, spawnY;
};
```

The client marks it with a zero-size `Me` tag component:

```cpp
registry.emplace<Me>(selfId);
```

Any system that needs the local player just queries for `Me`. Since only one entity ever carries it, `front()` is always correct:

```cpp
// camera system
auto me = reg.view<Me, Transform>().front();

// UI system — read own stats
auto me = reg.view<Me, CombatStats, Skills>().front();
```

No globals, no passed handles — systems are fully self contained. Private data packets (inventory, XP etc.) don't include an entity ID, the recipient is implicit and the client applies them directly to the `Me` entity.

### Interest Management

The server tracks all entities but each player connection only receives entities within view range (~15 tiles). Each tick the server diffs each player's tracked set and sends `SpawnPacket`, `UpdatePacket`, or `DespawnPacket` accordingly. Private data only ever travels down the owning connection's pipe — other clients never receive it.

## Actor and Rendering Pipeline

The server sends `Actor` and `Transform`. The client owns everything visual from there — no rendering logic lives on the server.

```cpp
// shared — server sends this
struct Actor {
    uint32_t    model_id;   // maps to a .glb filename
    ActorState  state;      // Idle, Moving, Attacking, Dead
};

enum class ActorState : uint8_t {
    Idle,
    Moving,
    Attacking,
    Dead
};

// client only — added when Actor is first seen
struct Renderer {
    ModelId  model;
    bool     loaded;
};

// client only — added automatically if model has animations
struct Animator {
    int   current_anim;
    float frame;
    float speed;
};
```

If an entity has no `Actor` component it is invisible and simulation only — no `Renderer` or `Animator` is ever added. Server can spawn client-side entities without a visual representation simply by not attaching `Actor`.

The client spawn flow:
```
server sends Actor + Transform
→ SpawnSystem adds Renderer, queues model load in AssetManager
→ model finishes loading
  → if model has animations, add Animator
→ AnimatorSystem reads Actor.state, drives Animator frame
→ RenderSystem draws model at RenderPosition
```

`Actor.state` is just an enum — a single byte. Server says "Moving", client decides which animation that maps to. Server never knows or cares about animations.

## Position, Heading and Visual Interpolation

### Authoritative Position

OSRS characters don't have freeform rotation — just one of 8 directions. Position and heading are compact:

```cpp
// shared — server authoritative
struct Position {
    uint16_t x, y, plane;
    uint8_t  heading;  // 0-7 (N, NE, E, SE, S, SW, W, NW)
};
```

7 bytes per entity. No quaternions, no euler angles — heading snaps or lerps to one of 8 fixed rotations on the client.

### Visual Interpolation (Snapshot Interpolation)

The client maintains a separate visual position that smoothly lerps toward the authoritative position each frame:

```cpp
// client only
struct RenderPosition {
    float x, y, z;  // lerping toward Position this frame
};
```

`RenderSystem` reads `RenderPosition` only — never `Position` directly. Each frame `RenderPosition` is nudged toward `Position` by a lerp factor. The server never knows or cares about `RenderPosition`.

### Stale Packet Rejection

Transform updates are stamped with the server tick. The client tracks the last accepted tick per entity and silently drops anything older:

```cpp
struct TransformUpdate {
    MessageHeader header;
    EntityID      entity_id;
    uint32_t      tick;
    uint16_t      x, y, plane;
    uint8_t       heading;
};
```

```cpp
if (update.tick > last_tick[entity]) {
    apply_transform(update);
    last_tick[entity] = update.tick;
}
```

Out of order packet arrives late — one integer comparison, silently dropped. No lerp glitch, no position snapping backwards.

### Network Lanes for Reliable Traffic

Lanes give independent reliable ordered streams — a stall in one lane doesn't block others:

| Lane | Use |
|------|-----|
| 0 | Critical game state — spawn/despawn, combat events, deaths |
| 1 | Stats/UI — health changes, XP drops |
| 2 | Chat, misc |

Transform updates are sent **unreliable** and bypass lanes entirely — by the time a retransmit arrives the next position update has already superseded it. Fire and forget, client lerps over any gaps.

## ECS Change Tracking

EnTT has built-in change detection via observers — no dirty components needed.

```cpp
struct NetworkObservers {
    entt::observer transform;
    entt::observer health;

    NetworkObservers(entt::registry &reg) {
        transform.connect(reg, entt::collector.update<Transform>());
        health.connect(reg, entt::collector.update<Health>());
    }
};
```

The network send system drains each observer and clears it each tick — only entities that actually changed are processed.

For a component change to be visible to an observer you must use `patch` instead of a direct ref write:

```cpp
// observer sees this
reg.patch<Transform>(entity, [](Transform &t) {
    t.pos.x = 5;
});

// observer does NOT see this
auto &t = reg.get<Transform>(entity);
t.pos.x = 5;
```

`patch` is synchronous — fetches the ref, runs the lambda, fires the signal, pushes the entity into the observer's sparse set. All inline, O(1), no overhead when nothing is listening. Systems that don't need change tracking just write the ref directly.

Adding a new synced component is one line in `NetworkObservers` and one drain in the send system.

## Client Input / Requests

Clients send intent, server decides what happens. Request components are placed on the player entity by input handling code and consumed by the network send system:

```cpp
reg.emplace_or_replace<MoveRequest>(player_entity, tile_x, tile_y);
reg.emplace_or_replace<InteractRequest>(player_entity, target_entity);
```

Network send system queries for request components, serializes and sends them, then removes them. Server validates everything — runs A* itself, checks interaction range — and never trusts client input blindly.

## Client Render Pipeline

The client loop mirrors the server but with a render system instead of a send system:

```
receive → simulate → render → sync to framerate
```

The render system is just another ECS system — reads components, calls Raylib draw calls, knows nothing about networking or game logic.

### Asset Manager

The render system never loads assets directly. It asks an asset manager which handles background loading:

```
render system asks for model
→ not loaded yet: queue it, return nullptr, skip entity silently this tick
→ loading: return nullptr, skip again
→ ready: return model, draw it
```

Models are just a `ModelId` component on an entity. Swapping cubes for real models later is a one line change.

### World Rendering

The world is not a Blender export — it's a procedural mesh generated from tile data at runtime:

```cpp
struct Tile {
    int16_t  height;   // terrain height at this tile
    uint8_t  overlay;  // texture id
    uint8_t  flags;    // walkability, blocked edges
};
```

At load time all tiles in a region are stitched into one big mesh and uploaded to the GPU as a single draw call. Heights at tile corners are interpolated to produce smooth terrain. When the player moves far enough, the next region loads and the old one unloads — OSRS uses 64x64 tile chunks.

3D objects (buildings, trees, rocks) are separate meshes placed at world positions on top of the terrain. They are not part of the terrain mesh.

### Collision and Walkability

Each tile holds a bitmask of blocked edges:

```cpp
const uint8_t BLOCK_NORTH = 1 << 0;
const uint8_t BLOCK_EAST  = 1 << 1;
const uint8_t BLOCK_SOUTH = 1 << 2;
const uint8_t BLOCK_WEST  = 1 << 3;
const uint8_t BLOCK_FULL  = 1 << 4;
```

A wall between two tiles is just both tiles flagging their shared edge. A door is toggling a flag at runtime. No physics engine, no collider meshes — just bits.

A* pathfinding checks these flags when evaluating neighbours. Diagonal movement checks that both the horizontal and vertical edges are clear so you can't clip through corners.

Tile data lives in `shared/` — both server and client load the same file. Server uses it for authoritative pathfinding and movement validation, client uses it for local prediction and terrain mesh generation.

## World Editor

Artists don't edit meshes — they use an editor tool that writes tile data:

- Paint tile heights
- Paint texture overlays
- Flag blocked edges (walls, doors)
- Place meshes at tile positions with rotation
- Export as raw tile data + object placement list

The engine reads that data and generates everything at runtime. A simple ImGui window inside a debug build of the game is enough for an early editor.

## Scaling

Each server process is one world — identical to how OSRS worlds work. Players pick a world and connect to that process. All worlds share a database for character persistence. No exotic distributed systems needed until player counts demand it, and even then the solution is just more world processes.
