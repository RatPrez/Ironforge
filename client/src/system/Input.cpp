#include "system/Input.hpp"

#include <raylib.h>
#include <cmath>

#include "Base.hpp"
#include "shared/Packets.hpp"
#include "base/NetContext.hpp"
#include "base/DebugTerminal.hpp"
#include "base/AssetCache.hpp"

static Vector3 raycastAtY(const Ray& ray, float planeY)
{
    if (fabsf(ray.direction.y) < 1e-6f) return { -1.f, 0.f, -1.f };
    float t = (planeY - ray.position.y) / ray.direction.y;
    if (t <= 0.f) return { -1.f, 0.f, -1.f };
    return { ray.position.x + t * ray.direction.x, planeY, ray.position.z + t * ray.direction.z };
}

void System::Input(WorldContext& ctx)
{
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return;
    }

    Ray ray = GetScreenToWorldRay(GetMousePosition(), ctx.camera);

    // first pass at Y=0 to estimate the tile
    Vector3 hitPoint = raycastAtY(ray, 0.f);
    if (hitPoint.x < 0.f) return;

    int gridX = (int)(hitPoint.x / Base::kTileSize);
    int gridY = (int)(hitPoint.z / Base::kTileSize);
    if (gridX < 0 || gridX >= Base::kWorldSize || gridY < 0 || gridY >= Base::kWorldSize) return;

    // refine using that tile's actual terrain height
    float surfaceY = ctx.assets.heightAt(gridX, gridY);
    hitPoint = raycastAtY(ray, surfaceY);
    if (hitPoint.x < 0.f) return;

    gridX = (int)(hitPoint.x / Base::kTileSize);
    gridY = (int)(hitPoint.z / Base::kTileSize);
    if (gridX < 0 || gridX >= Base::kWorldSize || gridY < 0 || gridY >= Base::kWorldSize) return;

    CPacketMove pkt;
    pkt.x = gridX;
    pkt.y = gridY;
    ctx.net.outbox.push(pkt);

    DebugTerminal::Instance()->log("Clicked X: " + std::to_string(gridX) + ", Y: " + std::to_string(gridY));
}
