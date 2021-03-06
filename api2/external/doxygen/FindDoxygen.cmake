#  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

SET( DOXYGEN_PATH "${EXTERNAL_DIR}/doxygen" )

IF( WIN32 )
	SET( DOXYGEN_BIN_PATH "${EXTERNAL_DIR}/doxygen/bin/win" )
ELSE( WIN32 )
	SET( DOXYGEN_BIN_PATH "/usr/bin" )
ENDIF( WIN32 )

FIND_PROGRAM(
  DOXYGEN_EXEC
  NAMES doxygen
  PATHS
  ${DOXYGEN_BIN_PATH}
)

FIND_FILE(
	DOXYGEN_CFG
	NAMES doxygen.cfg.in
	PATHS
	${DOXYGEN_PATH}
  NO_DEFAULT_PATH
)

# create doxygen generated documentation
# TARGET_NAME is the name of the current build target
# SOURCEFILE is the list a source files
# OUTPUT_DIRECTORY is the directory where the documentation is generated
# PROJECT_NAME the name of the project
MACRO( GENERATE_DOCUMENTATION PARENT TAREGT_NAME SOURCEFILES OUTPUT_DIRECTORY PROJECT_NAME )
	
	SET(
		DOXYGEN_OUTPUT_DIRECTORY
		${OUTPUT_DIRECTORY}
	)
	
	SET(
		DOXYGEN_PROJECT_NAME
		${PROJECT_NAME}
	)
	
	SET( INPUT "" )
	FOREACH( var ${SOURCEFILES} )
		FIND_FILE( FULLPATH "${var}" PATHS "${CMAKE_CURRENT_SOURCE_DIR}" NO_DEFAULT_PATH )
		IF( NOT FULLPATH )
			SET( FULLPATH "${var}" )
		ENDIF( NOT FULLPATH )
		SET( INPUT "${INPUT} \"${FULLPATH}\"" )
		SET( FULLPATH <FULLPATH>-NOTFOUND )
	ENDFOREACH( var ${SOURCEFILES} )
	
	CONFIGURE_FILE( "${DOXYGEN_CFG}" "${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg" @ONLY )
	
  FILE( MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY} )
	
	FILE( WRITE ${CMAKE_CURRENT_BINARY_DIR}/ExecDoxygen.cmake 
		"EXECUTE_PROCESS(
		COMMAND \"${DOXYGEN_EXEC}\" \"${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg\"
		TIMEOUT 10
	)
	")
	
	ADD_CUSTOM_TARGET(
    ${TAREGT_NAME}
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/ExecDoxygen.cmake
    COMMENT "creating doxygen documentation"
  )
  
  ADD_DEPENDENCIES(
		${PARENT}
		${TAREGT_NAME}
  )
  
ENDMACRO( GENERATE_DOCUMENTATION )

IF( DOXYGEN_EXEC )
  SET( DOXYGEN_FOUND "YES" )
  SET( DOXYGEN_VERSION_USED ${DOXYGEN_VERSION} CACHE STRING DOCSTRING FORCE )
ENDIF( DOXYGEN_EXEC )
