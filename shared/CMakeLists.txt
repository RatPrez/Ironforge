add_library(shared INTERFACE)

target_include_directories(shared INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(shared INTERFACE
    EnTT::EnTT
    GameNetworkingSockets::GameNetworkingSockets)
