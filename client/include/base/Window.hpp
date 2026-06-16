#pragma once
#include <raylib.h>
#include "Game.hpp"

struct Window
{
public:
    Window();
    ~Window();
    void run();

private:
    Camera3D m_camera;
    Game m_game;

};
