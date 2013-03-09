# include all project dependencies (whether or not they're used -- this can be fixed later if needed)

# OpenCV

INCLUDEPATH *= $${PAVE_COMMON_PATH}/OpenCV/opencv/include/opencv
INCLUDEPATH *= $${PAVE_COMMON_PATH}/OpenCV/opencv/modules/core/include
INCLUDEPATH *= $${PAVE_COMMON_PATH}/OpenCV/opencv/modules/flann/include
INCLUDEPATH *= $${PAVE_COMMON_PATH}/OpenCV/opencv/modules/imgproc/include
INCLUDEPATH *= $${PAVE_COMMON_PATH}/OpenCV/opencv/modules/video/include
INCLUDEPATH *= $${PAVE_COMMON_PATH}/OpenCV/opencv/modules/highgui/include
INCLUDEPATH *= $${PAVE_COMMON_PATH}/OpenCV/opencv/modules/ml/include
INCLUDEPATH *= $${PAVE_COMMON_PATH}/OpenCV/opencv/modules/calib3d/include
INCLUDEPATH *= $${PAVE_COMMON_PATH}/OpenCV/opencv/modules/features2d/include
INCLUDEPATH *= $${PAVE_COMMON_PATH}/OpenCV/opencv/modules/objdetect/include
INCLUDEPATH *= $${PAVE_COMMON_PATH}/OpenCV/opencv/modules/legacy/include
INCLUDEPATH *= $${PAVE_COMMON_PATH}/OpenCV/opencv/modules/contrib/include

CONFIG(debug, debug|release) {
    QMAKE_LIBDIR *= $${PAVE_COMMON_PATH}/OpenCV/debug/lib/debug
    LIBS *= opencv_calib3d220d.lib
    LIBS *= opencv_core220d.lib
    LIBS *= opencv_highgui220d.lib
    LIBS *= opencv_imgproc220d.lib
}

CONFIG(release, debug|release) {
    QMAKE_LIBDIR *= $${PAVE_COMMON_PATH}/OpenCV/release/lib/release
    LIBS *= opencv_calib3d220.lib
    LIBS *= opencv_core220.lib
    LIBS *= opencv_highgui220.lib
    LIBS *= opencv_imgproc220.lib
}


# SVS

# determine system version
VER = $$system(ver)
VER_FOUND_XP = $$find(VER, 5.* )
VER_FOUND_VISTA = $$find(VER, 6.* )
! isEmpty(VER_FOUND_XP) {
    CONFIG += use_svs44
}
! isEmpty(VER_FOUND_VISTA) {
    CONFIG += use_svs45
}

use_svs44 {
    INCLUDEPATH *= $${PAVE_COMMON_PATH}/SVS/SVS44/src
    QMAKE_LIBDIR *= $${PAVE_COMMON_PATH}/SVS/SVS44/bin
}
use_svs45 {
    INCLUDEPATH *= $${PAVE_COMMON_PATH}/SVS/SVS45/src
    QMAKE_LIBDIR *= $${PAVE_COMMON_PATH}/SVS/SVS45/bin
}
LIBS *= svs.lib
LIBS *= svspix.lib
LIBS *= svscalc.lib
LIBS *= svsgrab.lib


# IPC

INCLUDEPATH *= $${PAVE_COMMON_PATH}/IPC/ipc-3.8.6/src
LIBS *= wsock32.lib  # WinSock


# Boost

INCLUDEPATH *= $${PAVE_COMMON_PATH}/Boost/boost_1_46_0
QMAKE_LIBDIR *= $${PAVE_COMMON_PATH}/Boost/stage/lib


# Labjack

INCLUDEPATH *= $${PAVE_COMMON_PATH}/Labjack/Drivers
QMAKE_LIBDIR *= $${PAVE_COMMON_PATH}/Labjack/Drivers
LIBS *= labjackud.lib


# Qwt

INCLUDEPATH *= $${PAVE_COMMON_PATH}/qwt-6.0.0-rc5/src

QMAKE_LIBDIR *= $${PAVE_COMMON_PATH}/qwt-6.0.0-rc5/lib
CONFIG(debug, debug|release) {
    LIBS *= qwtd.lib
}

CONFIG(release, debug|release) {
    LIBS *= qwt.lib
}
