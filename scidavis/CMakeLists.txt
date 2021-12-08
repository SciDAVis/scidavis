set( RESOURCES
  "appicons.qrc"
  "icons.qrc"
  )

if( WIN32 )
  list( APPEND RESOURCES "scidavis.rc" )
endif()

# Translation Files
file( COPY translations DESTINATION . )
set( TS_FILES
  "${CMAKE_CURRENT_BINARY_DIR}/translations/scidavis_de.ts"
  "${CMAKE_CURRENT_BINARY_DIR}/translations/scidavis_es.ts"
  "${CMAKE_CURRENT_BINARY_DIR}/translations/scidavis_fr.ts"
  "${CMAKE_CURRENT_BINARY_DIR}/translations/scidavis_ru.ts"
  "${CMAKE_CURRENT_BINARY_DIR}/translations/scidavis_ja.ts"
  "${CMAKE_CURRENT_BINARY_DIR}/translations/scidavis_sv.ts"
  "${CMAKE_CURRENT_BINARY_DIR}/translations/scidavis_pt-br.ts"
  "${CMAKE_CURRENT_BINARY_DIR}/translations/scidavis_cs.ts"
  "${CMAKE_CURRENT_BINARY_DIR}/translations/scidavis_cs-alt.ts"
  "${CMAKE_CURRENT_BINARY_DIR}/translations/scidavis_pl.ts"
  "${CMAKE_CURRENT_BINARY_DIR}/translations/scidavis_zh-cn.ts"
  "${CMAKE_CURRENT_BINARY_DIR}/translations/scidavis_zh-tw.ts"
  )
qt5_create_translation( QM_FILES "${CMAKE_SOURCE_DIR}/libscidavis" ${TS_FILES} )

add_executable( scidavis
  WIN32
  "src/main.cpp"
  "${RESOURCES}"
  "${QM_FILES}"
  )

target_link_libraries( scidavis libscidavis )

if( MULTI_CONFIG )
  target_include_directories( scidavis PRIVATE
    "${CMAKE_BINARY_DIR}/libscidavis/libscidavis_autogen/include_$<CONFIG>"
    )
else()
  target_include_directories( scidavis PRIVATE
    "${CMAKE_BINARY_DIR}/libscidavis/libscidavis_autogen/include"
    )
endif()

if( SCRIPTING_PYTHON )

  if( MULTI_CONFIG )
    file( GENERATE OUTPUT $<CONFIG>/scidavisrc.py INPUT scidavisrc.py )
    add_custom_command( OUTPUT scidavisrc.pyc
      COMMAND ${Python3_EXECUTABLE} -m compileall -b scidavisrc.py
      WORKING_DIRECTORY $<CONFIG>
      COMMENT "Compiling python files"
      )
    add_custom_target( generate_pyc_files
      DEPENDS $<CONFIG>/scidavisrc.pyc
      )
  else()
    file( COPY scidavisrc.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
    add_custom_command( OUTPUT scidavisrc.pyc
      COMMAND ${Python3_EXECUTABLE} -m compileall -b scidavisrc.py
      COMMENT "Compiling python files"
      )
    add_custom_target( generate_pyc_files
      DEPENDS scidavisrc.pyc
      )
  endif()

  add_dependencies( scidavis generate_pyc_files )

endif()

if (NOT CMAKE_CROSS_COMPILING)
    if( WIN32 )
      install( TARGETS scidavis RUNTIME DESTINATION . )
      install( FILES ${QM_FILES} DESTINATION translations )
    else()
      install( TARGETS scidavis RUNTIME DESTINATION bin )
      install( FILES ${QM_FILES}  DESTINATION share/scidavis/translations )
    endif()
    
    if( RUNTIME_DIR )
      set( EXCLUDE_DLL "system32/*" "shell32*" "kernel32*" )
      install( CODE "
        file( GET_RUNTIME_DEPENDENCIES
          RESOLVED_DEPENDENCIES_VAR RESOLVED_DEPENDENCIES
          UNRESOLVED_DEPENDENCIES_VAR UNRESOLVED_DEPENDENCIES
          CONFLICTING_DEPENDENCIES_PREFIX CONFLICTING_DEPENDENCIES
          EXECUTABLES \"\${CMAKE_INSTALL_PREFIX}/scidavis.exe\"
          DIRECTORIES \"${RUNTIME_DIR}\"
          POST_EXCLUDE_REGEXES ${EXCLUDE_DLL}
          )
        foreach( lib \${RESOLVED_DEPENDENCIES} )
          execute_process(
            COMMAND \"${CMAKE_COMMAND}\" -E
            copy \${lib} \"\${CMAKE_INSTALL_PREFIX}\"
            )
        endforeach()
        ")
    endif()
    
    if( WIN32 )
      set( CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION . )
      include(Windeployqt)
      windeployqt( scidavis . )
    endif()
    
    # Icons
    
    if( UNIX )
      install( FILES icons/scidavis.svg DESTINATION share/icons/hicolor/scalable/apps )
    
      foreach( res IN ITEMS 16 22 32 48 64 128 )
        install( FILES icons/hicolor-${res}/scidavis.png
          DESTINATION share/icons/hicolor/${res}x${res}/apps )
      endforeach()
    
      foreach( res IN ITEMS 16 22 32 )
        install( FILES icons/locolor-${res}/scidavis.png
          DESTINATION share/icons/locolor/${res}x${res}/apps )
      endforeach()
    endif()
    
    # Application Files
    if( UNIX )
      install( FILES scidavis.desktop DESTINATION share/applications )
      install( FILES scidavis.xml DESTINATION share/mime/packages )
      install( FILES scidavis.1 DESTINATION share/man/man1 )
      install( FILES scidavis.appdata.xml DESTINATION share/metainfo )
      install( FILES x-sciprj.desktop DESTINATION share/mimelnk/application )
    endif()
    
    if( SCRIPTING_PYTHON )
      if(NOT PYTHON_SCRIPTDIR)
        set(PYTHON_SCRIPTDIR etc)
      endif()
      if( MULTI_CONFIG )
        install( FILES scidavisrc.py ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/scidavisrc.pyc DESTINATION ${PYTHON_SCRIPTDIR} )
      else()
        install( FILES scidavisrc.py ${CMAKE_CURRENT_BINARY_DIR}/scidavisrc.pyc DESTINATION ${PYTHON_SCRIPTDIR} )
      endif()
      install( FILES scidavisUtil.py DESTINATION share/scidavis )
    endif()
endif()
