SET(
  NAME
  rec_robotinoarm_serial_test
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
  rec_robotinoarm
)

SOURCE_GROUP( "Source Files" FILES ${SRCS} ${HEADER} )

