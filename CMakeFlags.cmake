
set (PROJECT_FLAGS
    -DPROJECT_NAME="CamSim"
    -DHAVE_FFMPEG=0
#    -DHAVE_VIDEO_ENCODER=0
    -DHAVE_TOUCH=0
    -DHAVE_3DGRAPHICS=1
    -DHAVE_VIDEO_ENCODER=1
	-DHAVE_AV_DEVICE=0 ##TODO:REMOVE WITH FFMPEG LIBAV FLAGS 

	-DOPENGLES_VERSION=2

	-DHAVE_OPENCV=1# if 0 -> nsrVideoStab.cpp in Images would have errors !
	-DHAVE_PLPLOT=1
)

