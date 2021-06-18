add_executable(gambatte_psp gambatte_psp.cpp
  audiosink.cpp
  blitterwrapper.cpp
  sdlblitter.cpp
  usec.cpp
  GPU/draw.cpp)

target_link_libraries(gambatte_psp
  PUBLIC gambatte
  PUBLIC common
  PUBLIC z
  PUBLIC gcc
  PUBLIC stdc++
  PUBLIC SDL
  PUBLIC ${PSP_LIBRARIES}
  PUBLIC psppower)

create_pbp_file(TARGET gambatte_psp TITLE "Gambatte PSP SDL"
    ICON_PATH ${CMAKE_SOURCE_DIR}/resources/ICON0.png)
