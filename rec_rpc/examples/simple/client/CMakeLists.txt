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

SET(
  NAME
  rec_rpc_simple_client_test
)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

SET(
	HEADER
	Client.h
	Handler.h
	../Serialization.h
	../SignalHandler.h
)

SET(
	SRCS
	main.cpp
	Client.cpp
	Handler.cpp
	../Serialization.cpp
)

IF( WIN32 )
	SET( HEADER ${HEADER} ../SignalHandler_win32.h )
	SET( SRCS ${SRCS} ../SignalHandler_win32.cpp "${CMAKE_CURRENT_BINARY_DIR}/rec_rpc_simple_client_test_version.rc" )
ELSE( WIN32 )
	SET( HEADER ${HEADER} ../SignalHandler_linux.h )
	SET( SRCS ${SRCS} ../SignalHandler_linux.cpp )
ENDIF( WIN32 )

INCLUDE_DIRECTORIES( .. )

SET( MOC_SRCS )

QT_WRAP_CPP(
	${NAME}
	MOC_SRCS
	${HEADER}
)

ADD_EXECUTABLE(
	${NAME}
	${SRCS}
	${HEADER}
	${MOC_SRCS}
)

IF(WIN32 AND NOT MINGW)
SET_TARGET_PROPERTIES(
	${NAME}
	PROPERTIES
	COMPILE_FLAGS "/wd4251 /MP"
)
ENDIF(WIN32 AND NOT MINGW)

TARGET_LINK_LIBRARIES(
	${NAME}
	${QT_QTCORE_LIBRARY}
	rec_rpc
)

GENERATE_VERSION_RC( ${NAME} ${CMAKE_CURRENT_BINARY_DIR}/rec_rpc_simple_client_test_version.rc "${REC_RPC_VERSION}" "REC RPC simple client test application" "${NAME}" "${NAME}.exe" 0 )

SOURCE_GROUP( "Source Files" FILES ${SRCS} ${HEADER} )
SOURCE_GROUP( Moc REGULAR_EXPRESSION "(moc_|qrc_)" )

INSTALL(
	TARGETS ${NAME}
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)