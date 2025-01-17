cmake_minimum_required(VERSION 3.10)
project(physball)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        ${CMAKE_SOURCE_DIR}/include
)

link_directories(${CMAKE_SOURCE_DIR}/lib)

file(
        GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/include/glad/glad.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
        glfw3
        opengl32
        #        dl
        #        pthread
)
