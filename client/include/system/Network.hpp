#pragma once
#include "base/WorldContext.hpp"

namespace System
{
    void NetReceive(WorldContext& ctx);
    void NetSend(WorldContext& ctx);
};
