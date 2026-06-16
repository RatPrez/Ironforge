#include "system/Camera.hpp"

#include <raylib.h>
#include <raymath.h>

#include "base/CamSettings.hpp"
#include "Components.hpp"

void System::Camera(WorldContext& ctx, const float& dt)
{
    auto entity = ctx.registry.view<RenderPosition, LocalPlayer>().front();
    if (entity == entt::null) {
        return;
    }

    auto& rp = ctx.registry.get<RenderPosition>(entity);
    auto& settings = ctx.camSettings;

    float scroll = GetMouseWheelMove();
    if (scroll != 0.f) {
        settings.fov = Clamp(settings.fov - scroll * 4.f, 10.f, 100.f);
        ctx.camera.fovy = settings.fov;
    }

    const float rotateSpeed = 90.0f * dt;

    if (IsKeyDown(KEY_LEFT)) {
        settings.yaw -= rotateSpeed;
    } else if (IsKeyDown(KEY_RIGHT)) {
        settings.yaw += rotateSpeed;
    }

    if (IsKeyDown(KEY_UP)) {
        settings.pitch = Clamp(settings.pitch + rotateSpeed, 5.0f, 89.0f);
    } else if (IsKeyDown(KEY_DOWN)) {
        settings.pitch = Clamp(settings.pitch - rotateSpeed, 5.0f, 89.0f);
    }

    float yawRad = settings.yaw * DEG2RAD;
    float pitchRad = settings.pitch * DEG2RAD;

    ctx.camera.target = { rp.x, rp.y, rp.z };
    ctx.camera.position = {
        rp.x + settings.radius * cosf(pitchRad) * sinf(yawRad),
        rp.y + settings.radius * sinf(pitchRad),
        rp.z + settings.radius * cosf(pitchRad) * cosf(yawRad),
    };
}
