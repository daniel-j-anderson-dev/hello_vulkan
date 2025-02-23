cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(hello_vulkan)

include_directories(include)

add_executable(
    ${PROJECT_NAME}
    source/main.cpp
)

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    -O2
)

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    glfw
    vulkan
    dl
    pthread
    X11
    Xxf86vm
    Xrandr
    Xi
)
