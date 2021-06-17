set(CMAKE_CXX_STANDARD 11)

add_library(gambatte_sdllib audiosink.cpp blitterwrapper.cpp parser.cpp sdlblitter.cpp str_to_sdlkey.cpp usec.cpp)

include_directories(${PROJECT_SOURCE_DIR}/libgambatte/include ${PROJECT_SOURCE_DIR}/common ${PROJECT_SOURCE_DIR}/libgambatte/src ${SDL_INCLUDE_DIR})

add_executable(gambatte_sdl gambatte_sdl.cpp)



if(PSP)
    set(ADDITIONAL_LIBS psppower ${PSP_LIBRARIES})
endif(PSP)

target_link_libraries(gambatte_sdl gambatte_sdllib gambatte common SDL z gcc stdc++ SDL ${ADDITIONAL_LIBS})

if(PSP)
    create_pbp_file(TARGET gambatte_sdl TITLE Gambatte_SDL)
 endif(PSP)