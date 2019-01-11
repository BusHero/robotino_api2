IF( NOT QT4_MOC_EXECUTABLE )
	SET( QT4_MOC_EXECUTABLE "${QT_MOC_EXECUTABLE}" )
ENDIF( NOT QT4_MOC_EXECUTABLE )

# Modified copy from FindQt4.cmake since the QT4_WRAP_CPP macro does not
# generate unique moc names for files with identical names residing in
# different subdirectories
MACRO (MY_WRAP_CPP outfiles prefix)
	# get include dirs
	QT4_GET_MOC_FLAGS(moc_includes)

	FOREACH (it ${ARGN})
		GET_FILENAME_COMPONENT(it ${it} ABSOLUTE)
		GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)

		SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/moc_${prefix}_${outfile}.cxx)
		ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
		COMMAND ${QT4_MOC_EXECUTABLE}
		ARGS -nw ${moc_includes} -o ${outfile} ${it}
		DEPENDS ${it})
		SET(${outfiles} ${${outfiles}} ${outfile})
	ENDFOREACH(it)
ENDMACRO (MY_WRAP_CPP)

IF( WIN32 )
SET( QT4_BIN_PATH "${QT_DOC_DIR}/../bin" )

	#FILE( GLOB QT4_RELEASE_DLLS "${QT4_BIN_PATH}/*[abcefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789]4.dll" )
	#FILE( GLOB QT4_DEBUG_DLLS "${QT4_BIN_PATH}/*d4.dll" )

	SET( QT4_CORE_RELEASE_DLL "${QT4_BIN_PATH}/QtCore4.dll" )
	SET( QT4_CORE_DEBUG_DLL "${QT4_BIN_PATH}/QtCored4.dll" )

	SET( QT4_GUI_RELEASE_DLL "${QT4_BIN_PATH}/QtGui4.dll" )
	SET( QT4_GUI_DEBUG_DLL "${QT4_BIN_PATH}/QtGuid4.dll" )

	SET( QT4_NETWORK_RELEASE_DLL "${QT4_BIN_PATH}/QtNetwork4.dll" )
	SET( QT4_NETWORK_DEBUG_DLL "${QT4_BIN_PATH}/QtNetworkd4.dll" )

	SET( QT4_XML_RELEASE_DLL "${QT4_BIN_PATH}/QtXml4.dll" )
	SET( QT4_XML_DEBUG_DLL "${QT4_BIN_PATH}/QtXmld4.dll" )
	
	SET( QT4_SVG_RELEASE_DLL "${QT4_BIN_PATH}/QtSvg4.dll" )
	SET( QT4_SVG_DEBUG_DLL "${QT4_BIN_PATH}/QtSvgd4.dll" )

	SET( QT4_RELEASE_DLLS ${QT4_CORE_RELEASE_DLL} )
	SET( QT4_DEBUG_DLLS	${QT4_CORE_DEBUG_DLL} )

	IF( QT_USE_QTGUI )
		SET( QT4_RELEASE_DLLS ${QT4_RELEASE_DLLS} ${QT4_GUI_RELEASE_DLL} )
		SET( QT4_DEBUG_DLLS ${QT4_DEBUG_DLLS} ${QT4_GUI_DEBUG_DLL} )
	ENDIF( QT_USE_QTGUI )

	IF( QT_USE_QTXML )
		SET( QT4_RELEASE_DLLS ${QT4_RELEASE_DLLS} ${QT4_XML_RELEASE_DLL} )
		SET( QT4_DEBUG_DLLS ${QT4_DEBUG_DLLS} ${QT4_XML_DEBUG_DLL} )
	ENDIF( QT_USE_QTXML )

	IF( QT_USE_QTNETWORK )
		SET( QT4_RELEASE_DLLS ${QT4_RELEASE_DLLS} ${QT4_NETWORK_RELEASE_DLL} )
		SET( QT4_DEBUG_DLLS ${QT4_DEBUG_DLLS} ${QT4_NETWORK_DEBUG_DLL} )
	ENDIF( QT_USE_QTNETWORK )

	IF( QT_USE_QTSVG )
		SET( QT4_RELEASE_DLLS ${QT4_RELEASE_DLLS} ${QT4_SVG_RELEASE_DLL} )
		SET( QT4_DEBUG_DLLS ${QT4_DEBUG_DLLS} ${QT4_SVG_DEBUG_DLL} )
	ENDIF( QT_USE_QTSVG )

ENDIF( WIN32 )

MACRO( COPY_QT4_DLLS releaseTarget debugTarget )
  FOREACH( var ${QT4_RELEASE_DLLS} )
    CONFIGURE_FILE( ${var} ${releaseTarget} COPYONLY )
  ENDFOREACH( var )
  FOREACH( var ${QT4_DEBUG_DLLS} )
    CONFIGURE_FILE( ${var} ${debugTarget} COPYONLY )
  ENDFOREACH( var )
ENDMACRO( COPY_QT4_DLLS )