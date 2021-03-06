IF( UNIX )
	IF( NOT PACKAGE_NAME )
	  MESSAGE( FATAL_ERROR "PACKAGE_NAME not set" )
	ENDIF()
	
	IF( NOT PACKAGE_VERSION )
	  MESSAGE( FATAL_ERROR "PACKAGE_VERSION not set" )
	ENDIF()
	
	FILE( WRITE ${CMAKE_CURRENT_BINARY_DIR}/CreateInstaller.cmake
"EXECUTE_PROCESS(
	COMMAND \"${CMAKE_CURRENT_BINARY_DIR}/create_packages.sh\"
)
")
	
	ADD_CUSTOM_TARGET(
		CREATE_INSTALLER
		COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/CreateInstaller.cmake
		COMMENT "create installer"
	)
	
	EXECUTE_PROCESS(
	    COMMAND lsb_release -a
	    OUTPUT_VARIABLE CODENAMELISTSTR
		ERROR_QUIET
	    TIMEOUT 10
	)
	#MESSAGE( "CODENAMELISTSTR=${CODENAMELISTSTR}" )
	STRING(REPLACE ":" " " CODENAMELISTSTR ${CODENAMELISTSTR})
	string(REGEX MATCHALL "[a-zA-Z]+" CODENAMELIST "${CODENAMELISTSTR}")
	#MESSAGE( "CODENAMELIST=${CODENAMELIST}" )
	LIST( REVERSE CODENAMELIST )
	LIST(GET CODENAMELIST 0 CODENAME )
	MESSAGE( "CODENAME=${CODENAME}" )
	IF( NOT CODENAME STREQUAL "precise"
	    AND NOT CODENAME STREQUAL "trusty"
	    AND NOT CODENAME STREQUAL "xenial" )
		MESSAGE( FATAL_ERROR "Unsupported CODENAME ${CODENAME}" )
	ENDIF()
	
	SET( DEBIAN_IN_DIR "$ENV{HOME}/debian-in/" )
	SET( DEBIAN_OUT_DIR "$ENV{HOME}/debian/" )
	
	file( MAKE_DIRECTORY "${DEBIAN_IN_DIR}" )
	
	file( MAKE_DIRECTORY "${DEBIAN_OUT_DIR}" )
	file( MAKE_DIRECTORY "${DEBIAN_OUT_DIR}/conf" )
	FILE( WRITE "${DEBIAN_OUT_DIR}/conf/distributions"
"Codename: ${CODENAME}
Components: main
Architectures: i386 amd64
SignWith: 8097CA6D
")
	
	FILE( WRITE ${CMAKE_CURRENT_BINARY_DIR}/PublishInstaller.cmake 
"
FILE( GLOB INSTALLER_NAME RELATIVE \"\${CMAKE_CURRENT_BINARY_DIR}\" \"\${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${PACKAGE_VERSION}_*.deb\" )

EXECUTE_PROCESS( 
	COMMAND rsync -hv \${INSTALLER_NAME} root@svn.rec.de:/tmp
	TIMEOUT 10000
)

EXECUTE_PROCESS( 
	COMMAND ssh root@svn.rec.de \"./includedeb.sh ${CODENAME} /tmp/\${INSTALLER_NAME}\"
	TIMEOUT 10000
)
")
	
	ADD_CUSTOM_TARGET(
		PUBLISH_INSTALLER
		COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/PublishInstaller.cmake
		COMMENT "publishing"
	)
	
ENDIF( UNIX )