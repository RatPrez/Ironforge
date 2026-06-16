file(GLOB_RECURSE CLIENT_SOURCES src/*.cpp)

set(IMGUI_RL_DIR ${CMAKE_SOURCE_DIR}/lib/imgui_rl)

set(IMGUI_RL_SOURCES
    ${IMGUI_RL_DIR}/imgui.cpp
    ${IMGUI_RL_DIR}/imgui_draw.cpp
    ${IMGUI_RL_DIR}/imgui_tables.cpp
    ${IMGUI_RL_DIR}/imgui_widgets.cpp
    ${IMGUI_RL_DIR}/rlImGui.cpp
)

add_executable(client ${CLIENT_SOURCES} ${IMGUI_RL_SOURCES})

target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${IMGUI_RL_DIR})

target_link_libraries(client PRIVATE
    shared
    EnTT::EnTT
    GameNetworkingSockets::GameNetworkingSockets
    raylib)
