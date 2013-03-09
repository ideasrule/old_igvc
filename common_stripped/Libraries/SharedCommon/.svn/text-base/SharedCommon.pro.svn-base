# SharedCommon.pro 

# NOT auto-generated, don't replace with auto-generated version
# generator doesn't handle non-project (not IGVC, for example) paths.
# -- Josh, 2010-12-23

# Put additional options in the corresponding SharedCommon.pri

PROJECT = SharedCommon

CONFIG += debug_and_release
CONFIG += build_all
CONFIG += warn_on
CONFIG += console

PAVE_COMMON_PATH = ../../
PAVE_COMMON_LIBRARY_PATH = ../

SOURCES *= CommonFormats.cpp
SOURCES *= Conversions.cpp
SOURCES *= Data.cpp
SOURCES *= IPCConnect.cpp
SOURCES *= IPCMessage.cpp
SOURCES *= MedianFilter.cpp
SOURCES *= PerformanceTimer.cpp
SOURCES *= Timer.cpp

HEADERS *= Array2D.h
HEADERS *= CommonFormats.h
HEADERS *= Conversions.h
HEADERS *= Data.h
HEADERS *= Handler.h
HEADERS *= IPCConnect.h
HEADERS *= IPCMessage.h
HEADERS *= MedianFilter.h
HEADERS *= Memory.h
HEADERS *= PerformanceTimer.h
HEADERS *= Timer.h
HEADERS *= imutil.h

# optional pri file to specify additional build options and dependencies
include( SharedCommon.pri )

# output dir for build outputs
CONFIG(debug, debug|release) {
    DESTDIR = $${PAVE_COMMON_LIBRARY_PATH}/debug
    TARGET = SharedCommon
}
CONFIG(release, debug|release) {
    DESTDIR = $${PAVE_COMMON_LIBRARY_PATH}/release
    TARGET = SharedCommon
}

# include all project dependencies (whether or not they're used -- this can be fixed later if needed)
# project libraries can be missing if building PAVE_COMMON projects
include( $${PAVE_COMMON_LIBRARY_PATH}/Libraries.pri )
include( $${PAVE_COMMON_PATH}/Common.pri )
