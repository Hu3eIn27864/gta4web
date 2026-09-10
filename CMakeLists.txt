cmake_minimum_required(VERSION 3.20)
project(gta4_web VERSION 0.1.0 DESCRIPTION "Clean-room WebAssembly/WebGL open-world engine prototype" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(gta4_web src/app.cpp)
target_include_directories(gta4_web PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(gta4_web PRIVATE -Wall -Wextra -Wpedantic)

if(NOT EMSCRIPTEN)
  message(FATAL_ERROR "This initial project targets Emscripten. Configure with: emcmake cmake -S . -B build")
endif()

set_target_properties(gta4_web PROPERTIES
  OUTPUT_NAME "gta4_web"
  SUFFIX ".html"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/web"
)

target_link_options(gta4_web PRIVATE
  "-sMIN_WEBGL_VERSION=2"
  "-sMAX_WEBGL_VERSION=2"
  "-sALLOW_MEMORY_GROWTH=1"
  "-sNO_EXIT_RUNTIME=1"
  "-sASSERTIONS=1"
  "--shell-file=${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html"
)
