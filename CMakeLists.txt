
cmake_minimum_required(VERSION "3.30")

project("using_sdl_ttf")

add_executable("example")
target_sources("example" PRIVATE
    "main.cpp"
)

add_subdirectory("external/sdl3/")
set(SDLTTF_VENDORED ON)
add_subdirectory("external/sdl3_ttf/")

target_link_libraries("example"
    PRIVATE "SDL3::SDL3"
    PRIVATE "SDL3_ttf::SDL3_ttf"
)
