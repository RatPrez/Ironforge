#pragma once

struct CamSettings
{
    float yaw = 0.f;
    float pitch = 45.f;
    float radius = 50.f;
    float fov = 40.f;

    // smoothed camera focus point, lags slightly behind the actual target
    bool  focusInitialized = false;
    float focusX = 0.f;
    float focusY = 0.f;
    float focusZ = 0.f;
};
