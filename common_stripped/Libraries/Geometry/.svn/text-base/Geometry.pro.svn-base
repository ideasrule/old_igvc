# Geometry.pro

# NOT auto-generated, don't replace with auto-generated version

# generator doesn't handle non-project (not IGVC, for example) paths.
# -- Josh, 2010-12-23

# Put additional options in the corresponding Geometry.pri

PROJECT = Geometry
TEMPLATE = lib

CONFIG += staticlib  # type of lib, if this is a lib project

CONFIG += debug_and_release
CONFIG += build_all
CONFIG += warn_on
CONFIG += console

PAVE_COMMON_PATH = ../../../PAVE_Common
PAVE_COMMON_LIBRARY_PATH = ../../../PAVE_Common/Libraries

SOURCES += Path2D.cpp
SOURCES += Point2D.cpp
SOURCES += Point3D.cpp

HEADERS += Geometry.h
HEADERS += Path2D.h
HEADERS += Path2DException.h
HEADERS += Point2D.h
HEADERS += Point3D.h
HEADERS += PointStructs.h

# optional pri file to specify additional build options and dependencies
include( Geometry.pri )

# output dir for build outputs
CONFIG(debug, debug|release) {
    DESTDIR = $${PAVE_COMMON_LIBRARY_PATH}/debug
    TARGET = Geometry
}
CONFIG(release, debug|release) {
    DESTDIR = $${PAVE_COMMON_LIBRARY_PATH}/release
    TARGET = Geometry
}

# include all project dependencies (whether or not they're used -- this can be fixed later if needed)
# project libraries can be missing if building PAVE_COMMON projects
include( $${PAVE_COMMON_LIBRARY_PATH}/Libraries.pri )
include( $${PAVE_COMMON_PATH}/Common.pri )
