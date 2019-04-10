INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_NUT2NT nut2nt)

FIND_PATH(
    NUT2NT_INCLUDE_DIRS
    NAMES nut2nt/api.h
    HINTS $ENV{NUT2NT_DIR}/include
        ${PC_NUT2NT_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    NUT2NT_LIBRARIES
    NAMES gnuradio-nut2nt
    HINTS $ENV{NUT2NT_DIR}/lib
        ${PC_NUT2NT_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NUT2NT DEFAULT_MSG NUT2NT_LIBRARIES NUT2NT_INCLUDE_DIRS)
MARK_AS_ADVANCED(NUT2NT_LIBRARIES NUT2NT_INCLUDE_DIRS)

