
set( CMAKE_AUTOMOC OFF )
set( CMAKE_AUTOUIC OFF )
set( CMAKE_AUTORCC OFF )

set( SRCS
  "main.cpp"
  "applicationWindow.cpp"
  "readWriteProject.cpp"
  "fft.cpp"
  "menus.cpp"
  "arrowMarker.cpp"
  )
if( NOT WIN32 )
  list( APPEND SRCS
    "testPaintDevice.cpp"
    "3dplot.cpp"
    )
endif()

add_executable( unittest ${SRCS} )

find_package( GTest REQUIRED )

target_link_libraries( unittest libscidavis GTest::GTest )

if( NOT WIN32 )
  target_link_libraries( unittest pthread )
endif()

if( ORIGIN_IMPORT )
  target_compile_definitions( unittest PRIVATE ORIGIN_IMPORT )
endif()

if( MULTI_CONFIG )
  target_include_directories( unittest PRIVATE
    "${CMAKE_BINARY_DIR}/libscidavis/libscidavis_autogen/include_$<CONFIG>"
    )
else()
  target_include_directories( unittest PRIVATE
    "${CMAKE_BINARY_DIR}/libscidavis/libscidavis_autogen/include"
    )
endif()

include(GoogleTest)
gtest_discover_tests( unittest DISCOVERY_MODE PRE_TEST )

set( TestDataFiles
  "3dplot.sciprj"
  "3dplotTable.sciprj"
  "testProject.sciprj"
  "Histo.opj"
  "USstates.opj"
  )
file( COPY ${TestDataFiles} DESTINATION . )

if( NOT (SEARCH_FOR_UPDATES OR DOWNLOAD_LINKS) )

  set( menu_files defaultMenus_en.menudat appWithGraphen.menudat )

  foreach( menu ${menu_files} )
    get_filename_component( test_name ${menu} NAME_WLE )
    add_test( NAME menuData_${test_name} COMMAND diff -q ${menu} ${CMAKE_CURRENT_SOURCE_DIR}/menuData/${menu} )
    set_tests_properties( menuData_${test_name} PROPERTIES DEPENDS unittest )
  endforeach()

endif()

if( SCRIPTING_PYTHON )
  
  file( COPY pythonTests/ DESTINATION ./tmp )
  file( COPY ../scidavis-logo.png DESTINATION ./tmp )

  file( GLOB pythonTests pythonTests/*.py )

  foreach( pythonTest ${pythonTests} )
    get_filename_component( test_name ${pythonTest} NAME_WLE )
    add_test( NAME python_${test_name}
      COMMAND scidavis -l=en -x ${pythonTest}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tmp
    )
  endforeach()

endif()
