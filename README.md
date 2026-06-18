# OSRS Clone

A from-scratch recreation of Old School RuneScape built in C++ using raylib, EnTT, and GameNetworkingSockets.

> **Work in progress** — early stages, expect missing features and rough edges.

## What it is

A tile-based multiplayer game with a client-server architecture, aiming to replicate the core feel of OSRS: grid movement, isometric-ish camera, character animations, and a persistent world. The server runs an authoritative game loop with A* pathfinding and tile collision; the client handles rendering, input, and smooth movement interpolation between server ticks.

## Current state

- Multiple clients can connect and see each other move in real time
- Click-to-move with server-side A* pathfinding and tile collision flags
- Heightmap terrain rendered from a binary map chunk (`.omap`), with checkerboard tile shading
- Skeletal character model with walk/idle animation blending
- Smooth heading interpolation when turning
- Isometric camera with zoom, keyboard orbit, and middle-mouse drag pan; smoothed follow
- In-game text chat with floating chat bubbles above characters
- Standalone world editor for painting tile heights and collision flags and exporting map chunks
