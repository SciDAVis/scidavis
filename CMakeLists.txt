cmake_minimum_required( VERSION 3.16 )
include(GNUInstallDirs)

# Target platform is Windows 10
if( CMAKE_GENERATOR STREQUAL "Visual Studio 15 2017" OR
    CMAKE_GENERATOR STREQUAL "Visual Studio 14 2015" )
  set(CMAKE_SYSTEM_VERSION 10.0)
endif()

cmake_policy(SET CMP0091 NEW)

project( scidavis
  VERSION 2.3.0
  DESCRIPTION "SciDAVis is a free application for Scientific Data Analysis and Visualization."
  HOMEPAGE_URL "https://scidavis.sourceforge.net"
  LANGUAGES CXX C )

set( CMAKE_CXX_STANDARD 17)
set( CMAKE_CXX_STANDARD_REQUIRED TRUE )
set( CMAKE_CXX_EXTENSIONS OFF )

set( CMAKE_C_STANDARD 11)
set( CMAKE_C_STANDARD_REQUIRED TRUE )
set( CMAKE_C_EXTENSIONS OFF )

get_property( MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG )

set( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )

# Target platform is Windows 10
if( CMAKE_GENERATOR STREQUAL "Visual Studio 15 2017" OR
    CMAKE_GENERATOR STREQUAL "Visual Studio 14 2015" )
  if (CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION VERSION_LESS 10)
    message(SEND_ERROR "Windows 10 SDK is not found! Please, install one or set \
      a CMAKE_WINDOWS_KITS_10_DIR environment variable to an absolute path to look \
      for Windows 10 SDKs. The directory is expected to contain Include/10.0.* directories.")
  else()
    message(STATUS "Use Windows 10 SDK version ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
  endif()
endif()

find_package( Qt5
  COMPONENTS
  Core
  Gui
  Widgets
  Svg
  PrintSupport
  Xml
  OpenGL
  LinguistTools
  REQUIRED
  )

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTORCC ON )

if( MSVC )
  # /wd4456 /wd4457 /wd4458 Silent "declaration of %1 hides %2 ..."
  # /wd4251 Silent dll-related warnings
  # /wd4127 Silent conditional expression is constant (Qt headers)
  # /wd4310 Silent cast truncates constant value (muParser headers)
  # /wd4996 strcpy is unsafe warnings, equiv to #define _SCL_SECURE_NO_WARNINGS
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /W4 /permissive- \
	 /wd4456 /wd4457 /wd4458 /wd4251 /wd4127 /wd4310 /wd4996" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /permissive-" )
  if( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-nonportable-include-path" )
  else()
    add_compile_options( "/MP" )
  endif()
else()
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -Wall -Wextra -pedantic" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic" )
endif()

if( APPLE )
  add_compile_definitions( GL_SILENCE_DEPRECATION )
endif()

# Searching for updates
option( SEARCH_FOR_UPDATES "Enable Searching for updates" ON )

# Download links
option( DOWNLOAD_LINKS "Enable Download links" ON )

if( SEARCH_FOR_UPDATES OR DOWNLOAD_LINKS )
  find_package( Qt5 COMPONENTS Network REQUIRED )
endif()

option( SCRIPTING_MUPARSER "Enable muParser Scripting" ON )

option( SCRIPTING_PYTHON "Enable Python Scripting" OFF )

option( BUILD_TESTS "Build tests" OFF )

if( SCRIPTING_PYTHON )
  if( MINGW )
    set( Python3_FIND_REGISTRY LAST )
  endif()
  find_package( Python3 COMPONENTS Interpreter Development REQUIRED )
  find_package( SIP REQUIRED )
  find_package( PyQt REQUIRED )
endif()

option( ORIGIN_IMPORT "Enable importing OriginLab project files" OFF )

# GSL
find_package( GSL REQUIRED )

# ZLIB
find_package( ZLIB "1.2.11" REQUIRED )

# OpenGL
find_package( OpenGL COMPONENTS OpenGL REQUIRED )

# muParser
if( SCRIPTING_MUPARSER )
find_library( MUPARSER_LIB
  NAMES muparser
  REQUIRED
  )
find_path( MUPARSER_INCLUDE_DIR
  NAMES "muParser.h"
  REQUIRED
  )
message( STATUS "Found muParser : ${MUPARSER_LIB} include: ${MUPARSER_INCLUDE_DIR}" )
endif()

add_subdirectory( 3rdparty )

add_subdirectory( libscidavis )

add_subdirectory( scidavis )

add_subdirectory( fitPlugins )

if( BUILD_TESTS )
  enable_testing()
  add_subdirectory( test )
endif()

# Documentation
set( DOC_FILES
  ChangeLog.md
  README.md
  gpl.txt
  license.rtf
  )

if( WIN32 )
  install( FILES ${DOC_FILES} DESTINATION . )
else()
  install( FILES ${DOC_FILES} DESTINATION ${CMAKE_INSTALL_DOCDIR} )
endif()

if( MSVC )
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "scidavis")
endif()

set( CPACK_PACKAGE_NAME "SciDAVis" )
set( CPACK_PACKAGE_VENDOR "High Performance Coders" )
set( CPACK_PACKAGE_DESCRIPTION "SciDAVis Installer" )
set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/gpl.txt" )
set( CPACK_PACKAGE_EXECUTABLES "scidavis;SciDAVis" )
set( CPACK_CREATE_DESKTOP_LINKS "scidavis" )
set( CPACK_STRIP_FILES ON )

# Source packaging
set( CPACK_SOURCE_IGNORE_FILES "/build/;CMakeLists\.txt\.user*;/\.git/" )

# WIX
set( CPACK_WIX_UPGRADE_GUID "d6f4ef98-3744-47a2-b581-d789db8a4d63" )
set( CPACK_WIX_LICENSE_RTF "${CMAKE_SOURCE_DIR}/license.rtf" )
set( CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/scidavis/icons/scidavis.ico" )

# NSIS
set( CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/scidavis/icons/scidavis.ico" )
set( CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON )
set( CPACK_NSIS_EXECUTABLES_DIRECTORY . )

include(CPack)
