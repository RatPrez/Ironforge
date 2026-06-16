#include "base/Window.hpp"
#include "rlImGui.h"

Window::Window()
{
    InitWindow(1270, 900, "OSRS");
    SetTargetFPS(60);
    rlImGuiSetup(true);

    // --- CAMERA ---

    m_camera.position = { 0.0f, 80.0f, 80.0f };
    m_camera.target   = { 0.0f, 0.0f, 0.0f };
    m_camera.up       = { 0.0f, 1.0f, 0.0f };
    m_camera.fovy     = 60.0f;
    m_camera.projection = CAMERA_PERSPECTIVE;
}

Window::~Window()
{
    rlImGuiShutdown();
    CloseWindow();
}

void Window::run()
{
    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();
        m_game.tick(dt, m_camera);
        BeginDrawing();
        ClearBackground(BLACK);
        rlImGuiBegin();

        BeginMode3D(m_camera);
        m_game.draw3D(m_camera);
        EndMode3D();

        m_game.draw2D();
        rlImGuiEnd();
        EndDrawing();
        m_game.tickLate(dt, m_camera);
    }
}
