#include "system/Pathfinder.hpp"

#include <queue>
#include <unordered_map>

#include "shared/Components.hpp"
#include "shared/Base.hpp"
#include "Components.hpp"

namespace
{

    constexpr int kDx[8] = { 0, 1, 1,  1,  0, -1, -1, -1 };
    constexpr int kDy[8] = { 1, 1, 0, -1, -1, -1,  0,  1 };

    struct TileKey
    {
        uint16_t x, y;
        bool operator==(const TileKey& o) const { return x == o.x && y == o.y; }
    };

    struct TileKeyHash
    {
        size_t operator()(const TileKey& k) const {
            return std::hash<uint32_t>()((uint32_t)k.x << 16 | k.y);
        }
    };

    // Octile distance — diagonal costs 14, cardinal costs 10 (scaled sqrt(2))
    inline int octile(int ax, int ay, int bx, int by)
    {
        int dx = std::abs(ax - bx);
        int dy = std::abs(ay - by);
        return 10 * std::max(dx, dy) + 4 * std::min(dx, dy);
    }

    constexpr int kCardinalCost = 10;
    constexpr int kDiagonalCost = 14;
    constexpr bool kIsDiagonal[8] = { false, true, false, true, false, true, false, true };

    struct Node
    {
        int f;
        TileKey tile;
        bool operator>(const Node& o) const { return f > o.f; }
    };

    MovePath astar(uint16_t sx, uint16_t sy, uint16_t tx, uint16_t ty)
    {
        if (sx == tx && sy == ty) return {};

        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> frontier;
        std::unordered_map<TileKey, TileKey, TileKeyHash> parent;
        std::unordered_map<TileKey, int, TileKeyHash> gCost;

        TileKey start{ sx, sy };
        TileKey goal{ tx, ty };

        frontier.push({ 0, start });
        parent[start] = start;
        gCost[start]  = 0;

        while (!frontier.empty()) {
            TileKey cur = frontier.top().tile;
            frontier.pop();

            if (cur == goal) {
                MovePath path;
                TileKey step = goal;
                while (!(step == start)) {
                    path.steps.emplace_back(step.x, step.y);
                    step = parent[step];
                }
                std::reverse(path.steps.begin(), path.steps.end());
                return path;
            }

            for (int i = 0; i < 8; i++) {
                int nx = cur.x + kDx[i];
                int ny = cur.y + kDy[i];

                if (nx < 0 || ny < 0) continue;
                if (std::abs(nx - sx) > Base::kMaxPathDistance || std::abs(ny - sy) > Base::kMaxPathDistance) continue;

                TileKey next{ (uint16_t)nx, (uint16_t)ny };

                // TODO: check tile collision flags here

                int g = gCost[cur] + (kIsDiagonal[i] ? kDiagonalCost : kCardinalCost);
                int h = octile(nx, ny, tx, ty);
                if (!gCost.count(next) || g < gCost[next]) {
                    gCost[next]  = g;
                    parent[next] = cur;
                    frontier.push({ g + h, next });
                }
            }
        }

        return {};
    }
}

void System::Pathfinder(WorldContext& ctx)
{
    auto view = ctx.registry.view<MoveTarget, Position>();
    for (auto entity : view) {
        auto& pos    = view.get<Position>(entity);
        auto& target = view.get<MoveTarget>(entity);

        MovePath path = astar(pos.x, pos.y, target.x, target.y);
        if (!path.steps.empty()) {
            ctx.registry.emplace_or_replace<MovePath>(entity, std::move(path));
        }

        ctx.registry.remove<MoveTarget>(entity);
    }
}
