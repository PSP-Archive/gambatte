add_library(gambatte OBJECT
  bitmap_font.cpp
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
  video.cpp

  # file/unzip/ioapi.c
  # file/unzip/unzip.c
  file/file.cpp
  # file/file_zip.cpp

  mem/cartridge.cpp
  mem/memptrs.cpp
  mem/pakinfo.cpp
  mem/rtc.cpp
  
  sound/channel1.cpp
  sound/channel2.cpp
  sound/channel3.cpp
  sound/channel4.cpp
  sound/duty_unit.cpp
  sound/envelope_unit.cpp
  sound/length_counter.cpp
  
  video/lyc_irq.cpp
  video/ly_counter.cpp
  video/next_m0_time.cpp
  video/ppu.cpp
  video/sprite_mapper.cpp)

target_compile_definitions(gambatte
  PUBLIC -DHAVE_STDINT_H)
