# Specifies dependencies for the PAVE_Common Libraries
# Assumes PAVE_COMMON_LIBRARY_PATH is set to the this directory

# Also, assumes PAVE_COMMON_LIBRARIES contains whatever libraries
#   the including project needs.

# -- Josh, 2010-12-23

CONFIG(debug, debug|release) {
    QMAKE_LIBDIR *= $${PAVE_COMMON_LIBRARY_PATH}/debug
}
CONFIG(release, debug|release) {
    QMAKE_LIBDIR *= $${PAVE_COMMON_LIBRARY_PATH}/release
}

for(pave_common_lib, PAVE_COMMON_LIBRARIES) {
    INCLUDEPATH *= $${PAVE_COMMON_LIBRARY_PATH}/$${pave_common_lib}
    
    CONFIG(debug, debug|release) {
        LIBS *= $${pave_common_lib}.lib
    }
    CONFIG(release, debug|release) {
        LIBS *= $${pave_common_lib}.lib
    }
    
    SUBDIRS *= $${PAVE_COMMON_LIBRARY_PATH}/$${pave_common_lib}
}
