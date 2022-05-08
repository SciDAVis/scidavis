###################### HEADERS ##############################################
set( HEADERS
  "src/ApplicationWindow.h"
  "src/IconLoader.h"
  "src/globals.h"
  "src/Graph.h"
  "src/Graph3D.h"
  "src/Table.h"
  "src/CurvesDialog.h"
  "src/PlotDialog.h"
  "src/Plot3DDialog.h"
  "src/PlotWizard.h"
  "src/ExportDialog.h"
  "src/AxesDialog.h"
  "src/PolynomFitDialog.h"
  "src/ExpDecayDialog.h"
  "src/FunctionDialog.h"
  "src/FitDialog.h"
  "src/SurfaceDialog.h"
  "src/TextDialog.h"
  "src/LineDialog.h"
  "src/ScalePicker.h"
  "src/TitlePicker.h"
  "src/CanvasPicker.h"
  "src/PlotCurve.h"
  "src/QwtErrorPlotCurve.h"
  "src/QwtPieCurve.h"
  "src/ErrDialog.h"
  "src/Legend.h"
  "src/ArrowMarker.h"
  "src/ImageMarker.h"
  "src/ImageDialog.h"
  "src/fit_gsl.h"
  "src/MultiLayer.h"
  "src/LayerDialog.h"
  "src/IntDialog.h"
  "src/Bar.h"
  "src/Cone3D.h"
  "src/ConfigDialog.h"
  "src/QwtBarCurve.h"
  "src/BoxCurve.h"
  "src/QwtHistogram.h"
  "src/VectorCurve.h"
  "src/ScaleDraw.h"
  "src/Matrix.h"
  "src/DataSetDialog.h"
  "src/MyParser.h"
  "src/SymbolBox.h"
  "src/PatternBox.h"
  "src/SymbolDialog.h"
  "src/Plot.h"
  "src/ColorButton.h"
  "src/AssociationsDialog.h"
  "src/RenameWindowDialog.h"
  "src/MyWidget.h"
  "src/InterpolationDialog.h"
  "src/ImportASCIIDialog.h"
  "src/ImageExportDialog.h"
  "src/SmoothCurveDialog.h"
  "src/FilterDialog.h"
  "src/FFTDialog.h"
  "src/Note.h"
  "src/Folder.h"
  "src/FindDialog.h"
  "src/ScriptingEnv.h"
  "src/Script.h"
  "src/ScriptEdit.h"
  "src/FunctionCurve.h"
  "src/Fit.h"
  "src/MultiPeakFit.h"
  "src/ExponentialFit.h"
  "src/PolynomialFit.h"
  "src/NonLinearFit.h"
  "src/PluginFit.h"
  "src/SigmoidalFit.h"
  "src/customevents.h"
  "src/ScriptingLangDialog.h"
  "src/TextFormatButtons.h"
  "src/TableStatistics.h"
  "src/Spectrogram.h"
  "src/ColorMapEditor.h"
  "src/SelectionMoveResizer.h"
  "src/Filter.h"
  "src/Differentiation.h"
  "src/Integration.h"
  "src/Interpolation.h"
  "src/SmoothFilter.h"
  "src/FFTFilter.h"
  "src/FFT.h"
  "src/Convolution.h"
  "src/Correlation.h"
  "src/PlotToolInterface.h"
  "src/ScreenPickerTool.h"
  "src/DataPickerTool.h"
  "src/RangeSelectorTool.h"
  "src/TranslateCurveTool.h"
  "src/MultiPeakFitTool.h"
  "src/CurveRangeDialog.h"
  "src/LineProfileTool.h"
  "src/PlotEnrichement.h"
  "src/ExtensibleFileDialog.h"
  "src/OpenProjectDialog.h"
  "src/Grid.h"
  "src/DataPointPicker.h"
  "src/TeXTableSettings.h"
  "src/TeXTableSettingsWidget.h"
  "src/TeXTableExportDialog.h"
  "src/PenWidget.h"
  "src/QStringStdString.h"
  "src/future/core/AbstractAspect.h"
  "src/future/core/AbstractPart.h"
  "src/future/core/AspectPrivate.h"
  "src/future/core/aspectcommands.h"
  "src/future/core/future_Folder.h"
  "src/future/core/Project.h"
  "src/future/core/ProjectConfigPage.h"
  "src/future/core/PartMdiView.h"
  "src/future/core/AbstractColumn.h"
  "src/future/core/column/Column.h"
  "src/future/core/column/ColumnPrivate.h"
  "src/future/core/column/columncommands.h"
  "src/future/core/AbstractFilter.h"
  "src/future/core/AbstractSimpleFilter.h"
  "src/future/core/datatypes/SimpleCopyThroughFilter.h"
  "src/future/core/datatypes/DateTime2DoubleFilter.h"
  "src/future/core/datatypes/DateTime2StringFilter.h"
  "src/future/core/datatypes/DayOfWeek2DoubleFilter.h"
  "src/future/core/datatypes/Double2DateTimeFilter.h"
  "src/future/core/datatypes/Double2DayOfWeekFilter.h"
  "src/future/core/datatypes/Double2MonthFilter.h"
  "src/future/core/datatypes/Double2StringFilter.h"
  "src/future/core/datatypes/NumericDateTimeBaseFilter.h"
  "src/future/core/datatypes/Month2DoubleFilter.h"
  "src/future/core/datatypes/String2DateTimeFilter.h"
  "src/future/core/datatypes/String2DayOfWeekFilter.h"
  "src/future/core/datatypes/String2DoubleFilter.h"
  "src/future/core/datatypes/String2MonthFilter.h"
  "src/future/lib/macros.h"
  "src/future/lib/XmlStreamReader.h"
  "src/future/lib/ActionManager.h"
  "src/future/lib/ConfigPageWidget.h"
  "src/future/lib/Interval.h"
  "src/future/lib/IntervalAttribute.h"
  "src/future/matrix/future_Matrix.h"
  "src/future/matrix/MatrixModel.h"
  "src/future/matrix/MatrixView.h"
  "src/future/matrix/matrixcommands.h"
  "src/future/table/future_Table.h"
  "src/future/table/TableModel.h"
  "src/future/table/TableView.h"
  "src/future/table/tablecommands.h"
  "src/future/table/TableDoubleHeaderView.h"
  "src/future/table/TableItemDelegate.h"
  "src/future/table/TableCommentsHeaderModel.h"
  "src/future/table/future_SortDialog.h"
  "src/future/table/AsciiTableImportFilter.h"
  "src/future/core/AbstractImportFilter.h"
  "src/future/core/interfaces.h"
  "src/MuParserScript.h"
  "src/MuParserScripting.h"
)

###################### SOURCES ##############################################
set( SRCS
  "src/ApplicationWindow.cpp"
  "src/IconLoader.cpp"
  "src/Graph.cpp"
  "src/Graph3D.cpp"
  "src/Table.cpp"
  "src/CurvesDialog.cpp"
  "src/PlotDialog.cpp"
  "src/Plot3DDialog.cpp"
  "src/PlotWizard.cpp"
  "src/ExportDialog.cpp"
  "src/AxesDialog.cpp"
  "src/PolynomFitDialog.cpp"
  "src/TextDialog.cpp"
  "src/ScalePicker.cpp"
  "src/TitlePicker.cpp"
  "src/CanvasPicker.cpp"
  "src/ExpDecayDialog.cpp"
  "src/FunctionDialog.cpp"
  "src/FitDialog.cpp"
  "src/SurfaceDialog.cpp"
  "src/LineDialog.cpp"
  "src/PlotCurve.cpp"
  "src/QwtErrorPlotCurve.cpp"
  "src/QwtPieCurve.cpp"
  "src/ErrDialog.cpp"
  "src/Legend.cpp"
  "src/ArrowMarker.cpp"
  "src/ImageMarker.cpp"
  "src/ImageDialog.cpp"
  "src/MultiLayer.cpp"
  "src/LayerDialog.cpp"
  "src/IntDialog.cpp"
  "src/Bar.cpp"
  "src/Cone3D.cpp"
  "src/DataSetDialog.cpp"
  "src/ConfigDialog.cpp"
  "src/QwtBarCurve.cpp"
  "src/BoxCurve.cpp"
  "src/QwtHistogram.cpp"
  "src/VectorCurve.cpp"
  "src/Matrix.cpp"
  "src/MyParser.cpp"
  "src/SymbolBox.cpp"
  "src/PatternBox.cpp"
  "src/SymbolDialog.cpp"
  "src/Plot.cpp"
  "src/ColorButton.cpp"
  "src/AssociationsDialog.cpp"
  "src/RenameWindowDialog.cpp"
  "src/MyWidget.cpp"
  "src/InterpolationDialog.cpp"
  "src/fit_gsl.cpp"
  "src/SmoothCurveDialog.cpp"
  "src/FilterDialog.cpp"
  "src/FFTDialog.cpp"
  "src/Note.cpp"
  "src/Folder.cpp"
  "src/FindDialog.cpp"
  "src/TextFormatButtons.cpp"
  "src/ScriptEdit.cpp"
  "src/ImportASCIIDialog.cpp"
  "src/ImageExportDialog.cpp"
  "src/ScaleDraw.cpp"
  "src/FunctionCurve.cpp"
  "src/Fit.cpp"
  "src/MultiPeakFit.cpp"
  "src/ExponentialFit.cpp"
  "src/PolynomialFit.cpp"
  "src/PluginFit.cpp"
  "src/NonLinearFit.cpp"
  "src/SigmoidalFit.cpp"
  "src/ScriptingEnv.cpp"
  "src/Script.cpp"
  "src/ScriptingLangDialog.cpp"
  "src/TableStatistics.cpp"
  "src/Spectrogram.cpp"
  "src/ColorMapEditor.cpp"
  "src/SelectionMoveResizer.cpp"
  "src/Filter.cpp"
  "src/Differentiation.cpp"
  "src/Integration.cpp"
  "src/Interpolation.cpp"
  "src/SmoothFilter.cpp"
  "src/FFTFilter.cpp"
  "src/FFT.cpp"
  "src/Convolution.cpp"
  "src/Correlation.cpp"
  "src/ScreenPickerTool.cpp"
  "src/DataPickerTool.cpp"
  "src/RangeSelectorTool.cpp"
  "src/TranslateCurveTool.cpp"
  "src/MultiPeakFitTool.cpp"
  "src/CurveRangeDialog.cpp"
  "src/LineProfileTool.cpp"
  "src/PlotEnrichement.cpp"
  "src/ExtensibleFileDialog.cpp"
  "src/OpenProjectDialog.cpp"
  "src/Grid.cpp"
  "src/globals.cpp"
  "src/DataPointPicker.cpp"
  "src/TeXTableSettings.cpp"
  "src/TeXTableSettingsWidget.cpp"
  "src/TeXTableExportDialog.cpp"
  "src/version.cpp"
  "src/PenWidget.cpp"
  "src/future/core/AbstractAspect.cpp"
  "src/future/core/AbstractPart.cpp"
  "src/future/core/AspectPrivate.cpp"
  "src/future/core/future_Folder.cpp"
  "src/future/core/PartMdiView.cpp"
  "src/future/core/Project.cpp"
  "src/future/core/column/Column.cpp"
  "src/future/core/column/ColumnPrivate.cpp"
  "src/future/core/column/columncommands.cpp"
  "src/future/core/datatypes/DateTime2StringFilter.cpp"
  "src/future/core/datatypes/String2DateTimeFilter.cpp"
  "src/future/core/datatypes/Double2StringFilter.cpp"
  "src/future/core/datatypes/NumericDateTimeBaseFilter.cpp"
  "src/future/core/AbstractSimpleFilter.cpp"
  "src/future/core/AbstractFilter.cpp"
  "src/future/core/ProjectConfigPage.cpp"
  "src/future/lib/XmlStreamReader.cpp"
  "src/future/lib/ActionManager.cpp"
  "src/future/lib/ConfigPageWidget.cpp"
  "src/future/matrix/future_Matrix.cpp"
  "src/future/matrix/MatrixModel.cpp"
  "src/future/matrix/MatrixView.cpp"
  "src/future/matrix/matrixcommands.cpp"
  "src/future/table/future_Table.cpp"
  "src/future/table/TableModel.cpp"
  "src/future/table/TableView.cpp"
  "src/future/table/tablecommands.cpp"
  "src/future/table/TableDoubleHeaderView.cpp"
  "src/future/table/TableItemDelegate.cpp"
  "src/future/table/TableCommentsHeaderModel.cpp"
  "src/future/table/future_SortDialog.cpp"
  "src/future/table/AsciiTableImportFilter.cpp"
  "src/MuParserScript.cpp"
  "src/MuParserScripting.cpp"
  )

###################### FORMS ##############################################
set( FORMS "src/SciDAVisAbout.ui"
  "src/future/matrix/matrixcontroltabs.ui"
  "src/future/core/ProjectConfigPage.ui"
  "src/future/table/controltabs.ui"
  "src/future/table/DimensionsDialog.ui"
  )

if( ORIGIN_IMPORT )
  list( APPEND HEADERS "src/importOPJ.h" )
  list( APPEND SRCS "src/importOPJ.cpp" )
endif()

add_library( libscidavis STATIC
  ${SRCS}
  ${HEADERS}
  ${FORMS}
  )
  
if( MSVC )
	foreach(_source IN ITEMS ${SRCS})
		get_filename_component(_source_path "${_source}" PATH)
		string(REGEX REPLACE "^src/?([^/]*)" "\\1" _source_path ${_source_path})
		if (NOT ${_source_path} STREQUAL "")
			source_group("Source Files/${_source_path}" FILES "${_source}")
		endif()
	endforeach()

	foreach(_source IN ITEMS ${HEADERS})
		get_filename_component(_source_path "${_source}" PATH)
		string(REGEX REPLACE "^src/?([^/]*)" "\\1" _source_path ${_source_path})
		if (NOT ${_source_path} STREQUAL "")
			source_group("Header Files/${_source_path}" FILES "${_source}")
		endif()
	endforeach()

	foreach(_source IN ITEMS ${FORMS})
		get_filename_component(_source_path "${_source}" PATH)
		string(REGEX REPLACE "^src/?([^/]*)" "\\1" _source_path ${_source_path})
		if (NOT ${_source_path} STREQUAL "")
			source_group("Forms/${_source_path}" FILES "${_source}")
		else()
			source_group("Forms" FILES "${_source}")
		endif()
		
	endforeach()
endif()

set_target_properties(libscidavis PROPERTIES OUTPUT_NAME scidavis)

target_link_libraries( libscidavis
  ZLIB::ZLIB
  GSL::gsl
  GSL::gslcblas
  OpenGL::GL
  OpenGL::GLU
  ${MUPARSER_LIB}
  minigzip
  Qt5::Core
  Qt5::Gui
  Qt5::PrintSupport
  Qt5::OpenGL
  Qt5::Svg
  m
  )

target_include_directories( libscidavis PUBLIC ${MUPARSER_INCLUDE_DIR} )

if( NOT BUILD_QWT )
  target_include_directories( libscidavis PUBLIC ${QWT_INCLUDE_DIR} )
  target_link_libraries( libscidavis ${QWT_LIBRARY} )
else()
  target_include_directories( libscidavis PUBLIC
    "${CMAKE_SOURCE_DIR}/3rdparty/qwt5-qt5/src" )
  target_link_libraries( libscidavis qwt5-qt5 )
endif()

if( NOT BUILD_QWTPLOT3D )
  target_include_directories( libscidavis PUBLIC ${QWTPLOT3D_INCLUDE_DIR} )
  target_link_libraries( libscidavis ${QWTPLOT3D_LIBRARY} )
else()
  target_link_libraries( libscidavis qwtplot3d )
endif()

if( SCRIPTING_PYTHON )
  target_include_directories( libscidavis PUBLIC ${Python3_INCLUDE_DIRS} )
  target_link_libraries( libscidavis  ${Python3_LIBRARIES} )

  # add sip generated files' directory to include path
  target_include_directories( libscidavis PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
endif()

target_compile_definitions( libscidavis PUBLIC
  LEGACY_CODE_0_2_x
  SCRIPTING_CONSOLE
  SCRIPTING_DIALOG
  )

if( WIN32 )
  target_compile_definitions( libscidavis PUBLIC
    TS_PATH="${CMAKE_INSTALL_PREFIX}/translations"
    DOC_PATH="${CMAKE_INSTALL_PREFIX}"
    PLUGIN_PATH="${CMAKE_INSTALL_PREFIX}/fitPlugins"
    )
else()
  target_compile_definitions( libscidavis PUBLIC
     TS_PATH="${CMAKE_INSTALL_DATAROOTDIR}/scidavis/translations"
     DOC_PATH="${CMAKE_INSTALL_DOCDIR}/scidavis"
     PLUGIN_PATH="${CMAKE_INSTALL_LIBDIR}/scidavis/plugins"
    )
endif()

if( WIN32 )
  target_compile_definitions( libscidavis PUBLIC
    DYNAMIC_MANUAL_PATH
    DYNAMIC_PLUGIN_PATH
    )
endif()

if( SEARCH_FOR_UPDATES )
  target_compile_definitions( libscidavis PUBLIC SEARCH_FOR_UPDATES )
endif()

if( DOWNLOAD_LINKS )
  target_compile_definitions( libscidavis PUBLIC DOWNLOAD_LINKS )
endif()

if( SCRIPTING_MUPARSER )
  target_compile_definitions( libscidavis PUBLIC SCRIPTING_MUPARSER )
endif()

if( ORIGIN_IMPORT )
  target_compile_definitions( libscidavis PUBLIC ORIGIN_IMPORT )
  if( LIBORIGIN_FOUND )
    target_link_libraries( libscidavis PkgConfig::LIBORIGIN )
  else()
    target_link_libraries( libscidavis origin_static )
    target_include_directories( libscidavis PUBLIC
      "${CMAKE_SOURCE_DIR}/3rdparty/liborigin" )
  endif()
endif()

if( SCRIPTING_PYTHON )
  target_compile_definitions( libscidavis PUBLIC
    PYTHON_CONFIG_PATH="${CMAKE_INSTALL_PREFIX}/etc"
    PYTHON_UTIL_PATH="${CMAKE_INSTALL_PREFIX}/share/scidavis"
    SCRIPTING_PYTHON
    )
  if( MINGW )
    get_filename_component( PYTHON_DIR ${Python3_EXECUTABLE} DIRECTORY )
    file( REAL_PATH "${PYTHON_DIR}/.." PYTHON_ROOT_DIR )
    set( PYTHONHOME "${PYTHON_ROOT_DIR}" CACHE PATH "Python Home" )
    message( STATUS "PYTHONHOME = ${PYTHONHOME}" )
    set_property( SOURCE "src/PythonScripting.cpp"
      APPEND
      PROPERTY COMPILE_DEFINITIONS
      PYTHONHOME=${PYTHONHOME}
      )
  endif()
endif()

if( SEARCH_FOR_UPDATE OR DOWNLOAD_LINKS )
  target_link_libraries( libscidavis Qt5::Network )
endif()

target_include_directories( libscidavis PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/future"
  )

if( SCRIPTING_PYTHON )

  if( SIP_VERSION VERSION_GREATER_EQUAL 5 )
    set( scidavis_SIP_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/python/scidavis )
  else()
    set( scidavis_SIP_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR} )
  endif()

  set(scidavis_PyQt_HDR ${scidavis_SIP_OUTPUT_DIR}/sipAPIscidavis.h)
  if( SIP_VERSION VERSION_GREATER_EQUAL 5 )
    set( scidavis_SIP_HDR ${CMAKE_CURRENT_BINARY_DIR}/python/sip.h )
    target_include_directories( libscidavis BEFORE PUBLIC
      "${CMAKE_CURRENT_BINARY_DIR}/python"
      "${scidavis_SIP_OUTPUT_DIR}"
    )
  endif()

  set_source_files_properties(
    ${scidavis_PyQt_HDR}
    PROPERTIES
    GENERATED ON
    )

  set(scidavis_PyQt_SRC
    ${scidavis_SIP_OUTPUT_DIR}/sipscidaviscmodule.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisApplicationWindow.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisGraph.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisArrowMarker.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisImageMarker.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisLegend.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisMultiLayer.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisTable.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisMatrix.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisMyWidget.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisScriptEdit.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisNote.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisPythonScript.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisPythonScripting.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisFolder.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisFit.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisExponentialFit.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisTwoExpFit.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisThreeExpFit.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisSigmoidalFit.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisGaussAmpFit.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisLorentzFit.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisNonLinearFit.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisPluginFit.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisMultiPeakFit.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisPolynomialFit.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisLinearFit.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisGaussFit.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisFilter.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisDifferentiation.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisIntegration.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisInterpolation.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisSmoothFilter.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisFFTFilter.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisFFT.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisCorrelation.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisConvolution.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisDeconvolution.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisAbstractAspect.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisColumn.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisQwtSymbol.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisQwtPlotCurve.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisQwtPlot.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisGrid.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisQList0100QDateTime.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisQList0101Folder.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisQList0101MyWidget.cpp
    ${scidavis_SIP_OUTPUT_DIR}/sipscidavisQList0101QwtPlotCurve.cpp
    )

  set_source_files_properties(
    ${scidavis_PyQt_SRC}
    PROPERTIES
    GENERATED ON
    )

  if( SIP_VERSION VERSION_GREATER_EQUAL 5 )
    get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
    add_custom_command(
      OUTPUT ${scidavis_PyQt_HDR} ${scidavis_PyQt_SRC} ${scidavis_SIP_HDR}
      COMMAND ${SIP_EXECUTABLE} --no-make --qmake=${_qmake_executable}
        --include-dir=${PyQt_INCLUDE_DIRS}
        --build-dir=${CMAKE_CURRENT_BINARY_DIR}/python
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/scidavis.sip
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
      )
  else()
    add_custom_command(
      OUTPUT ${scidavis_PyQt_HDR} ${scidavis_PyQt_SRC}
      COMMAND ${SIP_EXECUTABLE} -c . -I${PyQt_INCLUDE_DIRS} ${PyQt_FLAGS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/scidavis.sip
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/scidavis.sip
      )
  endif()

  if( SIP_VERSION VERSION_GREATER_EQUAL 5 )
    add_custom_target(generate_PyQt_source
      DEPENDS ${scidavis_PyQt_HDR} ${scidavis_PyQt_SRC} ${scidavis_SIP_HDR}
      )
  else()
    add_custom_target(generate_PyQt_source
      DEPENDS ${scidavis_PyQt_HDR} ${scidavis_PyQt_SRC}
      )
  endif()

  target_sources( libscidavis PRIVATE
    "src/PythonScript.h"
    "src/PythonScripting.h"
    ${scidavis_PyQt_HDR}
    "src/PythonScript.cpp"
    "src/PythonScripting.cpp"
    ${scidavis_PyQt_SRC}
    )

  add_dependencies(libscidavis generate_PyQt_source)

endif()
