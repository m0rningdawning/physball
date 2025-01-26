cmake_minimum_required(VERSION 3.10)
project(physball)

set(CMAKE_CXX_STANDARD 17)

# Include dirs
include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src/ball
        ${CMAKE_SOURCE_DIR}/src/util
)

# Imgui shenanigans
set(IMGUI_SOURCES
        ${CMAKE_SOURCE_DIR}/lib/imgui/imgui.cpp
        ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_draw.cpp
        ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_demo.cpp
        ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_tables.cpp
        ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_widgets.cpp
        ${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_glfw.cpp
        ${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC include/imgui include/imgui/backends)

# Link dirs
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Source files
file(
        GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp
        GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/ball/*.cpp
        GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/util/*.cpp
        ${CMAKE_SOURCE_DIR}/lib/glad.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
        glfw/glfw3
        opengl32
        imgui
        #        dl
        #        pthread
)
