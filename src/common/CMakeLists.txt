add_library (common OBJECT
  adaptivesleep.cpp
  rateest.cpp
  skipsched.cpp)

add_subdirectory(resample)
add_subdirectory(videolink)

target_link_libraries(common
  PUBLIC common-videolink
  PUBLIC common-resample)
