cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 14)
project(gambatte)


    #Build files
add_subdirectory(common)
add_subdirectory(libgambatte/src)
add_subdirectory(gambatte_sdl/src)