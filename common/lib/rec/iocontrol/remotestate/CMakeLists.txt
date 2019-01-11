SET(
  NAME
  rec_iocontrol_remotestate
)

FILE( GLOB HEADER *.h )

INSTALL(
  FILES
  ${HEADER}
  DESTINATION include/rec/iocontrol/remotestate
)
