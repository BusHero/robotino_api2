#Copyright (c) 2013, REC Robotics Equipment Corporation GmbH, Planegg, Germany
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

SET( DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/rec_rpc" )
SET( DOXYGEN_IMAGE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/images" )
SET( INPUT "${CMAKE_CURRENT_SOURCE_DIR}/../lib/rec/rpc ${CMAKE_CURRENT_SOURCE_DIR}/../lib/rec/rpc/serialization" )

FILE( MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY} )

CONFIGURE_FILE( "Doxyfile.in" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY )

FILE( WRITE ${CMAKE_CURRENT_BINARY_DIR}/ExecDoxygen.cmake 
	"EXECUTE_PROCESS(
	COMMAND \"${DOXYGEN_EXECUTABLE}\" \"${CMAKE_CURRENT_BINARY_DIR}/Doxyfile\"
	TIMEOUT 10
)
")
	
ADD_CUSTOM_TARGET(
	rec_rpc_doc
	COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/ExecDoxygen.cmake
	COMMENT "creating doxygen documentation"
)

ADD_DEPENDENCIES(
	rec_rpc
	rec_rpc_doc
)

INSTALL(
	DIRECTORY
	"${DOXYGEN_OUTPUT_DIRECTORY}"
	DESTINATION doc
)

