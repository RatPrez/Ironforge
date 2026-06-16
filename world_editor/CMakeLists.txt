file(GLOB_RECURSE EDITOR_SOURCES src/*.cpp)

add_executable(world_editor ${EDITOR_SOURCES})

target_include_directories(world_editor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/shared/include)

target_link_libraries(world_editor PRIVATE raylib)
