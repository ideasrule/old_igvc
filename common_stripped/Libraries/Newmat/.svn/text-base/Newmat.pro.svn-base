# Newmat.pro

# NOT auto-generated, don't replace with auto-generated version

# generator doesn't handle non-project (not IGVC, for example) paths.
# -- Josh, 2010-12-23

# Put additional options in the corresponding Newmat.pri

PROJECT = Newmat

CONFIG += debug_and_release
CONFIG += build_all
CONFIG += warn_on
CONFIG += console

PAVE_COMMON_PATH = ../../../PAVE_Common
PAVE_COMMON_LIBRARY_PATH = ../../../PAVE_Common/Libraries

SOURCES += bandmat.cpp
SOURCES += cholesky.cpp
SOURCES += evalue.cpp
SOURCES += fft.cpp
SOURCES += hholder.cpp
SOURCES += jacobi.cpp
SOURCES += myexcept.cpp
SOURCES += newfft.cpp
SOURCES += newmat1.cpp
SOURCES += newmat2.cpp
SOURCES += newmat3.cpp
SOURCES += newmat4.cpp
SOURCES += newmat5.cpp
SOURCES += newmat6.cpp
SOURCES += newmat7.cpp
SOURCES += newmat8.cpp
SOURCES += newmat9.cpp
SOURCES += newmatex.cpp
SOURCES += newmatnl.cpp
SOURCES += newmatrm.cpp
SOURCES += nm_misc.cpp
SOURCES += solution.cpp
SOURCES += sort.cpp
SOURCES += submat.cpp
SOURCES += svd.cpp

HEADERS += controlw.h
HEADERS += include.h
HEADERS += myexcept.h
HEADERS += newmat.h
HEADERS += newmatap.h
HEADERS += newmatio.h
HEADERS += newmatnl.h
HEADERS += newmatrc.h
HEADERS += newmatrm.h
HEADERS += precisio.h
HEADERS += solution.h
HEADERS += tmt.h

# optional pri file to specify additional build options and dependencies
include( Newmat.pri )

# output dir for build outputs
CONFIG(debug, debug|release) {
    DESTDIR = $${PAVE_COMMON_LIBRARY_PATH}/debug
    TARGET = Newmat
}
CONFIG(release, debug|release) {
    DESTDIR = $${PAVE_COMMON_LIBRARY_PATH}/release
    TARGET = Newmat
}

# include all project dependencies (whether or not they're used -- this can be fixed later if needed)
# project libraries can be missing if building PAVE_COMMON projects
include( $${PAVE_COMMON_LIBRARY_PATH}/Libraries.pri )
include( $${PAVE_COMMON_PATH}/Common.pri )
