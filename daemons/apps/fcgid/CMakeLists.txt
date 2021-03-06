SET(
  NAME
  fcgid
)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${SHARED_LIB_DIR}/rec/robotino/
)

FILE( GLOB SRCS *.cpp )
FILE( GLOB HEADER *.h )

MY_WRAP_CPP(
	MOC_SRCS
	${NAME}
	${HEADER}
)

#QT4_ADD_RESOURCES(
#	RCC_SRCS
#	web.qrc
#	images.qrc
#)

IF( WIN32 )
	SET( SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${NAME}_version.rc" )
ENDIF( WIN32 )

ADD_EXECUTABLE(
  ${NAME}
  ${SRCS}
  ${HEADER}
  ${MOC_SRCS}
  ${RCC_SRCS}
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
	${QT_QTNETWORK_LIBRARY}
	${QT_QTXML_LIBRARY}
	${QT_HELP_LIBRARIES}
	${REC_RPC_LIBRARY}
	${REC_ROBOTINO_DAEMONS_LIBRARY}
	${REC_ROBOTINO_RPC_LIBRARY}
	${REC_ROBOTINO_NAVIGATION_LIBRARY}
	${REC_ROBOTINO3_FLEETCOM_LIBRARY}
	${FCGI_LIB}
	${JANSSON_LIB}
	${REC_CV_LT_LIBRARY}
)

IF( UNIX )
	find_library( JPEG_LIB libjpeg.so )
	TARGET_LINK_LIBRARIES(
		${NAME}
		${JPEG_LIB}
	)
ENDIF()

INSTALL(
  TARGETS 
  ${NAME}
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  DESTINATION "${INSTALL_DIR}/var/www/fcgi-bin"
)

#INSTALL(
#	FILES
#	fcgid.sh
#	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
#	DESTINATION "${INSTALL_DIR}/etc/init.d"
#)

INSTALL(
  FILES 
  fcgid.conf
  DESTINATION "${INSTALL_DIR}/etc/robotino"
)

#FILE( GLOB SCRIPTS scripts/*.sh scripts/*.pl )

#INSTALL(
#  FILES
#  ${SCRIPTS}
#  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
#  DESTINATION "${INSTALL_DIR}/var/www/fcgi-bin/scripts"
#)

INSTALL(
  DIRECTORY
  "${CMAKE_CURRENT_SOURCE_DIR}/resources/"
  DESTINATION "${INSTALL_DIR}/var/www/fcgi-bin/resources"
  PATTERN ".svn" EXCLUDE
)

GENERATE_VERSION( ${NAME} "${CMAKE_CURRENT_BINARY_DIR}/${NAME}_version.h" "${ROBOTINO_DAEMONS_VERSION}" "FCGI Daemon" "${NAME}" )
GENERATE_VERSION_RC( ${NAME} "${CMAKE_CURRENT_BINARY_DIR}/${NAME}_version.rc"  "${ROBOTINO_DAEMONS_VERSION}" "FCGI Daemon" "${NAME}" "${NAME}.exe" 0 )

IF( WIN32 )
	IF( ARCHITECTURE STREQUAL "i386" )
		GENERATE_MANIFEST( ${NAME} "${CMAKE_CURRENT_BINARY_DIR}/manifest.xml" ${ROBOTINO_DAEMONS_VERSION} "${NAME}.exe" "X86" "win32" )
	ELSEIF( ARCHITECTURE STREQUAL "amd64" )
		GENERATE_MANIFEST( ${NAME} "${CMAKE_CURRENT_BINARY_DIR}/manifest.xml" ${ROBOTINO_DAEMONS_VERSION} "${NAME}.exe" "AMD64" "win32" )
	ENDIF( ARCHITECTURE STREQUAL "i386" )
ENDIF( WIN32 )

SOURCE_GROUP( "Source Files" FILES ${SRCS} ${HEADER} )
SOURCE_GROUP( Moc REGULAR_EXPRESSION "(moc_|qrc_)" )
