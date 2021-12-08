
set( CMAKE_AUTOMOC OFF )
set( CMAKE_AUTOUIC OFF )
set( CMAKE_AUTORCC OFF )

# minigzip: mini gzip implementation
add_library( minigzip STATIC "minigzip/minigzip.c" )
target_link_libraries( minigzip ZLIB::ZLIB )

if (Qt5Gui_FOUND)
  get_target_property(QT_INCLUDE_DIR Qt5::Gui INTERFACE_INCLUDE_DIRECTORIES)
  message( STATUS "Qt5 GUI found ${QT_INCLUDE_DIR}" )
else()
  message( STATUS Qt5 GUI not found )
endif ()

# Qwt5
option( BUILD_QWT "Build Qwt using own code" OFF )

if( NOT BUILD_QWT )
  find_library ( QWT_LIBRARY
    NAMES qwt5-qt5 qwt-qt5
    HINTS ${QT_LIBRARY_DIR} /usr/lib64
  )
  if( NOT QWT_LIBRARY )
    set( BUILD_QWT ON CACHE BOOL "Build Qwt using own code" FORCE )
  endif()
endif()

if( NOT BUILD_QWT )
  find_path ( QWT_INCLUDE_DIR
    NAMES qwt_plot.h
    HINTS ${QT_INCLUDE_DIR}
    PATH_SUFFIXES qwt5 qwt5-qt5
    REQUIRED
  )
  message( STATUS "Found Qwt5: ${QWT_LIBRARY}, with include: ${QWT_INCLUDE_DIR}" )
else()
  # Build Qwt5
  message( STATUS "Qwt5: using own code" )
  set(QWT_DESIGNER off)
  add_subdirectory( qwt5-qt5 )
endif()

# QwtPlot3D
option( BUILD_QWTPLOT3D "Build QwtPlot3D using own code" OFF )

if( NOT BUILD_QWTPLOT3D )
find_library ( QWTPLOT3D_LIBRARY
    NAMES qwtplot3d
    HINTS ${QT_LIBRARY_DIR}
  )
  if( NOT QWTPLOT3D_LIBRARY )
    set( BUILD_QWTPLOT3D ON CACHE BOOL "Build QwtPlot3D using own code" FORCE )
  endif()
endif()

if( NOT BUILD_QWTPLOT3D )
  find_path ( QWTPLOT3D_INCLUDE_DIR
    NAMES qwt3d_plot.h
    HINTS ${QT_INCLUDE_DIR}
    PATH_SUFFIXES qwtplot3d qwtplot3d-qt5
    REQUIRED
  )
  message( STATUS "Found QwtPlot3D: ${QWTPLOT3D_LIBRARY}, with include: ${QWTPLOT3D_INCLUDE_DIR}" )
else()
  # Build QwtPlot3D
  message( STATUS "QwtPlot3D: using own code" )
  add_subdirectory( qwtplot3d )
endif()

# liborigin
if( ORIGIN_IMPORT )
  find_package( PkgConfig )
  if( PkgConfig_FOUND )
    pkg_search_module( LIBORIGIN liborigin>=3.0.0 IMPORTED_TARGET GLOBAL )
  endif()
  if( NOT PkgConfig_FOUND OR NOT LIBORIGIN_FOUND )
    message( STATUS "liborigin: using own code" )
    add_subdirectory( liborigin )
  endif()
else()
  message( STATUS "liborigin not enabled" )
endif()
