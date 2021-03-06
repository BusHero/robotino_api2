SET(
  NAME
  rec_robotino_imagesender
)

FILE( GLOB SRCS *.cpp )
FILE( GLOB HEADER *.h )

MY_WRAP_CPP(
	MOC_SRCS
	"${rec_robotino_imagesender}"
	${HEADER}
)

ADD_LIBRARY(
	${NAME}
	STATIC
	${SRCS}
	${HEADER}
	${MOC_SRCS}
)

IF(WIN32 AND NOT MINGW)
	SET_TARGET_PROPERTIES( ${NAME} PROPERTIES COMPILE_FLAGS "/MP" )
ENDIF(WIN32 AND NOT MINGW)

TARGET_LINK_LIBRARIES(
	${NAME}
	${QT_QTCORE_LIBRARY}
	${QT_QTNETWORK_LIBRARY}
	${QT_HELP_LIBRARIES}
)

INSTALL(
  TARGETS ${NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib
)

IF( DO_COMMON_INSTALL )
	SET( REC_ROBOTINO_IMAGESENDER_INCLUDE_INSTALL_DIR include/rec/robotino/imagesender )

	INSTALL(
	  FILES
	  ${HEADER}
	  DESTINATION ${REC_ROBOTINO_IMAGESENDER_INCLUDE_INSTALL_DIR}
	)

ENDIF( DO_COMMON_INSTALL )

SOURCE_GROUP( Moc REGULAR_EXPRESSION "(moc_|qrc_)" )
SOURCE_GROUP( "Source Files" FILES ${SRCS} ${HEADER} )