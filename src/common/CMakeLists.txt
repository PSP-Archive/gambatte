add_library (common OBJECT
  adaptivesleep.cpp
  rateest.cpp
  skipsched.cpp
  resample/chainresampler.cpp
  resample/i0.cpp
  resample/kaiser50sinc.cpp
  resample/kaiser70sinc.cpp
  resample/makesinckernel.cpp
  resample/resamplerinfo.cpp
  resample/u48div.cpp
  videolink/rgb32conv.cpp
  videolink/vfilterinfo.cpp
  videolink/vfilters/catrom2x.cpp
  videolink/vfilters/catrom3x.cpp
  videolink/vfilters/kreed2xsai.cpp
  videolink/vfilters/maxsthq2x.cpp
  videolink/vfilters/maxsthq3x.cpp)
