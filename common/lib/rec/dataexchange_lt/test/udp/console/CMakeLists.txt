SET(
	NAME
	rec_dataexchange_udp_console_test
)

FILE( GLOB SRCS *.cpp )
FILE( GLOB HEADER *.h )

ADD_EXECUTABLE(
	${NAME}
	${SRCS}
	${HEADER}
)

TARGET_LINK_LIBRARIES(
	${NAME}
	rec_dataexchange
)

SOURCE_GROUP( "Source Files" FILES ${SRCS} ${HEADER} )

IF( REC_DATAEXCHANGE_INSTALL_HEADER_ENABLED )
	INSTALL(
		FILES
		${SRCS}
		${HEADER}
		DESTINATION api/examples/rec/dataexchange/udp
	)
	IF( WIN32 )
		INSTALL(
			FILES
			${CMAKE_CURRENT_SOURCE_DIR}/udp.vcproj
			${CMAKE_CURRENT_SOURCE_DIR}/udp.settings
			"${CMAKE_CURRENT_SOURCE_DIR}/RUN THIS FIRST THEN START VS.cmd"
			DESTINATION api/examples/rec/dataexchange/udp
		)
	ELSE( WIN32 )
		INSTALL(
			FILES
			${CMAKE_CURRENT_SOURCE_DIR}/Makefile
			DESTINATION api/examples/rec/dataexchange/udp
		)
	ENDIF( WIN32 )
ENDIF( REC_DATAEXCHANGE_INSTALL_HEADER_ENABLED )
