SET(
  NAME
  rec_dataexchange_client_console_test
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

