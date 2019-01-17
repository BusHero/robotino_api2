SET(
  NAME
  rec_serialport
)

SET(
  EXPORTED_HEADER
  Port.h
  SerialPortException.h
  SerialPort.h 
)

IF( RTAI_FOUND )
  SET(
    EXPORTED_HEADER
    ${EXPORTED_HEADER}
    SerialPortRTAI.h
  ) 
ENDIF( RTAI_FOUND )

SET(
  SRCS
  SerialPort.cpp
)

SET(
  INTERNAL_HEADER
  SerialPortImpl.h
)

IF( WIN32 )
  SET(
    INTERNAL_HEADER
    ${INTERNAL_HEADER}
    SerialPortImpl_win.h
  )
  SET(
    SRCS
    ${SRCS}
    SerialPortImpl_win.cpp
  )
ELSE( WIN32 )
  SET(
    INTERNAL_HEADER
    ${INTERNAL_HEADER}
    SerialPortImpl_linux.h
  )
  SET(
    SRCS
    ${SRCS}
    SerialPortImpl_linux.cpp
  )
  IF( RTAI_FOUND )
  SET(
    INTERNAL_HEADER
    ${INTERNAL_HEADER}
    SerialPortImpl_rtai.h
  )
  SET(
    SRCS
    ${SRCS}
    SerialPortRTAI.cpp
    SerialPortImpl_rtai.cpp
  )
  ENDIF( RTAI_FOUND )
ENDIF( WIN32 )

ADD_LIBRARY(
  ${NAME}
  STATIC
  ${SRCS}
  ${HEADER}
  ${INTERNAL_HEADER}
  ${EXPORTED_HEADER}
)

#TARGET_LINK_LIBRARIES(
#	${NAME}
#)

IF( DO_COMMON_INSTALL )
	SET( REC_SERIALPORT_INCLUDE_INSTALL_DIR include/rec/serialport )
	
	INSTALL(
	  FILES
	  ${EXPORTED_HEADER}
	  DESTINATION ${REC_SERIALPORT_INCLUDE_INSTALL_DIR}
	)

	INSTALL(
		TARGETS ${NAME}
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib
		RUNTIME DESTINATION lib
	)
ENDIF( DO_COMMON_INSTALL )

IF(WIN32 AND NOT MINGW)
	SET_TARGET_PROPERTIES( ${NAME} PROPERTIES COMPILE_FLAGS "/MP" )
ENDIF(WIN32 AND NOT MINGW)
