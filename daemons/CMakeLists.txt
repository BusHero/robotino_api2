cmake_minimum_required(VERSION 3.0)

PROJECT( ROBOTINO_DAEMONSS )

SET( ROBOTINO_DAEMONS_VERSION 1.2.6 CACHE STRING "Robotino Daemons Version" FORCE )
STRING( REGEX MATCHALL [0-9]+ VERSION_SPLIT ${ROBOTINO_DAEMONS_VERSION} )
LIST( GET VERSION_SPLIT 0 ROBOTINO_DAEMONS_MAJOR )
LIST( GET VERSION_SPLIT 1 ROBOTINO_DAEMONS_MINOR )
LIST( GET VERSION_SPLIT 2 ROBOTINO_DAEMONS_PATCH )

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	PROJECT( ROBOTINO_DAEMONS_x64 )
	SET( ARCHITECTURE amd64 )
ELSE( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	SET( ARCHITECTURE i386 )
ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )

SET( EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external )

IF( WIN32 )
	IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		STRING( REPLACE "\\" "/" RECRPC_DIR "$ENV{RECRPC64_DIR}" )
	ELSE( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		STRING( REPLACE "\\" "/" RECRPC_DIR "$ENV{RECRPC32_DIR}" )
	ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
ELSE( WIN32 )
	STRING( REPLACE "\\" "/" RECRPC_DIR "$ENV{RECRPC_DIR}" )
ENDIF( WIN32 )

IF( NOT RECRPC_DIR )
	IF( WIN32 )
		STRING( REPLACE "robotino_daemons" "rec_rpc" RECRPC_DIR "${CMAKE_CURRENT_BINARY_DIR}/install" )
	ELSE( WIN32 )
		SET( RECRPC_DIR "/usr/local/rec_rpc" )
	ENDIF( WIN32 )
ENDIF( NOT RECRPC_DIR )

SET(ROBOTINOCOMMON_DIR "C:\\Users\\petru\\Desktop\\install\\common")
IF( NOT ROBOTINOCOMMON_DIR )
	IF( WIN32 )
		STRING( REPLACE "robotino_daemons" "robotino_common" ROBOTINOCOMMON_DIR "${CMAKE_CURRENT_BINARY_DIR}/install" )
	ELSE( WIN32 )
		SET( ROBOTINOCOMMON_DIR "/usr/local/robotino/common" )
	ENDIF( WIN32 )
ENDIF( NOT ROBOTINOCOMMON_DIR )

SET(
	CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/cmake
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/common
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/external
	${RECRPC_DIR}
	${EXTERNAL_DIR}/cmake
)

IF( WIN32 )
	#MESSAGE( "SSE2 enabled on Windows" )
	SET( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /arch:SSE2" )
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2" )
ELSE( WIN32 )
	IF( NOT CMAKE_BUILD_TYPE )
		SET( CMAKE_BUILD_TYPE Release CACHE STRING
			"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
			FORCE )
		SET( CMAKE_CXX_FLAGS_RELEASE "-O2" CACHE STRING
			"cxx release flags."
			FORCE )
		SET( CMAKE_C_FLAGS_RELEASE "-O2" CACHE STRING
			"c release flags."
			FORCE )
	ENDIF( NOT CMAKE_BUILD_TYPE )

	#MESSAGE( "MMX enabled on Linux" )
	#SET( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -mmmx -Wno-long-long -std=c++11" )
	#SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmmx -Wno-long-long -std=c++11" )
	
	SET( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -mmmx -Wno-long-long" )
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmmx -Wno-long-long" )

	EXEC_PROGRAM( "cat /etc/issue" OUTPUT_VARIABLE BUILDDIST )
	STRING( SUBSTRING ${BUILDDIST} 0 11 BUILDDIST )
	IF ( ARCHITECTURE STREQUAL "i386" AND BUILDDIST STREQUAL "Ubuntu 9.04" )
		MESSAGE( "Detected Robotino Development System (Ubuntu 9.04 x86)!" )
		MESSAGE( "Providing Robotino specific optimization!" )
		OPTION( ROBOTINO_OPTIMIZATION "Robotino specific optimization?" ON)
	ENDIF ( ARCHITECTURE STREQUAL "i386" AND BUILDDIST STREQUAL "Ubuntu 9.04" )
	IF( ROBOTINO_OPTIMIZATION )
		MESSAGE( "Using Robotino specific optimization: optimizing for i486 and tuning for AMD Geode." )
		SET( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -march=i486 -mtune=geode -pipe -fno-align-functions -fno-align-loops -fno-align-jumps -fno-align-labels -finline-functions" )
		SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=i486 -mtune=geode -pipe -fno-align-functions -fno-align-loops -fno-align-jumps -fno-align-labels -finline-functions" )
	ELSE( ROBOTINO_OPTIMIZATION )
		IF( ARCHITECTURE STREQUAL "i386" )
			SET( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -march=i686" )
			SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=i686" )
		ENDIF( ARCHITECTURE STREQUAL "i386" )
		#MESSAGE( "SSE and SSE2 enabled on Linux" )
		SET( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -msse -msse2 -mfpmath=sse" )
		SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -mfpmath=sse" )
	ENDIF( ROBOTINO_OPTIMIZATION )
ENDIF( WIN32 )

INCLUDE( GenerateVersion )
INCLUDE( GenerateManifest )

SET(
  GLOBAL_TARGET_DIR
  ${ROBOTINO_DAEMONS_BINARY_DIR}/target
)
FILE( MAKE_DIRECTORY ${GLOBAL_TARGET_DIR} )
FILE( MAKE_DIRECTORY ${GLOBAL_TARGET_DIR}/debug )
FILE( MAKE_DIRECTORY ${GLOBAL_TARGET_DIR}/release )

INCLUDE( MyQt )

IF( WIN32 )
  SET(
    CMAKE_DEBUG_POSTFIX
    "d"
  )
ENDIF( WIN32 )

SET( INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/install" )

IF( WIN32 )
	STRING( REPLACE "\\" "/" ProgramFiles "$ENV{ProgramFiles}" )

	SET(
		CMAKE_INSTALL_PREFIX
		"${CMAKE_CURRENT_BINARY_DIR}/install/"
	)
	ADD_DEFINITIONS( 
		-D_CRT_SECURE_NO_DEPRECATE
		-DWIN32_LEAN_AND_MEAN
	)
	SET( CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE}   /GL /D _SECURE_SCL=0" )
	SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL /D _SECURE_SCL=0" )
	SET( CMAKE_EXE_LINKER_FLAGS_RELEASE    "${CMAKE_EXE_LINKER_FLAGS_RELEASE}    /LTCG" )
	SET( CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /LTCG" )
	SET( CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG" )
ELSE( WIN32 )
	SET(
		CMAKE_INSTALL_PREFIX
		"install/usr/local/robotino/daemons"
	)

	FILE( MAKE_DIRECTORY ${INSTALL_DIR} )
	FILE( MAKE_DIRECTORY ${INSTALL_DIR}/DEBIAN )
	
	INCLUDE( Codename )
	
	IF( QT5_FOUND )
		if( CODENAME STREQUAL "xenial" )
			ADD_DEFINITIONS(-DCODENAME_XENIAL)
			SET( DEPENDS "robotino-config (>=1.1.5), robotino-common (>= 1.1.13), rec-rpc, lighttpd, php-cgi, at, wput, jq" )
		else()
			ADD_DEFINITIONS(-DCODENAME_PRECISE)
			SET( DEPENDS "robotino-config, robotino-common (>= 1.1.0), rec-rpc, lighttpd, at" )
		endif()
	ELSE()
		ADD_DEFINITIONS(-DCODENAME_TRUSTY)
		SET( DEPENDS "robotino-config, robotino-common (>= 1.1.0), rec-rpc, libqtcore4, libqtgui4, libqt4-network, libqt4-xml, at" )
	ENDIF()

	IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )	
		CONFIGURE_FILE( linux_install/Robotino3/${CODENAME}/create_packages.sh.in ${CMAKE_CURRENT_BINARY_DIR}/create_packages.sh @ONLY )

		CONFIGURE_FILE( linux_install/Robotino3/${CODENAME}/postinst ${INSTALL_DIR}/DEBIAN/postinst COPYONLY )
		CONFIGURE_FILE( linux_install/Robotino3/${CODENAME}/preinst ${INSTALL_DIR}/DEBIAN/preinst COPYONLY )
		CONFIGURE_FILE( linux_install/Robotino3/${CODENAME}/postrm ${INSTALL_DIR}/DEBIAN/postrm COPYONLY )
		CONFIGURE_FILE( linux_install/Robotino3/${CODENAME}/prerm ${INSTALL_DIR}/DEBIAN/prerm COPYONLY )
		CONFIGURE_FILE( linux_install/Robotino3/${CODENAME}/control.in ${INSTALL_DIR}/DEBIAN/control @ONLY )
	ELSE()
		CONFIGURE_FILE( linux_install/Robotino2/create_packages.sh.in ${CMAKE_CURRENT_BINARY_DIR}/create_packages.sh @ONLY )

		CONFIGURE_FILE( linux_install/Robotino2/postinst ${INSTALL_DIR}/DEBIAN/postinst COPYONLY )
		CONFIGURE_FILE( linux_install/Robotino2/preinst ${INSTALL_DIR}/DEBIAN/preinst COPYONLY )
		CONFIGURE_FILE( linux_install/Robotino2/postrm ${INSTALL_DIR}/DEBIAN/postrm COPYONLY )
		CONFIGURE_FILE( linux_install/Robotino2/prerm ${INSTALL_DIR}/DEBIAN/prerm COPYONLY )
		CONFIGURE_FILE( linux_install/Robotino2/config ${INSTALL_DIR}/DEBIAN/config COPYONLY )
		CONFIGURE_FILE( linux_install/Robotino2/templates ${INSTALL_DIR}/DEBIAN/templates COPYONLY )
		CONFIGURE_FILE( linux_install/Robotino2/control.in ${INSTALL_DIR}/DEBIAN/control @ONLY )
	ENDIF()
	
	EXECUTE_PROCESS( COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/create_packages.sh TIMEOUT 100 )
ENDIF( WIN32 )
  
SET( BIN_INSTALL_DIR bin )
SET( LIB_INSTALL_DIR lib )
	
IF( WIN32 )
	find_package( ATL )
ENDIF( WIN32 )

SET(
  EXECUTABLE_OUTPUT_PATH
  ${GLOBAL_TARGET_DIR}
)
  
SET(
  LIBRARY_OUTPUT_PATH
  ${GLOBAL_TARGET_DIR}
)    

find_package( RTAI )
INCLUDE_DIRECTORIES( ${RTAI_INCLUDE_DIR} )
ADD_DEFINITIONS( -DUSE_RTAI )

#find_package( PCAN REQUIRED )
#INCLUDE_DIRECTORIES(
#	${PCAN_INCLUDE_DIR}
#)
#COPY_PCAN_DLLS( ${GLOBAL_TARGET_DIR}/release/ ${GLOBAL_TARGET_DIR}/debug/ )

ADD_DEFINITIONS( -DUNICODE -D_UNICODE )

find_package( RecRpc REQUIRED )
INCLUDE_DIRECTORIES(
	${REC_RPC_INCLUDES}
)
IF( WIN32 )
	COPY_REC_RPC_DLLS( ${GLOBAL_TARGET_DIR}/release/ ${GLOBAL_TARGET_DIR}/debug/ )
ENDIF( WIN32 )

find_package( RobotinoCommon REQUIRED )
INCLUDE_DIRECTORIES(
	${ROBOTINOCOMMON_INCLUDE_DIR}
)
COPY_ROBOTINOCOMMON_DLLS( ${GLOBAL_TARGET_DIR}/release/ ${GLOBAL_TARGET_DIR}/debug/ )

#find_package( libjansson )
find_package( libfcgi )

ADD_SUBDIRECTORY( apps )
ADD_SUBDIRECTORY( test )

INSTALL(
  FILES
  CHANGELOG.txt
  DESTINATION .
)

IF( WIN32 )
INSTALL(
	FILES
		${QT4_DEBUG_DLLS}
		${QT4_RELEASE_DLLS}
		${REC_RPC_DEBUG_DLL}
		${REC_RPC_RELEASE_DLL}
		${ROBOTINOCOMMON_DEBUG_DLLS}
		${ROBOTINOCOMMON_RELEASE_DLLS}
		${PCAN_DLL}
		DESTINATION bin
	)
ENDIF( WIN32 )

#FILE( WRITE ${CMAKE_CURRENT_BINARY_DIR}/Publish_documentation.cmake 
#	"EXECUTE_PROCESS(
#	COMMAND ${RECFTP} \"${CMAKE_INSTALL_PREFIX}/doc\" ftp://$ENV{DOC_ROBOTINO_DAEMONS_ORG_FTP_USER}:$ENV{DOC_ROBOTINO_DAEMONS_ORG_FTP_PASSWD}@doc.openrobotino.org:/documentation/robotino/daemons
#	TIMEOUT 100
#	)
#")
	
#ADD_CUSTOM_TARGET(
#  ALL_PUBLISH_DOCUMENTATION
#  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/Publish_documentation.cmake
#  COMMENT "publishing"
#)

#ADD_DEPENDENCIES(
#		ALL_PUBLISH_DOCUMENTATION
#		robotino_daemons_doxygen
#	)

IF( WIN32 )
	INSTALL(
		DIRECTORY
		"${ROBOTINOCOMMON_DRIVERS_DIR}"
		DESTINATION "${INSTALL_DIR}"
	)
ELSE( WIN32 )
	IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		INSTALL(
		  DIRECTORY
		  "${CMAKE_CURRENT_SOURCE_DIR}/scripts/Robotino3/common/"
		  DESTINATION "${INSTALL_DIR}"
		  PATTERN ".svn" EXCLUDE
		)
		if( CODENAME STREQUAL "xenial" )
			MESSAGE( "xenail install")
			INSTALL(
			  DIRECTORY
			  "${CMAKE_CURRENT_SOURCE_DIR}/scripts/Robotino3/xenial/"
			  DESTINATION "${INSTALL_DIR}"
			  PATTERN ".svn" EXCLUDE
			)
		else()
			MESSAGE( "precise install")
			INSTALL(
			  DIRECTORY
			  "${CMAKE_CURRENT_SOURCE_DIR}/scripts/Robotino3/precise/"
			  DESTINATION "${INSTALL_DIR}"
			  PATTERN ".svn" EXCLUDE
			)
		endif()
	ELSE( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		INSTALL(
		  DIRECTORY
		  "${CMAKE_CURRENT_SOURCE_DIR}/scripts/Robotino2/"
		  DESTINATION "${INSTALL_DIR}"
		  PATTERN ".svn" EXCLUDE
		)
	ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
ENDIF( WIN32 )

SET( ROBOTINO_WAR_DIR_ENV "$ENV{ROBOTINO_WAR_DIR}" CACHE STRING "ROBOTINO_WAR_DIR_ENV" FORCE )
IF(DEFINED ENV{ROBOTINO_WAR_DIR})
INSTALL(
  DIRECTORY
  "$ENV{ROBOTINO_WAR_DIR}/"
  DESTINATION "${INSTALL_DIR}/var/www"
  PATTERN ".svn" EXCLUDE
)
ENDIF(DEFINED ENV{ROBOTINO_WAR_DIR})

SET( CHANGELOG_NAME CHANGELOG_robotino_daemons.txt )
SET( CHANGELOG_FILE "${CMAKE_CURRENT_BINARY_DIR}/${CHANGELOG_NAME}" )
CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG.txt" "${CHANGELOG_FILE}" COPYONLY )

SET( PACKAGE_NAME robotino-daemons )
SET( PACKAGE_VERSION ${ROBOTINO_DAEMONS_VERSION} )
INCLUDE( CreateInstaller )
