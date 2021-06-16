add_library(gambatte OBJECT
  cpu.cpp
  gambatte.cpp
  initstate.cpp
  interrupter.cpp
  interruptrequester.cpp
  loadres.cpp
  memory.cpp
  sound.cpp
  state_osd_elements.cpp
  statesaver.cpp
  tima.cpp
  video.cpp)

add_subdirectory(file)
add_subdirectory(mem)
add_subdirectory(sound)
add_subdirectory(video)

target_link_libraries(gambatte
  PUBLIC gambatte-file
  PUBLIC gambatte-mem
  PUBLIC gambatte-sound
  PUBLIC gambatte-video)

target_compile_definitions(gambatte
  PUBLIC -DHAVE_STDINT_H)
