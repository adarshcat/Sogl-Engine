cmake_minimum_required(VERSION 3.27)
project(LearnOpengl VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O4")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")

include_directories(
        include
        src
        src/util/imgui
        src/util/imgui/backends
)

file(GLOB SOURCES src/engine/*.cpp src/renderer/*.cpp src/renderer/modules/*.cpp src/util/*.cpp src/util/shaders/*.cpp src/util/imgui/*.cpp src/util/imgui/backends/*.cpp src/*.cpp)

add_executable(LearnOpengl ${SOURCES})

target_compile_definitions(LearnOpengl PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)

target_link_libraries(LearnOpengl PRIVATE
    opengl32
    ${CMAKE_SOURCE_DIR}/lib/glew32.lib
    ${CMAKE_SOURCE_DIR}/lib/libglfw3dll.a
    ${CMAKE_SOURCE_DIR}/lib/libassimp.dll.a
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
