#pragma once
#include <cstdint>

enum PacketType : int
{
    // client -> server
    CMove = 0,
    CRun  = 1,

    // server -> client
    SSpawn    = 1000,
    SPosition = 1001,
    SSelf     = 1002,
    SDespawn  = 1003,
    CChat     = 1004,
    SChat     = 1005,
};

struct PacketHeader
{
    PacketType type;
};

#pragma pack(push, 1)

// client -> server
struct CPacketMove
{
    PacketHeader header = { PacketType::CMove };
    int x;
    int y;
};

struct CPacketRun
{
    PacketHeader header = { PacketType::CRun };
    bool state;
};

// server -> client
struct SPacketSpawn
{
    PacketHeader header = { PacketType::SSpawn };
    uint32_t netId;
    uint16_t x, y;
    uint8_t heading;
};

struct SPacketSelf
{
    PacketHeader header = { PacketType::SSelf };
    uint32_t netId;
};

struct SPacketDespawn
{
    PacketHeader header = { PacketType::SDespawn };
    uint32_t netId;
};

struct SPacketPosition
{
    PacketHeader header = { PacketType::SPosition };
    uint32_t netId;  // entity id so client knows who moved
    uint16_t x, y;
    uint8_t  heading;
    bool     isRunning;
    uint16_t midX, midY;
};

// client -> server
struct CPacketChat
{
    PacketHeader header = { PacketType::CChat };
    char text[128];
};

// server -> client
struct SPacketChat
{
    PacketHeader header = { PacketType::SChat };
    uint32_t netId;
    char text[128];
};

#pragma pack(pop)
