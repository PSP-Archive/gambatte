set (FILE_FILES file/file_zip.cpp file/file.cpp file/unzip/ioapi.c file/unzip/unzip.c)
set (MEM_FILES mem/cartridge.cpp mem/memptrs.cpp mem/pakinfo.cpp mem/rtc.cpp)
set (SOUND_FILES sound/channel1.cpp sound/channel2.cpp sound/channel3.cpp sound/channel4.cpp sound/duty_unit.cpp sound/envelope_unit.cpp sound/length_counter.cpp)
set (VIDEO_FILES video/ly_counter.cpp video/lyc_irq.cpp video/next_m0_time.cpp video/ppu.cpp video/sprite_mapper.cpp)
set (MAIN_FILES bitmap_font.cpp cpu.cpp gambatte.cpp initstate.cpp interrupter.cpp interruptrequester.cpp loadres.cpp memory.cpp sound.cpp state_osd_elements.cpp statesaver.cpp tima.cpp video.cpp)

include_directories(${PROJECT_SOURCE_DIR}/libgambatte/include ${PROJECT_SOURCE_DIR}/common ${PROJECT_SOURCE_DIR}/libgambatte/src)
add_definitions(-DHAVE_STDINT_H) # Dunno why this is a thing.. oh well

add_library(gambatte ${MAIN_FILES} ${FILE_FILES} ${MEM_FILES} ${SOUND_FILES} ${VIDEO_FILES})
