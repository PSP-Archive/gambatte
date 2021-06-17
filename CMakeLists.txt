cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 14)
project(gambatte
  DESCRIPTION "Gameboy color emulator."
  LANGUAGES CXX C)

include_directories(include)
add_subdirectory(src)
add_subdirectory(gambatte_sdl)
