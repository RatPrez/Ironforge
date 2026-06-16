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
        settings.fov = Clamp(settings.fov - scroll * 2.f, 5.f, 100.f);
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

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        Vector2 delta = GetMouseDelta();
        settings.yaw   -= delta.x * 0.3f;
        settings.pitch  = Clamp(settings.pitch + delta.y * 0.3f, 5.0f, 89.0f);
    }

    float yawRad = settings.yaw * DEG2RAD;
    float pitchRad = settings.pitch * DEG2RAD;

    constexpr float kFocusHeight = 2.5f; // just above center of a 4-unit-tall character
    Vector3 target = { rp.x, rp.y + kFocusHeight, rp.z };

    if (!settings.focusInitialized) {
        settings.focusX = target.x;
        settings.focusY = target.y;
        settings.focusZ = target.z;
        settings.focusInitialized = true;
    }

    constexpr float kFocusLagSpeed = 5.0f; // higher = snappier, lower = more lag
    float smoothing = 1.f - expf(-kFocusLagSpeed * dt);
    settings.focusX = Lerp(settings.focusX, target.x, smoothing);
    settings.focusY = Lerp(settings.focusY, target.y, smoothing);
    settings.focusZ = Lerp(settings.focusZ, target.z, smoothing);

    Vector3 focus = { settings.focusX, settings.focusY, settings.focusZ };

    ctx.camera.target = focus;
    ctx.camera.position = {
        focus.x + settings.radius * cosf(pitchRad) * sinf(yawRad),
        focus.y + settings.radius * sinf(pitchRad),
        focus.z + settings.radius * cosf(pitchRad) * cosf(yawRad),
    };
}
