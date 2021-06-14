set(CMAKE_CXX_STANDARD 11)

add_library(gambatte_sdllib audiosink.cpp blitterwrapper.cpp parser.cpp sdlblitter.cpp str_to_sdlkey.cpp usec.cpp)

include_directories(${PROJECT_SOURCE_DIR}/libgambatte/include ${PROJECT_SOURCE_DIR}/common ${PROJECT_SOURCE_DIR}/libgambatte/src)

add_definitions(-I$(PSPDEV)/psp/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT) #SDL Options

add_executable(gambatte_sdl.elf gambatte_sdl.cpp)
# set_target_properties(gambatte_sdl.prx PROPERTIES LINK_FLAGS "-specs=${PSPSDK}/lib/prxspecs -Wl,-q,-T${PSPSDK}/lib/linkfile.prx ${PSPSDK}/lib/prxexports.o")
target_link_libraries(gambatte_sdl.elf gambatte_sdllib gambatte common SDL z gcc stdc++ c ${PSP_LIBRARIES} psppower pspkernel)

create_pbp_file(TARGET gambatte_sdl.elf TITLE "Gambatte SDL")