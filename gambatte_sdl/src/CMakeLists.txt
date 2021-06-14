set(CMAKE_CXX_STANDARD 11)

add_library(gambatte_sdllib audiosink.cpp blitterwrapper.cpp parser.cpp sdlblitter.cpp str_to_sdlkey.cpp usec.cpp)

include_directories(${PROJECT_SOURCE_DIR}/libgambatte/include ${PROJECT_SOURCE_DIR}/common ${PROJECT_SOURCE_DIR}/libgambatte/src)

add_definitions(-I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT) #SDL Options

add_executable(gambatte_sdl gambatte_sdl.cpp)
target_link_libraries(gambatte_sdl gambatte_sdllib gambatte common SDL z)