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

cmake_minimum_required( VERSION 2.6 )

SET( NAME rec_rpc_ros_client_examples )

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/../
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

file( GLOB HEADER src/*.h )
file( GLOB SRCS src/*.cpp )

IF( WIN32 )
	SET( SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/rec_rpc_ros_client_examples_version.rc" )
ENDIF( WIN32 )

file( GLOB SERIALIZATION ${CMAKE_CURRENT_SOURCE_DIR}/../serialization/*.h )

SET( MOC_SRCS "" )

QT4_WRAP_CPP(
	MOC_SRCS
	${HEADER}
)

SET(
	RCCS
	images.qrc
)
	
QT4_ADD_RESOURCES(
  RCC_SRCS
  ${RCCS}
)

add_executable(
	${NAME}
	WIN32
	${SRCS}
	${HEADER}
	${MOC_SRCS}
	${RCC_SRCS}
	
	${SERIALIZATION}
)

IF(WIN32)
	SET_TARGET_PROPERTIES( ${NAME} PROPERTIES COMPILE_FLAGS "/wd4251 /MP" )
ENDIF( WIN32 )

target_link_libraries(
	${NAME}
	${APPICON}
	${QT_QTMAIN_LIBRARY}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTSVG_LIBRARY}
	${QT_QTXML_LIBRARY}
	rec_rpc
	${LIBREC_RPC}
)

GENERATE_VERSION_RC( ${NAME} ${CMAKE_CURRENT_BINARY_DIR}/rec_rpc_ros_client_examples_version.rc "${REC_RPC_VERSION}" "REC RPC ros client examples application" "${NAME}" "${NAME}.exe" 0 )

SOURCE_GROUP( Moc REGULAR_EXPRESSION "(moc_|qrc_)" )
SOURCE_GROUP( "Source Files" FILES ${SRCS} ${HEADER} )
SOURCE_GROUP( "serialization" FILES ${SERIALIZATION} )

INSTALL(
	TARGETS 
	${NAME}
	DESTINATION bin
)
	