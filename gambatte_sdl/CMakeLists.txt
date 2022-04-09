add_executable(gambatte_sdl gambatte_sdl.cpp
  audiosink.cpp
  blitterwrapper.cpp
  parser.cpp
  sdlblitter.cpp
  str_to_sdlkey.cpp
  usec.cpp)


target_link_libraries(gambatte_sdl
  PUBLIC gambatte
  PUBLIC common
  PUBLIC SDL
  PUBLIC z
  PUBLIC gcc
  PUBLIC stdc++
  PUBLIC SDL)

if(PSP) 
  target_link_libraries(gambatte_sdl
    PUBLIC GL
    PUBLIC pspvram
    PUBLIC pspaudio
    PUBLIC pspvfpu
    PUBLIC pspdisplay
    PUBLIC pspgu
    PUBLIC pspge
    PUBLIC psphprm
    PUBLIC pspctrl
    PUBLIC pspirkeyb
    PUBLIC psppower)
  
  create_pbp_file(TARGET gambatte_sdl TITLE "Gambatte SDL")
endif(PSP)
