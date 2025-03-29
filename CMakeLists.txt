
cmake_minimum_required(VERSION "3.30")

project("using_sdl_ttf")

add_executable("example")
target_sources("example" PRIVATE
    "main.cpp"
)

set(BUILD_SHARED_LIBS OFF)
set(SDL_STATIC ON)
add_subdirectory("external/sdl3/")
set(SDLTTF_VENDORED ON)
set(SDLTTF_SAMPLES OFF)
set(SDLTTF_BUILD_SHARED_LIBS OFF)
add_subdirectory("external/sdl3_ttf/")

target_link_libraries("example"
    PRIVATE "SDL3::SDL3-static"
    PRIVATE "SDL3_ttf::SDL3_ttf-static"
)
