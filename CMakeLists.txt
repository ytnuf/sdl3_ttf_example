
cmake_minimum_required(VERSION "3.30")

project("using_sdl_ttf")

add_executable("example_proj")
target_sources("example_proj" PRIVATE
    "main.cpp"
)

find_package("SDL3" CONFIG REQUIRED)
find_package("SDL3_ttf" CONFIG REQUIRED)
target_link_libraries("example_proj"
    PRIVATE "SDL3::SDL3"
    PRIVATE "SDL3_ttf::SDL3_ttf"
)
