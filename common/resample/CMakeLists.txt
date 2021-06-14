include_directories(${PROJECT_SOURCE_DIR}/common)
add_library (resample src/chainresampler.cpp src/i0.cpp src/kaiser50sinc.cpp src/kaiser70sinc.cpp src/makesinckernel.cpp src/resamplerinfo.cpp src/u48div.cpp)
