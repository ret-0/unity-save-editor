cmake_minimum_required(VERSION 3.9)
project(unity-save-editor CXX)

find_package(SDL2 REQUIRED)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-pipe -std=c++17 -D_POSIX_SOURCE \
-Wall -Wformat-security \
-ffast-math -fdata-sections -ffunction-sections -flto \
-fno-plt -fPIC \
-I ../imgui/ \
")

set(CMAKE_CXX_FLAGS_DEBUG "-Og -g -pg -fsanitize=undefined -fsanitize=address") # -fsanitize=undefined -fsanitize=address -fsanitize=thread
set(CMAKE_CXX_FLAGS_RELEASE "-Os -s -Wl,--build-id=none -Wl,--gc-sections -Wl,-s")

add_library(imgui STATIC
	imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp
	imgui/imgui_impl_sdl.cpp imgui/imgui_impl_opengl3.cpp
	imgui/imgui_stdlib.cpp
)
target_compile_options(imgui PUBLIC -I imgui/ -I/usr/include/SDL2 -D_REENTRANT)

add_executable(editor
	imgui/ImGuiFileDialog/ImGuiFileDialog.cpp
	src/main.cpp src/log.cpp
)
target_link_libraries(editor PRIVATE imgui GL dl pthread SDL2 nlohmann_json::nlohmann_json)
