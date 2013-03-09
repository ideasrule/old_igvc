# Libraries.pro 

# NOT auto-generated, don't replace with auto-generated version
# generator doesn't handle non-project (not IGVC, for example) paths.
# -- Josh, 2010-12-23

# Put additional options in the corresponding Libraries.pri

PROJECT = Libraries
TEMPLATE = subdirs

CONFIG *=   # type of lib, if this is a lib project

CONFIG *= debug_and_release
CONFIG *= build_all
CONFIG *= warn_on
CONFIG *= console

PAVE_COMMON_PATH = ../
PAVE_COMMON_LIBRARY_PATH = ./

SUBDIRS *= Camera
SUBDIRS *= Geometry
SUBDIRS *= IPC_library
SUBDIRS *= Newmat
SUBDIRS *= SharedCommon

# optional pri file to specify additional build options and dependencies
include( Libraries.pri )

# include all project dependencies (whether or not they're used -- this can be fixed later if needed)
# project libraries can be missing if building PAVE_COMMON projects
include( $${PAVE_COMMON_PATH}/Common.pri )
