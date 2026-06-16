#pragma once
#include <cstdint>

enum class MessageType : uint8_t {
    Ping = 1,
    Pong = 2,
    Chat = 3,
};

#pragma pack(push, 1)
struct MessageHeader {
    MessageType type;
    uint16_t    payload_len;
};

struct ChatMessage {
    MessageHeader header;
    char          text[256];
};
#pragma pack(pop)
