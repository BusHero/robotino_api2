cmake_minimum_required(VERSION 3.0)

PROJECT( ROBOTINO_API2 )

SET( ROBOTINO_API2_VERSION 1.1.7 CACHE STRING "Robotino API2 Version" FORCE )
STRING( REGEX MATCHALL [0-9]+ VERSION_SPLIT ${ROBOTINO_API2_VERSION} )
LIST( GET VERSION_SPLIT 0 ROBOTINO_API2_MAJOR )
LIST( GET VERSION_SPLIT 1 ROBOTINO_API2_MINOR )
LIST( GET VERSION_SPLIT 2 ROBOTINO_API2_PATCH )

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	PROJECT( ROBOTINO_API2_x64 )
	SET( ARCHITECTURE amd64 )
ELSE( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	SET( ARCHITECTURE i386 )
ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )

OPTION(USE_SYSTEM_QT "Link dynamically with Qt found on your system" ON)

SET( ROBOTINO_API2_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
SET( ROBOTINO_API2_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" )

SET( EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external" )

IF(APPLE)
	#set (IOS True)
	SET( QT5ROOT "$ENV{HOME}/Qt/5.5" )
	
	OPTION( IOS "Build for iOS?" ON )
	SET(
		CMAKE_MODULE_PATH
		${CMAKE_CURRENT_SOURCE_DIR}/cmake/common
		${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/external
		${EXTERNAL_DIR}/doxygen
	)
ELSE (APPLE)
	SET(
	CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/common
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/external
	${EXTERNAL_DIR}/qt_4_7_4_static_robotino_api2_qt/cmake
	${EXTERNAL_DIR}/doxygen
)
ENDIF (APPLE)

IF( NOT WIN32 )
	SET(
		CMAKE_MODULE_PATH
		${CMAKE_MODULE_PATH}
		$ENV{REC_RPC_DIR}
		$ENV{ROBOTINOCOMMON_DIR}
	)
ENDIF( NOT WIN32 )

FIND_FILE(
	DOXYGEN_CFG
	NAMES doxygen.cfg.in
	PATHS
	"${EXTERNAL_DIR}/doxygen"
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
		COMMAND \"${DOXYGEN_EXECUTABLE}\" \"${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg\"
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


IF( WIN32 )
	#MESSAGE( "SSE2 enabled on Windows" )
	SET( CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /D NDEBUG /Zi /Oy- /DEBUG" CACHE STRING
		"Flags used by the compiler during Release builds."
		FORCE )
	SET( CMAKE_C_FLAGS_RELEASE "/MD /O2 /Ob2 /D NDEBUG /Zi /Oy- /DEBUG " CACHE STRING
		"Flags used by the compiler during Release builds."
		FORCE )
	SET( CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MD /O2 /Ob2 /D NDEBUG /Zi /Oy- /DEBUG " CACHE STRING
		"Flags used by the compiler during Release builds."
		FORCE )
	SET( CMAKE_EXE_LINKER_FLAGS_RELEASE "/debug /INCREMENTAL" CACHE STRING
		"Flags used by the linker during Release builds."
		FORCE )
	SET( CMAKE_MODULE_LINKER_FLAGS_RELEASE "/debug /INCREMENTAL" CACHE STRING
		"Flags used by the linker during Release builds."
		FORCE )
	SET( CMAKE_SHARED_LINKER_FLAGS_RELEASE "/debug /INCREMENTAL" CACHE STRING
		"Flags used by the linker during Release builds."
		FORCE )
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

	MESSAGE( "MMX enabled on Linux" )
	SET( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -mmmx -fPIC" )
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmmx -fPIC" )

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
		MESSAGE( "SSE and SSE2 enabled on Linux" )
		IF(APPLE)
			SET( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -msse -msse2" )
			SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2" )
		ELSE (APPLE)
			SET( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -msse -msse2 -mfpmath=sse" )
			SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -mfpmath=sse" )
		ENDIF (APPLE)
	ENDIF( ROBOTINO_OPTIMIZATION )
ENDIF( WIN32 )

INCLUDE( GenerateVersion )

IF( APPLE )
	SET(
	  GLOBAL_TARGET_DIR
	  ${ROBOTINO_API2_BINARY_DIR}
	)
ELSE()
	SET(
	  GLOBAL_TARGET_DIR
	  ${ROBOTINO_API2_BINARY_DIR}/target
	)
	FILE( MAKE_DIRECTORY ${GLOBAL_TARGET_DIR} )
	FILE( MAKE_DIRECTORY ${GLOBAL_TARGET_DIR}/debug )
	FILE( MAKE_DIRECTORY ${GLOBAL_TARGET_DIR}/release )
ENDIF()

IF (APPLE)
	IF (IOS)
		# Setup iOS platform
		if (NOT DEFINED IOS_PLATFORM)
			set (IOS_PLATFORM "OS")
		endif (NOT DEFINED IOS_PLATFORM)
		set (IOS_PLATFORM ${IOS_PLATFORM} CACHE STRING "Type of iOS Platform")

		# Check the platform selection and setup for developer root
		if (${IOS_PLATFORM} STREQUAL "OS")
			set (IOS_PLATFORM_LOCATION "iPhoneOS.platform")
		elseif (${IOS_PLATFORM} STREQUAL "SIMULATOR")
			set (IOS_PLATFORM_LOCATION "iPhoneSimulator.platform")
		else (${IOS_PLATFORM} STREQUAL "OS")
			message (FATAL_ERROR "Unsupported IOS_PLATFORM value selected. Please choose OS or SIMULATOR")
		endif (${IOS_PLATFORM} STREQUAL "OS")

		# Setup iOS developer location
		if (NOT DEFINED CMAKE_IOS_DEVELOPER_ROOT)
			set (CMAKE_IOS_DEVELOPER_ROOT "/Applications/Xcode.app/Contents/Developer/Platforms/${IOS_PLATFORM_LOCATION}/Developer")
		endif (NOT DEFINED CMAKE_IOS_DEVELOPER_ROOT)
		set (CMAKE_IOS_DEVELOPER_ROOT ${CMAKE_IOS_DEVELOPER_ROOT} CACHE PATH "Location of iOS Platform")

		# Find and use the most recent iOS sdk 
		if (NOT DEFINED CMAKE_IOS_SDK_ROOT)
			file (GLOB _CMAKE_IOS_SDKS "${CMAKE_IOS_DEVELOPER_ROOT}/SDKs/*")
			if (_CMAKE_IOS_SDKS) 
				list (SORT _CMAKE_IOS_SDKS)
				#list (REVERSE _CMAKE_IOS_SDKS)
				list (GET _CMAKE_IOS_SDKS 0 CMAKE_IOS_SDK_ROOT)
			else (_CMAKE_IOS_SDKS)
				message (FATAL_ERROR "No iOS SDK's found in default seach path ${CMAKE_IOS_DEVELOPER_ROOT}. Manually set CMAKE_IOS_SDK_ROOT or install the iOS SDK.")
			endif (_CMAKE_IOS_SDKS)
			message (STATUS "Toolchain using default iOS SDK: ${CMAKE_IOS_SDK_ROOT}")
		endif (NOT DEFINED CMAKE_IOS_SDK_ROOT)


		set (CMAKE_IOS_SDK_ROOT ${CMAKE_IOS_SDK_ROOT} CACHE PATH "Location of the selected iOS SDK")

		# Set the sysroot default to the most recent SDK
		# set (CMAKE_OSX_SYSROOT ${CMAKE_IOS_SDK_ROOT} CACHE PATH "Sysroot used for iOS support")
		# set(CMAKE_OSX_SYSROOT ${CMAKE_IOS_SDK_ROOT})
		set(CMAKE_OSX_SYSROOT "iphoneos")

		set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD) armv7s")
		SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

		# Set the find root to the iOS developer roots and to user defined paths
		set (CMAKE_FIND_ROOT_PATH ${CMAKE_IOS_DEVELOPER_ROOT} ${CMAKE_IOS_SDK_ROOT} ${CMAKE_PREFIX_PATH} CACHE string  "iOS find search path root")

		# default to searching for frameworks first
		set (CMAKE_FIND_FRAMEWORK FIRST)

		# set up the default search directories for frameworks
		set (CMAKE_SYSTEM_FRAMEWORK_PATH
			${CMAKE_IOS_SDK_ROOT}/System/Library/Frameworks
			${CMAKE_IOS_SDK_ROOT}/System/Library/PrivateFrameworks
			${CMAKE_IOS_SDK_ROOT}/Developer/Library/Frameworks
		)

		SET( Qt5Core_DIR "${QT5ROOT}/ios/lib/cmake/Qt5Core/" )
		SET( Qt5Xml_DIR "${QT5ROOT}/ios/lib/cmake/Qt5Xml/" )
		SET( Qt5Network_DIR "${QT5ROOT}/ios/lib/cmake/Qt5Network/" )
		SET( Qt5Svg_DIR "${QT5ROOT}/ios/lib/cmake/Qt5Svg/" )
		SET( Qt5Gui_DIR "${QT5ROOT}/ios/lib/cmake/Qt5Gui/" )
	ELSE(IOS)
		SET( Qt5Core_DIR "${QT5ROOT}/clang_64/lib/cmake/Qt5Core/" )
		SET( Qt5Xml_DIR "${QT5ROOT}/clang_64/lib/cmake/Qt5Xml/" )
		SET( Qt5Network_DIR "${QT5ROOT}/clang_64/lib/cmake/Qt5Network/" )
		SET( Qt5Svg_DIR "${QT5ROOT}/clang_64/lib/cmake/Qt5Svg/" )
		SET( Qt5Gui_DIR "${QT5ROOT}/clang_64/lib/cmake/Qt5Gui/" )
	ENDIF (IOS)
	
	find_package(Qt5Core REQUIRED)
	find_package(Qt5Xml REQUIRED)
	find_package(Qt5Network REQUIRED)
	find_package(Qt5Svg REQUIRED)
	find_package(Qt5Gui REQUIRED)

	MACRO (MY_WRAP_CPP outfiles prefix)
		# get include dirs
		#QT4_STATIC_GET_MOC_FLAGS(moc_includes)

		set(moc_files ${_WRAP_CPP_UNPARSED_ARGUMENTS})
	    set(moc_options ${_WRAP_CPP_OPTIONS})
	    set(moc_target ${_WRAP_CPP_TARGET})

		FOREACH (it ${ARGN})
			GET_FILENAME_COMPONENT(it ${it} ABSOLUTE)
			GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)

			SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/moc_${prefix}_${outfile}.cxx)
			 ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
			 COMMAND ${Qt5Core_MOC_EXECUTABLE}
			 ARGS -nw ${moc_includes} -o ${outfile} ${it}
			 DEPENDS ${it})
			# qt5_create_moc_command(${it} ${outfile} "${moc_flags}" "${moc_options}" "${moc_target}")
			SET(${outfiles} ${${outfiles}} ${outfile})
		ENDFOREACH(it)
	ENDMACRO (MY_WRAP_CPP)

ELSE (APPLE)
	IF(USE_SYSTEM_QT)
		SET( QT_USE_QTGUI 0 )
		SET( QT_USE_QTNETWORK 1 )
		SET( QT_USE_QTXML 1 )
		INCLUDE( MyQt )
		
		IF( NOT RECRPC_DIR )
			IF( WIN32 )
				SET(RECRPC_DIR "C:\\robotino\\rec_rpc")
				#STRING( REPLACE "robotino_api2" "rec_rpc" RECRPC_DIR "${CMAKE_CURRENT_BINARY_DIR}/install" )
			ELSE( WIN32 )
				SET( RECRPC_DIR "/usr/local/rec_rpc" )
			ENDIF( WIN32 )
		ENDIF( NOT RECRPC_DIR )

		IF( NOT ROBOTINOCOMMON_DIR )
			IF( WIN32 )
				SET(ROBOTINOCOMMON_DIR "C:\\robotino\\common")
				#STRING( REPLACE "robotino_api2" "robotino_common" ROBOTINOCOMMON_DIR "${CMAKE_CURRENT_BINARY_DIR}/install" )
			ELSE( WIN32 )
				SET( ROBOTINOCOMMON_DIR "/usr/local/robotino/common" )
			ENDIF( WIN32 )
		ENDIF( NOT ROBOTINOCOMMON_DIR )
		
		SET(
			CMAKE_MODULE_PATH
			${RECRPC_DIR}
			${CMAKE_CURRENT_SOURCE_DIR}/cmake/common
		)
		
		find_package( RecRpc REQUIRED )
		INCLUDE_DIRECTORIES( ${REC_RPC_INCLUDES})
		
		find_package( RobotinoCommon REQUIRED )
		INCLUDE_DIRECTORIES( ${ROBOTINOCOMMON_INCLUDE_DIR} )
	ELSE()
		find_package( MyQt4Static )
		find_package( MyQt4StaticHelpers REQUIRED )
		ADD_DEFINITIONS( -DQT_VERSION_4 )
		
		SET( REC_RPC_STATIC 1 )
		ADD_DEFINITIONS( -DREC_RPC_STATIC )
		SET( REC_RPC_LIBRARY rec_rpc )

		SET( REC_ROBOTINO_RPC_STATIC 1 )
		ADD_DEFINITIONS( -DREC_ROBOTINO_RPC_STATIC )
		SET( REC_ROBOTINO_RPC_LIBRARY rec_robotino_rpc )
	ENDIF()
ENDIF (APPLE)



SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG QT_DEBUG)
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE QT_NO_DEBUG)
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE QT_NO_DEBUG_OUTPUT )
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE QT_NO_WARNING_OUTPUT )
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELWITHDEBINFO QT_NO_DEBUG)
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_MINSIZEREL QT_NO_DEBUG)
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_MINSIZEREL QT_NO_DEBUG_OUTPUT )
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_MINSIZEREL QT_NO_WARNING_OUTPUT )


IF (APPLE)
	INCLUDE_DIRECTORIES( ${Qt5Core_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Svg_INCLUDE_DIRS} )
	ADD_DEFINITIONS(${Qt5Core_DEFINITIONS})
	ADD_DEFINITIONS(${Qt5Xml_DEFINITIONS})
	ADD_DEFINITIONS(${Qt5Network_DEFINITIONS})
	ADD_DEFINITIONS(${Qt5Gui_DEFINITIONS})
	ADD_DEFINITIONS(${Qt5Svg_DEFINITIONS})
ELSE (APPLE)
	INCLUDE_DIRECTORIES( ${QT_STATIC_INCLUDE_DIR} ${QT_STATIC_QTCORE_INCLUDE_DIR} ${QT_STATIC_QTXML_INCLUDE_DIR} ${QT_STATIC_QTNETWORK_INCLUDE_DIR} )
	ADD_DEFINITIONS(${QT_STATIC_DEFINITIONS})
ENDIF (APPLE)


find_package( Doxygen REQUIRED )

IF( WIN32 )
  SET(
    CMAKE_DEBUG_POSTFIX
    "d"
  )
ENDIF( WIN32 )

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
		INSTALL_DIR
		"${CMAKE_CURRENT_BINARY_DIR}/install"
	)
	SET(
		CMAKE_INSTALL_PREFIX
		"install/usr/local/robotino/api2"
	)

	FILE( MAKE_DIRECTORY ${INSTALL_DIR} )
	
	IF(APPLE)
		CONFIGURE_FILE( mac_install/create_packages.sh.in ${CMAKE_CURRENT_BINARY_DIR}/create_packages.sh @ONLY )
		EXECUTE_PROCESS( COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/create_packages.sh TIMEOUT 100 )
	ELSE(APPLE)
		FILE( MAKE_DIRECTORY ${INSTALL_DIR}/DEBIAN )
	
		CONFIGURE_FILE( linux_install/robotino_api2.conf ${CMAKE_CURRENT_BINARY_DIR}/robotino_api2.conf COPYONLY )
		CONFIGURE_FILE( linux_install/create_packages.sh.in ${CMAKE_CURRENT_BINARY_DIR}/create_packages.sh @ONLY )
		EXECUTE_PROCESS( COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/create_packages.sh TIMEOUT 100 )
		
		CONFIGURE_FILE( linux_install/postinst ${CMAKE_CURRENT_BINARY_DIR}/install/DEBIAN/postinst COPYONLY )
		CONFIGURE_FILE( linux_install/preinst ${CMAKE_CURRENT_BINARY_DIR}/install/DEBIAN/preinst COPYONLY )
		CONFIGURE_FILE( linux_install/postrm ${CMAKE_CURRENT_BINARY_DIR}/install/DEBIAN/postrm COPYONLY )
		CONFIGURE_FILE( linux_install/prerm ${CMAKE_CURRENT_BINARY_DIR}/install/DEBIAN/prerm COPYONLY )
		CONFIGURE_FILE( linux_install/control.in ${CMAKE_CURRENT_BINARY_DIR}/install/DEBIAN/control @ONLY )
	ENDIF()
ENDIF( WIN32 )
  
SET( BIN_INSTALL_DIR bin )
SET( LIB_INSTALL_DIR lib )
	
SET(
  LIBRARY_OUTPUT_PATH
  ${GLOBAL_TARGET_DIR}
)
    
SET(
  EXECUTABLE_OUTPUT_PATH
  ${GLOBAL_TARGET_DIR}
)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

ADD_DEFINITIONS( -DUNICODE -D_UNICODE )

IF (APPLE)
	SET( REC_ROBOTINO_API2_STATIC 1 )
	ADD_DEFINITIONS( -DREC_ROBOTINO_API2_STATIC )
ENDIF (APPLE)

ADD_SUBDIRECTORY( lib )

#IF(NOT IOS)
#ADD_SUBDIRECTORY( test )
#ENDIF()

INSTALL(
  FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindRobotinoAPI2.cmake
  DESTINATION cmake
)

INSTALL(
  FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/api2.pri
  DESTINATION .
)

FILE( GLOB REC_ROBOTINO_API2_PDBS "${GLOBAL_TARGET_DIR}/release/*.pdb" )
MESSAGE( "${REC_ROBOTINO_API2_PDBS}" )
INSTALL(
  FILES
  ${REC_ROBOTINO_API2_PDBS}
  DESTINATION bin
)

INSTALL(
  FILES
  CHANGELOG.txt
  DESTINATION .
)

IF( UNIX AND NOT APPLE )
	INSTALL(
		FILES
		${CMAKE_CURRENT_BINARY_DIR}/robotino_api2.conf
		DESTINATION "${INSTALL_DIR}/etc/ld.so.conf.d"
	)
ENDIF()

IF(APPLE)
	IF(IOS)
		SET( PLATFORMS Debug-iphoneos Release-iphoneos Debug-iphonesimulator Release-iphonesimulator )
		SET( POSTFIX_LIST iphoneos_debug iphoneos_release iphonesimulator_debug iphonesimulator_release )
	ELSE()
		SET( PLATFORMS Debug Release )
		SET( POSTFIX_LIST clang64_debug clang64_release )
	ENDIF()
	
	SET( POSTFIX_INDEX 0 )
	FOREACH( platform ${PLATFORMS} )
	MESSAGE( ${platform} )
	MESSAGE( ${POSTFIX_INDEX} )
	LIST( GET POSTFIX_LIST ${POSTFIX_INDEX} POSTFIX )
	MESSAGE( ${POSTFIX} )
	SET( LIBS librec_robotino_api2.a librec_robotino_rpc.a librec_rpc.a librec_robotino3_fleetcom.a librec_cv_lt.a )
	#file(GLOB LIBS "${CMAKE_CURRENT_BINARY_DIR}/${platform}/*.a")
		FOREACH(var ${LIBS})
			GET_FILENAME_COMPONENT( FILENAME "${CMAKE_CURRENT_BINARY_DIR}/${platform}/${var}" NAME_WE )
			MESSAGE( ${FILENAME})
			MESSAGE( ${var} )
				INSTALL(
					FILES
					"${CMAKE_CURRENT_BINARY_DIR}/${platform}/${var}"
					DESTINATION lib
					RENAME ${FILENAME}_${POSTFIX}.a
				)
		ENDFOREACH()
		math(EXPR POSTFIX_INDEX "${POSTFIX_INDEX} + 1")
	ENDFOREACH()
ENDIF(APPLE)

#FILE( WRITE ${CMAKE_CURRENT_BINARY_DIR}/Publish_documentation.cmake 
#	"EXECUTE_PROCESS(
#	COMMAND ${RECFTP} \"${CMAKE_INSTALL_PREFIX}/doc\" ftp://$ENV{DOC_ROBOTINO_API2_ORG_FTP_USER}:$ENV{DOC_ROBOTINO_API2_ORG_FTP_PASSWD}@doc.openrobotino.org:/documentation/robotino/common
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
#		ROBOTINO_API2_doxygen
#	)

SET( CHANGELOG_NAME CHANGELOG_ROBOTINO_API2.txt )
SET( CHANGELOG_FILE "${CMAKE_CURRENT_BINARY_DIR}/${CHANGELOG_NAME}" )
CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG.txt" "${CHANGELOG_FILE}" COPYONLY )

SET( PACKAGE_NAME robotino-api2 )
SET( PACKAGE_VERSION ${ROBOTINO_API2_VERSION} )
INCLUDE( CreateInstaller )
	
IF( INNOSETUP_FOUND )
  FILE( WRITE ${CMAKE_CURRENT_BINARY_DIR}/CreateInstaller.cmake 
		"
STRING( REPLACE \"robotino_api2\" \"robotino_examples\" EXAMPLES_INSTALL_DIR \"${CMAKE_CURRENT_BINARY_DIR}/install\" )
STRING( REPLACE \"robotino_api2\" \"robotino_api2_wrapper\" WRAPPER_INSTALL_DIR \"${CMAKE_CURRENT_BINARY_DIR}/install\" )
FIND_PATH( MATLAB_DIR name \"startup.m\" PATHS \"${CMAKE_CURRENT_SOURCE_DIR}/../matlab\" )
#MESSAGE( \"\${EXAMPLES_INSTALL_DIR}\" )
FIND_FILE( EXAMPLES_CIRCLE example_circle.exe PATHS \"\${EXAMPLES_INSTALL_DIR}/bin\" NO_DEFAULT_PATH )
FIND_FILE( DOTNET_WRAPPER_DLL rec_robotino_api2_dotnet.dll PATHS \"\${WRAPPER_INSTALL_DIR}/bin\" NO_DEFAULT_PATH )
FIND_FILE( JAVA_WRAPPER_DLL rec_robotino_api2_java.jar PATHS \"\${WRAPPER_INSTALL_DIR}/bin\" NO_DEFAULT_PATH )
IF( EXAMPLES_CIRCLE ) 
	EXECUTE_PROCESS(
		COMMAND \"${CMAKE_COMMAND}\" -E copy_directory \"\${EXAMPLES_INSTALL_DIR}/bin\" \"${CMAKE_CURRENT_BINARY_DIR}/install/bin\"
		COMMAND \"${CMAKE_COMMAND}\" -E copy_directory \"\${EXAMPLES_INSTALL_DIR}/src/c++\" \"${CMAKE_CURRENT_BINARY_DIR}/install/examples/c++\"
		COMMAND \"${CMAKE_COMMAND}\" -E copy_directory \"\${EXAMPLES_INSTALL_DIR}/src/c\" \"${CMAKE_CURRENT_BINARY_DIR}/install/examples/c\"
	)
ELSE()
	MESSAGE( FATAL_ERROR \"Missing examples\" )
ENDIF()

IF( MATLAB_DIR )
	EXECUTE_PROCESS(
		COMMAND \"${CMAKE_COMMAND}\" -E copy_directory \"\${MATLAB_DIR}\" \"${CMAKE_CURRENT_BINARY_DIR}/install/matlab\"
	)
	EXECUTE_PROCESS(
		COMMAND \"${CMAKE_COMMAND}\" -E remove_directory \"${CMAKE_CURRENT_BINARY_DIR}/install/matlab/.svn\"
	)
ELSE()
	MESSAGE( FATAL_ERROR \"Missing matlab\" )
ENDIF()

IF( DOTNET_WRAPPER_DLL AND JAVA_WRAPPER_DLL ) 
	EXECUTE_PROCESS(
		COMMAND \"${CMAKE_COMMAND}\" -E copy_directory \"\${WRAPPER_INSTALL_DIR}/bin\" \"${CMAKE_CURRENT_BINARY_DIR}/install/bin\"
		COMMAND \"${CMAKE_COMMAND}\" -E copy_directory \"\${WRAPPER_INSTALL_DIR}/examples\" \"${CMAKE_CURRENT_BINARY_DIR}/install/examples\"
	)
ELSE()
	MESSAGE( WARNING \"No wrapper\" )
ENDIF()

EXECUTE_PROCESS(
		COMMAND \"${INNOSETUP_COMPILER}\" \"/cc\" \"${CMAKE_CURRENT_BINARY_DIR}/robotino-api2.iss\"
	)
	")
	
ENDIF( INNOSETUP_FOUND )

IF( UNIX AND NOT APPLE )
	FILE( WRITE ${CMAKE_CURRENT_BINARY_DIR}/CreateInstaller.cmake
		"STRING( REPLACE \"api2\" \"wrapper\" WRAPPER_INSTALL_DIR \"${CMAKE_CURRENT_BINARY_DIR}/install\" )
		FIND_FILE( JAVA_WRAPPER_DLL rec_robotino_api2_java.jar PATHS \"\${WRAPPER_INSTALL_DIR}/usr/local/robotino/wrapper/bin\" NO_DEFAULT_PATH )
		IF( JAVA_WRAPPER_DLL )
			MESSAGE( \"Copy \${WRAPPER_INSTALL_DIR}/usr/local/robotino/wrapper/bin -> ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}/bin\" )
			MESSAGE( \"Copy \${WRAPPER_INSTALL_DIR}/usr/local/robotino/wrapper/examples -> ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}/wrapper/examples\" )
			MESSAGE( \"Copy \${WRAPPER_INSTALL_DIR}/usr/local/robotino/wrapper/lib -> ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}/lib\" )
			EXECUTE_PROCESS(
				COMMAND \"${CMAKE_COMMAND}\" -E copy_directory \"\${WRAPPER_INSTALL_DIR}/usr/local/robotino/wrapper/bin\" \"${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}/bin\"
				COMMAND \"${CMAKE_COMMAND}\" -E copy_directory \"\${WRAPPER_INSTALL_DIR}/usr/local/robotino/wrapper/examples\" \"${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}/wrapper/examples\"
				COMMAND \"${CMAKE_COMMAND}\" -E copy_directory \"\${WRAPPER_INSTALL_DIR}/usr/local/robotino/wrapper/lib\" \"${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}/lib\"
			)
		ELSE( JAVA_WRAPPER_DLL )
			MESSAGE( FATAL_ERROR \"Missing java wrapper\" )
		ENDIF( JAVA_WRAPPER_DLL )

		EXECUTE_PROCESS(
			COMMAND \"${CMAKE_CURRENT_BINARY_DIR}/create_packages.sh\"
		)
	")
	ADD_CUSTOM_TARGET(
		ALL_CREATE_INSTALLER
		COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/CreateInstaller.cmake
		COMMENT "create installer"
	)
ENDIF()

