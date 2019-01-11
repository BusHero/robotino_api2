IF(NOT DEFINED QT_USE_QTGUI)
	SET( QT_USE_QTGUI 1 )
ENDIF()

IF(NOT DEFINED QT_USE_QTNETWORK)
	SET( QT_USE_QTNETWORK 1 )
ENDIF()

IF(NOT DEFINED QT_USE_QTXML)
	SET( QT_USE_QTXML 1 )
ENDIF()

IF( WIN32 )
	SET( QT5DIR "c:/Qt" )
	
	IF(MSVC14)
		SET( SUBPATH "5.12.0/msvc2017" )
	ELSE()
		SET( SUBPATH "5.9.1/msvc2013" )
	ENDIF()
	
	IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		SET(SUBPATH "${SUBPATH}_64")
	ENDIF()
	
	SET( CMAKE_PREFIX_PATH
		${CMAKE_PREFIX_PATH}
		${QT5DIR}/${SUBPATH}/lib/cmake
	)
ENDIF( WIN32 )

find_package( Qt5Widgets )
IF( Qt5Widgets_FOUND )
	MESSAGE( "Using Qt5" )
	
	find_package( Qt5Core REQUIRED )
	find_package( Qt5Gui REQUIRED )
	find_package( Qt5Network REQUIRED )
	find_package( Qt5Xml REQUIRED )
	find_package( Qt5Svg REQUIRED )
	find_package( Qt5Concurrent REQUIRED )
	
	IF( WIN32 )
		FIND_PATH(
		   QT_BINARY_DIR 
		   NAMES
		   moc
		   moc.exe
		   PATHS
		   ${QT5DIR}/${SUBPATH}/bin
		   NO_DEFAULT_PATH
		)
		
		FIND_PATH(
		   QT_PLUGIN_DIR 
		   NAMES
		   imageformats
		   PATHS
		   ${QT5DIR}/${SUBPATH}/plugins #/${ARCH}/${COMPILER_NAME}
		   NO_DEFAULT_PATH
		)
		
		SET( QT_RELEASE_DLLS
			${QT_BINARY_DIR}/Qt5Core.dll
			${QT_BINARY_DIR}/Qt5Gui.dll
			${QT_BINARY_DIR}/Qt5Widgets.dll
			${QT_BINARY_DIR}/Qt5Network.dll
			${QT_BINARY_DIR}/Qt5Xml.dll
			${QT_BINARY_DIR}/Qt5Svg.dll
			${QT_BINARY_DIR}/Qt5Concurrent.dll
			)
		
		SET( QT_DEBUG_DLLS
			${QT_BINARY_DIR}/Qt5Cored.dll
			${QT_BINARY_DIR}/Qt5Guid.dll
			${QT_BINARY_DIR}/Qt5Widgetsd.dll
			${QT_BINARY_DIR}/Qt5Networkd.dll
			${QT_BINARY_DIR}/Qt5Xmld.dll
			${QT_BINARY_DIR}/Qt5Svgd.dll
			${QT_BINARY_DIR}/Qt5Concurrentd.dll
		)
	ENDIF( WIN32 )
	
	MACRO(MY_SIMPLE_WRAP_CPP )
		QT5_WRAP_CPP( ${ARGN} )
	ENDMACRO()
	
	MACRO( MY_ADD_RESOURCES )
		QT5_ADD_RESOURCES( ${ARGN} )
	ENDMACRO( MY_ADD_RESOURCES )
	
	INCLUDE_DIRECTORIES(
		${Qt5Core_INCLUDE_DIRS}
		${Qt5Gui_INCLUDE_DIRS}
		${Qt5Widgets_INCLUDE_DIRS}
		${Qt5Network_INCLUDE_DIRS}
		${Qt5Xml_INCLUDE_DIRS}
		${Qt5Svg_INCLUDE_DIRS}
		${Qt5Concurrent_INCLUDE_DIRS}
	)

	IF( WIN32 )
		SET( QT_QTMAIN_LIBRARY Qt5::WinMain )
	ENDIF()

	SET( QT_QTCORE_LIBRARY Qt5::Core )
	SET( QT_QTGUI_LIBRARY Qt5::Gui Qt5::Widgets )
	SET( QT_QTNETWORK_LIBRARY Qt5::Network )
	SET( QT_QTXML_LIBRARY Qt5::Xml )
	SET( QT_QTSVG_LIBRARY Qt5::Svg )
	SET( QT_QTWEBKIT_LIBRARY Qt5::WebKit Qt5::WebKitWidgets )
	SET( QT_QTPRINTSUPPORT_LIBRARY Qt5::PrintSupport )
	SET( QT_QTCONCURRENT_LIBRARY Qt5::Concurrent )

	FOREACH( var ${Qt5Core_LIBRARIES} )
		SET( QT_LIBRARIES ${QT_LIBRARIES} ${var} )
	ENDFOREACH()

	FOREACH( var ${Qt5Gui_LIBRARIES} )
		SET( QT_LIBRARIES ${QT_LIBRARIES} ${var} )
	ENDFOREACH()

	FOREACH( var ${Qt5Widgets_LIBRARIES} )
		SET( QT_LIBRARIES ${QT_LIBRARIES} ${var} )
	ENDFOREACH()

	FOREACH( var ${Qt5Network_LIBRARIES} )
		SET( QT_LIBRARIES ${QT_LIBRARIES} ${var} )
	ENDFOREACH()

	FOREACH( var ${Qt5Xml_LIBRARIES} )
		SET( QT_LIBRARIES ${QT_LIBRARIES} ${var} )
	ENDFOREACH()

	FOREACH( var ${Qt5Concurrent_LIBRARIES} )
		SET( QT_LIBRARIES ${QT_LIBRARIES} ${var} )
	ENDFOREACH()
	
	FOREACH( var ${Qt5Svg_LIBRARIES} )
		SET( QT_LIBRARIES ${QT_LIBRARIES} ${var} )
	ENDFOREACH()
	
	SET(QT5_FOUND TRUE)
ELSE()
	MESSAGE( "Using Qt4" )
	find_package( Qt4 REQUIRED )
	
	MACRO (MY_SIMPLE_WRAP_CPP )
		QT4_WRAP_CPP( ${ARGN} )
	ENDMACRO (MY_SIMPLE_WRAP_CPP)
	
	MACRO( MY_ADD_RESOURCES )
		QT4_ADD_RESOURCES( ${ARGN} )
	ENDMACRO( MY_ADD_RESOURCES files )
	
	INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTNETWORK_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} )
	ADD_DEFINITIONS(${QT_DEFINITIONS})
	
	SET( QT_BINARY_DIR "${QT_DOC_DIR}/../bin" )
	SET( QT_RELEASE_DLLS "${QT_BINARY_DIR}/QtCore4.dll" "${QT_BINARY_DIR}/QtNetwork4.dll" "${QT_BINARY_DIR}/QtXml4.dll" "${QT_BINARY_DIR}/QtGui4.dll" )
	SET( QT_DEBUG_DLLS "${QT_BINARY_DIR}/QtCored4.dll" "${QT_BINARY_DIR}/QtNetworkd4.dll" "${QT_BINARY_DIR}/QtXmld4.dll" "${QT_BINARY_DIR}/QtGuid4.dll" )
ENDIF()

IF( WIN32 )
	FOREACH( var ${QT_RELEASE_DLLS} )
#TODO this is not working (access rights missing!)
		CONFIGURE_FILE( ${var} "${GLOBAL_TARGET_DIR}/release" COPYONLY )
	ENDFOREACH( var )
	FOREACH( var ${QT_DEBUG_DLLS} )
		CONFIGURE_FILE( ${var} "${GLOBAL_TARGET_DIR}/debug" COPYONLY )
	ENDFOREACH( var )
ENDIF( WIN32 )

SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG QT_DEBUG)
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE QT_NO_DEBUG)
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE QT_NO_DEBUG_OUTPUT )
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE QT_NO_WARNING_OUTPUT )
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELWITHDEBINFO QT_NO_DEBUG)
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_MINSIZEREL QT_NO_DEBUG)
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_MINSIZEREL QT_NO_DEBUG_OUTPUT )
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_MINSIZEREL QT_NO_WARNING_OUTPUT )

ADD_DEFINITIONS( -DHAVE_QT )

IF( QT5_FOUND )
	MACRO (MY_WRAP_CPP outfiles prefix)
		# get include dirs
		qt5_get_moc_flags(moc_includes)

		FOREACH (it ${ARGN})
			GET_FILENAME_COMPONENT(it ${it} ABSOLUTE)
			GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)

			SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/moc_${prefix}_${outfile}.cxx)
			IF( NOT WIN32 )
				SET( ENV{LD_LIBRARY_PATH} ${QT_LIBRARY_DIR} )
			ENDIF( NOT WIN32 )
			ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
				COMMAND ${Qt5Core_MOC_EXECUTABLE}
				ARGS -nw ${moc_includes} -o ${outfile} -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED ${it}
				DEPENDS ${it}
			)
			SET(${outfiles} ${${outfiles}} ${outfile})
		ENDFOREACH(it)
	ENDMACRO (MY_WRAP_CPP)
ELSE()
	# Modified copy from FindQt4.cmake since the QT4_WRAP_CPP macro does not
	# generate unique moc names for files with identical names residing in
	# different subdirectories
	MACRO (MY_WRAP_CPP outfiles prefix)
		# get include dirs
		IF ( COMMAND QT4_GET_MOC_INC_DIRS )
			QT4_GET_MOC_INC_DIRS(moc_includes)
		ELSE ( COMMAND QT4_GET_MOC_INC_DIRS )
			IF ( COMMAND QT4_GET_MOC_FLAGS )
				QT4_GET_MOC_FLAGS(moc_includes)
			ELSE ( COMMAND QT4_GET_MOC_FLAGS )
				message("Your CMake version is incompatible!")
			ENDIF ( COMMAND QT4_GET_MOC_FLAGS )
		ENDIF ( COMMAND QT4_GET_MOC_INC_DIRS )


		FOREACH (it ${ARGN})
			GET_FILENAME_COMPONENT(it ${it} ABSOLUTE)
			GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)

			SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/moc_${prefix}_${outfile}.cxx)
			IF( NOT WIN32 )
				SET( ENV{LD_LIBRARY_PATH} ${QT_LIBRARY_DIR} )
			ENDIF( NOT WIN32 )
			ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
				COMMAND ${QT_MOC_EXECUTABLE}
				ARGS -nw ${moc_includes} -o ${outfile} ${it}
				DEPENDS ${it}
			)
			SET(${outfiles} ${${outfiles}} ${outfile})
		ENDFOREACH(it)
	ENDMACRO (MY_WRAP_CPP)
ENDIF()

IF( WIN32 )
	INSTALL(
		FILES
			${QT_RELEASE_DLLS}
			DESTINATION bin
	)
		
	IF(QT_PLUGIN_DIR)
	INSTALL(
		FILES
		"${QT_PLUGIN_DIR}/platforms/qwindows.dll"
		DESTINATION bin/platforms
	)
	ENDIF()
ENDIF( WIN32 )