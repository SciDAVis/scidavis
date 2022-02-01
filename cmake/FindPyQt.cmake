# FindPyQt
# -------

# Result Variables
# ^^^^^^^^^^^^^^^^

# This will define the following variables:

# ``PyQt_FOUND``
#   True if the system has PyQt sip files.
# ``PyQt_INCLUDE_DIRS``
#   The directory containing PyQt sip files.
# ``PyQt_FLAGS``
#   The required flags to compile generated c++ files.

# Cache Variables
# ^^^^^^^^^^^^^^^

# The following cache variables may also be set:

# ``PyQt_INCLUDE_DIR``
#   The directory containing PyQt sip files.

execute_process(
  COMMAND ${Python3_EXECUTABLE} -c "import sys; sys.stdout.write(sys.prefix)"
  OUTPUT_VARIABLE _Python3_PREFIX
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

set( _PyQt_HINTS 
  "${_Python3_PREFIX}/share/python3-sip"
  "${_Python3_PREFIX}/share/sip"
  "${Python3_SITELIB}"
  "${Python3_SITEARCH}"
  )
  
find_path( PyQt_INCLUDE_DIR
  NAMES "QtCore/QtCoremod.sip"
  HINTS ${_PyQt_HINTS}
  PATH_SUFFIXES "PyQt5" "PyQt5/bindings"
  )

execute_process(
  COMMAND ${Python3_EXECUTABLE} -c "from PyQt5.QtCore import PYQT_CONFIGURATION;import sys;sys.stdout.write(PYQT_CONFIGURATION['sip_flags'].replace(' ',';'))"
  OUTPUT_VARIABLE PyQt_FLAGS
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

execute_process(
  COMMAND ${Python3_EXECUTABLE} -c "from PyQt5.QtCore import PYQT_VERSION_STR;import sys;sys.stdout.write(PYQT_VERSION_STR)"
  OUTPUT_VARIABLE PyQt_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PyQt
  FOUND_VAR PyQt_FOUND
  REQUIRED_VARS
    PyQt_INCLUDE_DIR
    PyQt_FLAGS
  VERSION_VAR PyQt_VERSION
  )

if( PyQt_FOUND )
  set( PyQt_INCLUDE_DIRS ${PyQt_INCLUDE_DIR} )
endif()

mark_as_advanced( PyQt_INCLUDE_DIR )
