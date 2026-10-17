cmake_minimum_required(VERSION 3.20)
project(Nazarene LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization and warning flags
if(MSVC)
    add_compile_options(/W4 /permissive- /O2)
else()
    add_compile_options(-Wall -Wextra -Wno-unused-parameter -O2)
endif()

# Fetch Raylib 5.5
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(CUSTOMIZE_BUILD OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

set(SOURCE_FILES
    src/main.cpp
    src/Constants.h
    src/Input.h
    src/Input.cpp
    src/Audio.h
    src/Audio.cpp
    src/Graphics.h
    src/Graphics.cpp
    src/Level.h
    src/Level.cpp
    src/Player.h
    src/Player.cpp
    src/Enemies.h
    src/Enemies.cpp
    src/BonusStage.h
    src/BonusStage.cpp
    src/Mission2Graphics.h
    src/Mission2Graphics.cpp
    src/Mission3Graphics.h
    src/Mission3Graphics.cpp
    src/Mission4Graphics.h
    src/Mission4Graphics.cpp
)

add_executable(Nazarene ${SOURCE_FILES})

target_include_directories(Nazarene PRIVATE src)
target_link_libraries(Nazarene PRIVATE raylib)

# Link Windows multimedia and OpenGL libraries
if(WIN32)
    target_link_libraries(Nazarene PRIVATE winmm gdi32 opengl32)
endif()
