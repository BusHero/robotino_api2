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

MACRO( GENERATE_MANIFEST NAME TARGET_FILEPATH VERSION EXENAME ARCH TYPE )

IF( WIN32 )

STRING( LENGTH "${VERSION}" VERSION_LENGTH )

IF( VERSION_LENGTH LESS 1 )
	SET( VERSION_SPLIT_LENGTH 0 )
ELSE( VERSION_LENGTH LESS 1 )
	STRING( REGEX MATCHALL [0-9]+ VERSION_SPLIT ${VERSION} )
	LIST( LENGTH VERSION_SPLIT VERSION_SPLIT_LENGTH )
ENDIF( VERSION_LENGTH LESS 1 )

IF( VERSION_SPLIT_LENGTH LESS 3 )
	SET( MAJOR_VER 0 )
	SET( MINOR_VER 0 )
	SET( PATCH_VER 0 )
ELSE( VERSION_SPLIT_LENGTH LESS 3 )
	LIST( GET VERSION_SPLIT 0 MAJOR_VER )
	LIST( GET VERSION_SPLIT 1 MINOR_VER )
	LIST( GET VERSION_SPLIT 2 PATCH_VER )
	STRING( REGEX MATCH [a-z,A-Z]+.* VER_SUFFIX ${VERSION} )
ENDIF( VERSION_SPLIT_LENGTH LESS 3 )

FILE( WRITE "${TARGET_FILEPATH}"
"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>
<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\">
  <assemblyIdentity version=\"${MAJOR_VER}.${MINOR_VER}.${PATCH_VER}.0\" processorArchitecture=\"${ARCH}\" name=\"${EXENAME}\" type=\"${TYPE}\"/>
  <v3:trustInfo xmlns:v3=\"urn:schemas-microsoft-com:asm.v3\">
    <v3:security>
      <v3:requestedPrivileges>
        <v3:requestedExecutionLevel level=\"asInvoker\" uiAccess=\"false\" />
      </v3:requestedPrivileges>
    </v3:security>
  </v3:trustInfo>
  <compatibility xmlns=\"urn:schemas-microsoft-com:compatibility.v1\">
    <application>
      <!--The ID below indicates application support for Windows Vista -->
      <supportedOS Id=\"{e2011457-1546-43c5-a5fe-008deee3d3f0}\"/>
      <!--The ID below indicates application support for Windows 7 -->
      <supportedOS Id=\"{35138b9a-5d96-4fbd-8e2d-a2440225f93a}\"/>
    </application>
  </compatibility>
</assembly>
" )

SET_TARGET_PROPERTIES( ${NAME} PROPERTIES LINK_FLAGS "/MANIFEST:NO" )

ADD_CUSTOM_COMMAND( TARGET ${NAME} POST_BUILD
	COMMAND "mt.exe" -manifest \"${TARGET_FILEPATH}\" -outputresource:"$(TargetDir)$(TargetFileName)"\;\#1
	COMMENT "Adding manifest..." )

ENDIF( WIN32 )

ENDMACRO( GENERATE_MANIFEST )
