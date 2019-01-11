SET(
  NAME
  iocom
)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

FILE( GLOB SRCS *.cpp )
FILE( GLOB HEADER *.h )

MY_WRAP_CPP(
	MOC_SRCS
	${NAME}
	${HEADER}
)

IF( WIN32 )
	SET( SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${NAME}_version.rc" )
ENDIF( WIN32 )

ADD_EXECUTABLE(
  ${NAME}
  WIN32
  ${SRCS}
  ${HEADER}
  ${MOC_SRCS}
  ${MOC_RPC_SRCS}
  ${CONFIG_FILES}
)

IF(WIN32)
SET_TARGET_PROPERTIES(
  ${NAME}
  PROPERTIES
  COMPILE_FLAGS "/wd4251 /MP"
)
ENDIF( WIN32 )

TARGET_LINK_LIBRARIES(
	${NAME}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTMAIN_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_QTXML_LIBRARY}
	${QT_HELP_LIBRARIES}
	rec_robotino3_serialio
	rec_robotino3_iocom
)

IF( WIN32 )
	TARGET_LINK_LIBRARIES(
		${NAME}
		setupapi
	)
ENDIF( WIN32 )

INSTALL(
  TARGETS ${NAME}
  RUNTIME DESTINATION bin
)

if( WIN32 )
	#x86 drivers
	INSTALL(
	  DIRECTORY
	  driver/x86
	  DESTINATION drivers/robotino3-usb
	)
	INSTALL(
	  FILES
	  driver/DPInst.xml
	  DESTINATION drivers/robotino3-usb/x86
	)
	INSTALL(
	  FILES
	  driver/usbser/REC_Robotino_3_USB.inf
	  DESTINATION drivers/robotino3-usb/x86/usbser
	)
	INSTALL(
	  DIRECTORY
	  driver/usbser/x86/
	  DESTINATION drivers/robotino3-usb/x86/usbser
	)
	INSTALL(
	  FILES
	  driver/ftd2xx/ftdibus.inf
	  DESTINATION drivers/robotino3-usb/x86/ftd2xx
	)
	INSTALL(
	  FILES
	  driver/ftd2xx/ftdibus.cat
	  DESTINATION drivers/robotino3-usb/x86/ftd2xx
	)
	INSTALL(
	  FILES
	  driver/ftd2xx/ftdiport.inf
	  DESTINATION drivers/robotino3-usb/x86/ftd2xx
	)
	INSTALL(
	  FILES
	  driver/ftd2xx/ftdiport.cat
	  DESTINATION drivers/robotino3-usb/x86/ftd2xx
	)
	INSTALL(
	  DIRECTORY
	  driver/ftd2xx/amd64
	  DESTINATION drivers/robotino3-usb/x86/ftd2xx
	)
	INSTALL(
	  DIRECTORY
	  driver/ftd2xx/i386
	  DESTINATION drivers/robotino3-usb/x86/ftd2xx
	)

	#x64 drivers
	INSTALL(
	  DIRECTORY
	  driver/x64
	  DESTINATION drivers/robotino3-usb
	)
	INSTALL(
	  FILES
	  driver/DPInst.xml
	  DESTINATION drivers/robotino3-usb/x64
	)
	INSTALL(
	  FILES
	  driver/usbser/REC_Robotino_3_USB.inf
	  DESTINATION drivers/robotino3-usb/x64/usbser
	)
	INSTALL(
	  DIRECTORY
	  driver/usbser/x64/
	  DESTINATION drivers/robotino3-usb/x64/usbser
	)
	INSTALL(
	  FILES
	  driver/ftd2xx/ftdibus.inf
	  DESTINATION drivers/robotino3-usb/x64/ftd2xx
	)
	INSTALL(
	  FILES
	  driver/ftd2xx/ftdibus.cat
	  DESTINATION drivers/robotino3-usb/x64/ftd2xx
	)
	INSTALL(
	  FILES
	  driver/ftd2xx/ftdiport.inf
	  DESTINATION drivers/robotino3-usb/x64/ftd2xx
	)
	INSTALL(
	  FILES
	  driver/ftd2xx/ftdiport.cat
	  DESTINATION drivers/robotino3-usb/x64/ftd2xx
	)
	INSTALL(
	  DIRECTORY
	  driver/ftd2xx/amd64
	  DESTINATION drivers/robotino3-usb/x64/ftd2xx
	)
	INSTALL(
	  DIRECTORY
	  driver/ftd2xx/i386
	  DESTINATION drivers/robotino3-usb/x64/ftd2xx
	)

else( WIN32 )
	install(CODE "MESSAGE(\"Installing symlink /usr/bin/iocom.\")")
	INSTALL(CODE "
    EXECUTE_PROCESS( COMMAND mkdir -p ${INSTALL_DIR}/usr/bin )
    ")
    INSTALL(CODE "
    EXECUTE_PROCESS( COMMAND ln -sf /usr/local/robotino/common/bin/iocom ${INSTALL_DIR}/usr/bin )
    ")
endif( WIN32 )

GENERATE_VERSION( ${NAME} "${CMAKE_CURRENT_BINARY_DIR}/${NAME}_version.h" "${ROBOTINO_COMMON_VERSION}" "${NAME}" "${NAME}" )
GENERATE_VERSION_RC( ${NAME} "${CMAKE_CURRENT_BINARY_DIR}/${NAME}_version.rc"  "${ROBOTINO_COMMON_VERSION}" "${NAME}" "${NAME}" "${NAME}.exe" 0 )

IF( WIN32 )
	IF( ARCHITECTURE STREQUAL "i386" )
		GENERATE_MANIFEST( ${NAME} "${CMAKE_CURRENT_BINARY_DIR}/manifest.xml" ${ROBOTINO_COMMON_VERSION} "${NAME}.exe" "X86" "win32" )
	ELSEIF( ARCHITECTURE STREQUAL "amd64" )
		GENERATE_MANIFEST( ${NAME} "${CMAKE_CURRENT_BINARY_DIR}/manifest.xml" ${ROBOTINO_COMMON_VERSION} "${NAME}.exe" "AMD64" "win32" )
	ENDIF( ARCHITECTURE STREQUAL "i386" )
ENDIF( WIN32 )

SOURCE_GROUP( "Source Files" FILES ${SRCS} ${HEADER} )
SOURCE_GROUP( "Config" FILES ${CONFIG_FILES} )
SOURCE_GROUP( Moc REGULAR_EXPRESSION "(moc_|qrc_)" )
