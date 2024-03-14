cmake_minimum_required(VERSION 3.27)
project(LearnOpengl VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")

include_directories(
        include
        src
)

file(GLOB SOURCES src/engine/*.cpp src/renderer/*.cpp src/util/*.cpp src/util/shaders/*.cpp src/*.cpp)

add_executable(LearnOpengl ${SOURCES})

target_link_libraries(LearnOpengl PRIVATE
    opengl32
    ${CMAKE_SOURCE_DIR}/lib/glew32.lib
    ${CMAKE_SOURCE_DIR}/lib/libglfw3dll.a
    ${CMAKE_SOURCE_DIR}/lib/libassimp.dll.a
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
