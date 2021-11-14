cmake_minimum_required(VERSION 3.10)
project(minesweeper VERSION 0.1.0 LANGUAGES C)

set(TARGET ${PROJECT_NAME} C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED on)

set(SOURCES
	"src/minesweeper.c"
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("lib/raylib")


add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} raylib)