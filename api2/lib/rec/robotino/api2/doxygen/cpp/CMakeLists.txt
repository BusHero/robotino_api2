IF( DOXYGEN_FOUND )
	FILE( GLOB HEADER ${CMAKE_CURRENT_SOURCE_DIR}/../../*.h )
	
	SET( OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/rec_robotino_api2" )
	#GENERATE_DOCUMENTATION( rec_robotino_api2 "rec_robotino_api2_doxygen" "${HEADER}" "${OUTPUT_DIRECTORY}" "rec::robotino::api2 C++ interface" )
  
  INSTALL(
		DIRECTORY
		"${OUTPUT_DIRECTORY}"
		DESTINATION doc
  )
ENDIF( DOXYGEN_FOUND )