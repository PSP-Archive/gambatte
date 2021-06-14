cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 98)
project(gambatte)


    #Build files
add_subdirectory(common)
add_subdirectory(libgambatte/src)
add_subdirectory(gambatte_sdl/src)


    
#     if (PSP)
#         add_compile_options(-DPSP_LARGE_MEMORY=1 -DPSP_FW_VERSION=661)
#         add_definitions(-DDAEDALUS_PSP)
#         add_library(pspkubridge STATIC IMPORTED)
#         set_property(TARGET pspkubridge PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/SysPSP/Include/lib/libpspkubridge.a)
#         include_directories(BEFORE ${PROJECT_SOURCE_DIR}/SysPSP/Include)
#         add_library(daedalus STATIC ${BUILD} ${PSP_BUILD})
#         add_executable(daedalus.elf ${PSP_MAIN_FILES})
#         set_target_properties(daedalus.elf PROPERTIES LINK_FLAGS "-specs=${PSPSDK}/lib/prxspecs -Wl,-q,-T${PSPSDK}/lib/linkfile.prx ${PSPSDK}/lib/prxexports.o -L${PSPSDK}/lib")
#         target_include_directories(daedalus.elf PUBLIC /usr/local/pspdev/psp/sdk/include )
#         target_link_libraries(daedalus.elf daedalus -lstdc++ -lpsppower -lpspgu -lpspaudiolib -lpspaudio -lpsprtc -lpng -lz -lc -lg -lm -lpspfpu pspkubridge ${PSPSDK_LIBS})
# endif (PSP)
