cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(nova LANGUAGES C)
set(CMAKE_C_VERSION 89)
#set(CMAKE_C_FLAGS "")

set(NOVA_SOURCE "${CMAKE_CURRENT_LIST_DIR}/src/main.c")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/deps/SDL-Vita")

add_executable(nova ${NOVA_SOURCE})
target_link_libraries(nova PRIVATE SDL2-static)
target_include_directories(nova PRIVATE "${CMAKE_CURRENT_LIST_DIR}/deps/SDL-Vita/include")