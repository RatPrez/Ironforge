#pragma once
#include <cstdint>

enum PacketType : int
{
    // client -> server
    CMove = 0,

    // server -> client
    SSpawn    = 1,
    SPosition = 2,
    SSelf     = 3,
    SDespawn  = 4,
    CChat     = 5,
    SChat     = 6,
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
