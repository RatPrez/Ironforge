cmake_minimum_required(VERSION 3.16)
project(osrs_clone CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- EnTT ---
include(FetchContent)
FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG        v3.13.2
)
FetchContent_MakeAvailable(entt)

# --- Find GameNetworkingSockets ---
set(GNS_ROOT "/opt/gns" CACHE PATH "GameNetworkingSockets install prefix")

if(DEFINED GNS_ROOT OR DEFINED ENV{GNS_ROOT})
    set(_gns_root "${GNS_ROOT}")
    if(DEFINED ENV{GNS_ROOT})
        set(_gns_root "$ENV{GNS_ROOT}")
    endif()

    find_library(_gns_lib
        NAMES GameNetworkingSockets GameNetworkingSockets_s
        PATHS "${_gns_root}/lib" "${_gns_root}/lib64"
        NO_DEFAULT_PATH)
    if(NOT _gns_lib)
        message(FATAL_ERROR "GNS_ROOT='${_gns_root}' — library not found in lib/ or lib64/")
    endif()

    set(_gns_include "${_gns_root}/include/GameNetworkingSockets")

    add_library(GameNetworkingSockets::GameNetworkingSockets SHARED IMPORTED)
    set_target_properties(GameNetworkingSockets::GameNetworkingSockets PROPERTIES
        IMPORTED_LOCATION "${_gns_lib}"
        INTERFACE_INCLUDE_DIRECTORIES "${_gns_include}")
    message(STATUS "GameNetworkingSockets: ${_gns_lib}  includes: ${_gns_include}")

else()
    find_package(GameNetworkingSockets QUIET
        HINTS /opt/gns/lib64/cmake /opt/gns/lib/cmake)

    if(NOT GameNetworkingSockets_FOUND)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(GNS REQUIRED GameNetworkingSockets)
        add_library(GameNetworkingSockets::GameNetworkingSockets INTERFACE IMPORTED)
        target_include_directories(GameNetworkingSockets::GameNetworkingSockets INTERFACE ${GNS_INCLUDE_DIRS})
        target_link_libraries(GameNetworkingSockets::GameNetworkingSockets INTERFACE ${GNS_LIBRARIES})
        target_link_directories(GameNetworkingSockets::GameNetworkingSockets INTERFACE ${GNS_LIBRARY_DIRS})
        message(STATUS "GameNetworkingSockets: found via pkg-config")
    else()
        message(STATUS "GameNetworkingSockets: found via find_package")
    endif()
endif()

add_subdirectory(shared)
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(world_editor)
