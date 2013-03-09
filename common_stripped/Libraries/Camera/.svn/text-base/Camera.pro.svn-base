# Camera.pro 

# NOT auto-generated, don't replace with auto-generated version
# generator doesn't handle non-project (not IGVC, for example) paths.
# -- Josh, 2010-12-23

# Put additional options in the corresponding Camera.pri

PROJECT = Camera
TEMPLATE = lib

CONFIG += staticlib  # type of lib, if this is a lib project

CONFIG += debug_and_release
CONFIG += build_all
CONFIG += warn_on
CONFIG += console

PAVE_COMMON_PATH = ../../
PAVE_COMMON_LIBRARY_PATH = ../

SOURCES += Camera.cpp
SOURCES += CameraFiles.cpp
SOURCES += CameraLive.cpp
SOURCES += CameraTriclopsMethods.cpp
SOURCES += CameraVidere.cpp
SOURCES += Frame.cpp

HEADERS += Camera.h
HEADERS += CameraTypes.h
HEADERS += Frame.h
HEADERS += Image.h
HEADERS += TimeUtils.h

# optional pri file to specify additional build options and dependencies
include( Camera.pri )

# output dir for build outputs
CONFIG(debug, debug|release) {
    DESTDIR = $${PAVE_COMMON_LIBRARY_PATH}/debug
    TARGET = Camera
}
CONFIG(release, debug|release) {
    DESTDIR = $${PAVE_COMMON_LIBRARY_PATH}/release
    TARGET = Camera
}

# include all project dependencies (whether or not they're used -- this can be fixed later if needed)
# project libraries can be missing if building PAVE_COMMON projects
include( $${PAVE_COMMON_LIBRARY_PATH}/Libraries.pri )
include( $${PAVE_COMMON_PATH}/Common.pri )
