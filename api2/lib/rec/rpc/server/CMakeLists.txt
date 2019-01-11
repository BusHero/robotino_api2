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

SET( PATH server )
STRING( REPLACE "/" "_" PREFIX "${PATH}" )

FILE( GLOB ${PREFIX}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${PATH}/*.cpp )
FILE( GLOB ${PREFIX}_HEADER RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${PATH}/*.h )
FILE( GLOB ${PREFIX}_PRIVATE_HEADER RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${PATH}/*.hpp )


MY_WRAP_CPP(
    ${PREFIX}_MOC_SRCS
    "${PREFIX}"
    ${${PREFIX}_HEADER}
    ${${PREFIX}_PRIVATE_HEADER}
)

SOURCE_GROUP( ${PREFIX} FILES ${${PREFIX}_SRCS} ${${PREFIX}_HEADER} ${${PREFIX}_PRIVATE_HEADER} )

IF( DO_REC_RPC_INSTALL )
INSTALL(
	FILES
	${${PREFIX}_HEADER}
	DESTINATION ${REC_RPC_INCLUDE_INSTALL_DIR}/${PATH}
)
ENDIF( DO_REC_RPC_INSTALL )

SET(
	SRCS
	${SRCS}
	${${PREFIX}_SRCS}
)

SET(
	HEADER
	${HEADER}
	${${PREFIX}_HEADER}
)

SET(
	PRIVATE_HEADER
	${PRIVATE_HEADER}
	${${PREFIX}_PRIVATE_HEADER}
)

SET(
	MOC_SRCS
	${MOC_SRCS}
	${${PREFIX}_MOC_SRCS}
)
