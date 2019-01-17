IF( WIN32 )
	IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		SET( ARCH x64 )
	ELSE( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		SET( ARCH win32 )
	ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
ELSE( WIN32 )
	IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		SET( ARCH linux64 )
	ELSE( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		SET( ARCH linux32 )
	ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
ENDIF( WIN32 )

IF( WIN32 )
	FIND_FILE( 
	  PCAN_DLL
	  Pcan_usb.dll 
	  ${EXTERNAL_DIR}/pcan_win/bin/${ARCH}
	  NO_DEFAULT_PATH 
	)

	FIND_FILE( 
	  PCAN2_DLL
	  Pcan_2usb.dll 
	  ${EXTERNAL_DIR}/pcan_win/bin/${ARCH}
	  NO_DEFAULT_PATH 
	)
	
	FIND_PATH(
		PCAN_INCLUDE_DIR 
		NAMES Pcan_usb.h
		PATHS ${EXTERNAL_DIR}/pcan_win/include
	)

	FIND_LIBRARY (
 		PCAN_USB_LIB
		NAMES Pcan_usb
		PATHS ${EXTERNAL_DIR}/pcan_win/lib/${ARCH}
		NO_DEFAULT_PATH
	)

	FIND_LIBRARY (
 		PCAN_2USB_LIB
		NAMES Pcan_2usb
		PATHS ${EXTERNAL_DIR}/pcan_win/lib/${ARCH}
		NO_DEFAULT_PATH
	)
	
	SET( PCAN_LIBRARY "${PCAN_USB_LIB}" )

ELSE( WIN32 )
	FIND_PATH(
		PCAN_INCLUDE_DIR
		NAMES libpcan.h
		PATHS ${EXTERNAL_DIR}/pcan/include
	)

	FIND_LIBRARY(
		PCAN_LIBRARY
		NAMES	pcan
		PATHS ${EXTERNAL_DIR}/pcan/lib/${ARCH}
		NO_DEFAULT_PATH
	)
ENDIF( WIN32 )

MACRO( COPY_PCAN_DLLS releaseTarget debugTarget )
IF( WIN32 )
  CONFIGURE_FILE( ${PCAN_DLL} ${releaseTarget} COPYONLY )
  CONFIGURE_FILE( ${PCAN_DLL} ${debugTarget} COPYONLY )
ENDIF( WIN32 )
ENDMACRO( COPY_PCAN_DLLS )

MACRO( COPY_PCAN2_DLLS releaseTarget debugTarget )
IF( WIN32 )
  CONFIGURE_FILE( ${PCAN2_DLL} ${releaseTarget} COPYONLY )
  CONFIGURE_FILE( ${PCAN2_DLL} ${debugTarget} COPYONLY )
ENDIF( WIN32 )
ENDMACRO( COPY_PCAN2_DLLS )

SET( PCAN_FOUND 0 )

IF( PCAN_INCLUDE_DIR AND PCAN_LIBRARY )
  SET( PCAN_FOUND 1 )
  SET( PCAN_VERSION_USED ${PCAN_VERSION} CACHE STRING DOCSTRING FORCE )
  INCLUDE_DIRECTORIES( ${PCAN_INCLUDE_DIR} )
ENDIF( PCAN_INCLUDE_DIR AND PCAN_LIBRARY )

IF( NOT PCAN_FOUND AND PCAN_FIND_REQUIRED )
  MESSAGE( FATAL_ERROR "PCAN not found" )
ENDIF( NOT PCAN_FOUND AND PCAN_FIND_REQUIRED )
