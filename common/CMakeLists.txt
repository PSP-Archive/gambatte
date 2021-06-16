set (RESAMPLE_FILES resample/src/chainresampler.cpp resample/src/i0.cpp resample/src/kaiser50sinc.cpp resample/src/kaiser70sinc.cpp resample/src/makesinckernel.cpp resample/src/resamplerinfo.cpp resample/src/u48div.cpp )
set (VIDEOLINK_FILES videolink/rgb32conv.cpp videolink/vfilterinfo.cpp)
set (VFILTER_FILES videolink/vfilters/catrom2x.cpp videolink/vfilters/catrom3x.cpp videolink/vfilters/kreed2xsai.cpp videolink/vfilters/maxsthq2x.cpp videolink/vfilters/maxsthq3x.cpp)

set( COMMON_FILES adaptivesleep.cpp rateest.cpp skipsched.cpp ${RESAMPLE_FILES} ${VIDEOLINK_FILES} ${VFILTER_FILES})

add_library (common ${COMMON_FILES})
 
