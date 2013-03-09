# CameraTest.pro 
# 2010-12-23 19:19:10.234000

# NOT auto-generated, don't replace with auto-generated version
# generator doesn't handle non-project (not IGVC, for example) paths.
# -- Josh, 2010-12-23

# Put additional options in the corresponding CameraTest.pri

PROJECT = CameraTest
TEMPLATE = lib

CONFIG += staticlib  # type of lib, if this is a lib project

CONFIG += debug_and_release
CONFIG += build_all
CONFIG += warn_on
CONFIG += console

PAVE_COMMON_PATH = ../../
PAVE_COMMON_LIBRARY_PATH = ../

SOURCES += cameratest.cpp

# optional pri file to specify additional build options and dependencies
include( CameraTest.pri )

# output dir for build outputsCONFIG(debug, debug|release) {
    DESTDIR = $${PAVE_COMMON_LIBRARY_PATH}/debug
    TARGET = CameraTest
}
CONFIG(release, debug|release) {
    DESTDIR = $${PAVE_COMMON_LIBRARY_PATH}/release
    TARGET = CameraTest
}

# include all project dependencies (whether or not they're used -- this can be fixed later if needed)
# project libraries can be missing if building PAVE_COMMON projects
include( $${PAVE_COMMON_LIBRARY_PATH}/Libraries.pri )
include( $${PAVE_COMMON_PATH}/Common.pri )
