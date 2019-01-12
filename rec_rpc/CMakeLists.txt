#Copyright (c) 2011, REC Robotics Equipment Corporation GmbH, Planegg, Germany
#All rights reserved.

#Redistribution and use in source and binary forms, with or without modification,
#are permitted provided that the following conditions are met:

#- Redistributions of source code must retain the above copyright notice,
#  this list of conditions and the following disclaimer.
#- Redistributions in binary form must reproduce the above copyright notice,
#  this list of conditions and the following disclaimer in the documentation and/or
#  other materials provided with the distribution.
#- Neither the name of the REC Robotics Equipment Corporation GmbH nor the names of
#  its contributors may be used to endorse or promote products derived from this software
#  without specific prior written permission.

#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
#OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
#AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
#CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
#IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
#OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required( VERSION 3.0 )

PROJECT( REC_RPC )

SET( REC_RPC_VERSION 1.6.0 CACHE STRING "REC RPC Version" FORCE )
STRING( REGEX MATCHALL [0-9]+ VERSION_SPLIT ${REC_RPC_VERSION} )
LIST( GET VERSION_SPLIT 0 REC_RPC_MAJOR_VER )
LIST( GET VERSION_SPLIT 1 REC_RPC_MINOR_VER )
LIST( GET VERSION_SPLIT 2 REC_RPC_PATCH_VER )
STRING( REGEX MATCH [a-z,A-Z]+.* REC_RPC_VER_SUFFIX ${REC_RPC_VERSION} )

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	PROJECT( REC_RPC_x64 )
	SET( ARCHITECTURE amd64 )
ELSE( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	SET( ARCHITECTURE i386 )
ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )

SET( REC_RPC_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
SET( REC_RPC_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" )

SET( DO_REC_RPC_INSTALL 1 )

SET(
	CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/cmake
	${CMAKE_CURRENT_SOURCE_DIR}/utils
)
IF( WIN32)
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
	SET( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -mmmx" )
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmmx" )

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
		SET( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -msse -msse2 -mfpmath=sse" )
		SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -mfpmath=sse" )
	ENDIF( ROBOTINO_OPTIMIZATION )
ENDIF( WIN32 )

INCLUDE( GenerateVersion )

SET(
  GLOBAL_TARGET_DIR
  ${CMAKE_CURRENT_BINARY_DIR}/target
)
FILE( MAKE_DIRECTORY ${GLOBAL_TARGET_DIR} )
FILE( MAKE_DIRECTORY ${GLOBAL_TARGET_DIR}/debug )
FILE( MAKE_DIRECTORY ${GLOBAL_TARGET_DIR}/release )
    
SET(
  EXECUTABLE_OUTPUT_PATH
  ${GLOBAL_TARGET_DIR}
)
  
SET(
  LIBRARY_OUTPUT_PATH
  ${GLOBAL_TARGET_DIR}
)

find_package( Doxygen )

INCLUDE( MyQt )

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/lib
)

SET( INSTALL_ROOT ${GLOBAL_INSTALL_DIR} )

IF( WIN32 )
	SET(
		CMAKE_INSTALL_PREFIX
		${INSTALL_ROOT}
	)
	SET(
		CMAKE_DEBUG_POSTFIX
		"d"
	)
ELSE( WIN32 )
	SET(
		CMAKE_INSTALL_PREFIX
		${INSTALL_ROOT}/usr/local/rec_rpc
	)
ENDIF( WIN32 )

FILE( MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX} )

ADD_SUBDIRECTORY( lib/rec/rpc )
ADD_SUBDIRECTORY( examples )

IF( DOXYGEN_FOUND )
	ADD_SUBDIRECTORY( doc )
ENDIF( DOXYGEN_FOUND )


SET( CHANGELOG_NAME "CHANGELOG_rec_rpc.txt" )
SET( CHANGELOG_FILE "${CMAKE_CURRENT_BINARY_DIR}/${CHANGELOG_NAME}" )
CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG.txt" "${CHANGELOG_NAME}" COPYONLY )

IF( QT5_FOUND )
	SET( DEPENDS "qt5-default" )
ELSE()
	SET( DEPENDS "libqtcore4, libqt4-network, libqt4-xml" )
ENDIF()

IF( WIN32 )
ELSE( WIN32 )
	FILE( MAKE_DIRECTORY ${INSTALL_ROOT}/DEBIAN )
	FILE( WRITE ${INSTALL_ROOT}/DEBIAN/control
	"Package: rec-rpc
Version: ${REC_RPC_MAJOR_VER}.${REC_RPC_MINOR_VER}.${REC_RPC_PATCH_VER}
Section: devel
Priority: optional
Architecture: ${ARCHITECTURE}
Maintainer: REC GmbH
Depends: ${DEPENDS}
Replaces: rec-rpc-qt4.8.0
Description: Binary distribution of REC's Qt based RPC library
")

	SET( INSTALLER_NAME rec-rpc_${REC_RPC_MAJOR_VER}.${REC_RPC_MINOR_VER}.${REC_RPC_PATCH_VER}_${ARCHITECTURE}.deb )

	FILE( WRITE ${INSTALL_ROOT}/DEBIAN/postinst
	"#!/bin/sh
set -e
. /usr/share/debconf/confmodule

if [ \"\$1\" = \"configure\" ]; then
	chown -R root:root /usr/local/rec_rpc
	chown root:root /etc/ld.so.conf.d/rec_rpc.conf
	ldconfig
fi
")
	execute_process( COMMAND chmod +x ${INSTALL_ROOT}/DEBIAN/postinst )

	FILE( MAKE_DIRECTORY ${INSTALL_ROOT}/etc/ld.so.conf.d )
	FILE( WRITE ${INSTALL_ROOT}/etc/ld.so.conf.d/rec_rpc.conf
	"/usr/local/rec_rpc/lib"
	)
	
	FILE( WRITE ${CMAKE_CURRENT_BINARY_DIR}/create_packages.sh 
	"#!/bin/bash
dpkg-deb --build install .
")
	EXECUTE_PROCESS( COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/create_packages.sh TIMEOUT 100 )

	FILE( WRITE ${CMAKE_CURRENT_BINARY_DIR}/excludes
	"trash
bin
")
ENDIF(WIN32)

SET( PACKAGE_NAME rec-rpc )
SET( PACKAGE_VERSION ${REC_RPC_VERSION} )
INCLUDE( CreateInstaller )

FILE( WRITE "${CMAKE_INSTALL_PREFIX}/FindRecRpc.cmake"
"IF( NOT RECRPC_DIR )
	IF( WIN32 )
		IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
			STRING( REPLACE \"\\\\\" \"/\" RECRPC_DIR \"\$ENV{RECRPC64_DIR}\" )
		ELSE( CMAKE_SIZEOF_VOID_P EQUAL 8 )
			STRING( REPLACE \"\\\\\" \"/\" RECRPC_DIR \"\$ENV{RECRPC32_DIR}\" )
		ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	ELSE( WIN32 )
			SET( RECRPC_DIR \"/usr/local/rec_rpc\" )
	ENDIF( WIN32 )
ENDIF( NOT RECRPC_DIR )

IF( NOT RECRPC_DIR )
	IF( WIN32 )
		IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
			STRING( REPLACE \"\\\\\" \"/\" PROGRAMS \"\$ENV{ProgramFiles}\" )
			SET( RECRPC_DIR \"\${PROGRAMS}/REC GmbH/rpc\" )
		ELSE( CMAKE_SIZEOF_VOID_P EQUAL 8 )
			STRING( REPLACE \"\\\\\" \"/\" PROGRAMS \"\$ENV{ProgramFiles(x86)}\" )
			SET( RECRPC_DIR \"\${PROGRAMS}/REC GmbH/rpc\" )
		ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	ELSE( WIN32 )
		SET( RECRPC_DIR \"/usr/local/rec_rpc\" )
	ENDIF( WIN32 )
ENDIF( NOT RECRPC_DIR )

SET( REC_RPC_INCLUDE_DIR \"\${RECRPC_DIR}/include\" ) 
SET( REC_RPC_LIB_DIR \"\${RECRPC_DIR}/lib\" ) 
SET( REC_RPC_BIN_DIR \"\${RECRPC_DIR}/bin\" ) 

FIND_PATH(
	REC_RPC_INCLUDES
	rec/rpc/Client.h
	\${REC_RPC_INCLUDE_DIR}
)

FIND_LIBRARY(
	REC_RPC_RELEASE_LIBRARY
	NAMES 
	rec_rpc
	PATHS
	\${REC_RPC_LIB_DIR}
)

IF( WIN32 )
	FIND_LIBRARY(
		REC_RPC_DEBUG_LIBRARY
		NAMES 
		rec_rpcd
		PATHS
		\${REC_RPC_LIB_DIR}
	)

	SET( REC_RPC_LIBRARY optimized \"\${REC_RPC_RELEASE_LIBRARY}\" debug \"\${REC_RPC_DEBUG_LIBRARY}\" )

	FIND_FILE(
		REC_RPC_RELEASE_DLL
		rec_rpc.dll 
		\${REC_RPC_BIN_DIR}
		NO_DEFAULT_PATH 
	)
	FIND_FILE(
		REC_RPC_DEBUG_DLL
		rec_rpcd.dll 
		\${REC_RPC_BIN_DIR}
		NO_DEFAULT_PATH 
	)
ELSE( WIN32 )
	SET( REC_RPC_LIBRARY \"\${REC_RPC_RELEASE_LIBRARY}\" )
ENDIF( WIN32 )

MACRO( COPY_REC_RPC_DLLS releaseTarget debugTarget )
	CONFIGURE_FILE( \${REC_RPC_RELEASE_DLL} \${releaseTarget} COPYONLY )
    CONFIGURE_FILE( \${REC_RPC_DEBUG_DLL} \${debugTarget} COPYONLY )
ENDMACRO( COPY_REC_RPC_DLLS )

IF( LIBREC_RPC )
	SET( REC_RPC_LIB_FOUND TRUE )
ENDIF( LIBREC_RPC )

MARK_AS_ADVANCED( REC_RPC_LIB_FOUND )
MARK_AS_ADVANCED( REC_RPC_INCLUDES )
")

IF( WIN32 )
	INSTALL(
		FILES
		${REC_RPC_RELEASE_DLL}
		DESTINATION bin
	)
ENDIF( WIN32 )
