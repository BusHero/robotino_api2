set(_pf_x86 "ProgramFiles(x86)")

IF( MSVC80 )
	SET( VCREDIST_DIR "$ENV{ProgramFiles}/Microsoft Visual Studio 8/SDK/v2.0/BootStrapper/Packages/vcredist_x86" )
ELSEIF( MSVC90 )
	SET( VCREDIST_DIR "$ENV{ProgramFiles}/Microsoft SDKs/Windows/v6.0A/Bootstrapper/Packages/vcredist_x86" )
	SET( VCREDIST_X64_DIR "$ENV{ProgramFiles}/Microsoft SDKs/Windows/v6.0A/Bootstrapper/Packages/vcredist_x64" )
ELSEIF( MSVC10 )
	SET( VCREDIST_DIR "$ENV{ProgramFiles}/Microsoft SDKs/Windows/v7.0A/Bootstrapper/Packages/vcredist_x86" )
	SET( VCREDIST_X64_DIR "$ENV{ProgramFiles}/Microsoft SDKs/Windows/v7.0A/Bootstrapper/Packages/vcredist_x64" )
ELSEIF( MSVC12 )
	SET( VCREDIST_DIR "$ENV{${_pf_x86}}/Microsoft Visual Studio 12.0/VC/redist/1033" )
	SET( VCREDIST_X64_DIR "${VCREDIST_DIR}" )
ELSEIF( MSVC14 )
	SET( VCREDIST_DIR "$ENV{${_pf_x86}}/Microsoft Visual Studio/2017/Professional/VC/Redist/MSVC/14.16.27012" )
	SET( VCREDIST_X64_DIR "${VCREDIST_DIR}" )
ENDIF( MSVC80 )

MESSAGE(STATUS "VCREDIST_DIR = ${VCREDIST_DIR}")
MESSAGE(STATUS "VCREDIST_X64_DIR = ${VCREDIST_X64_DIR}")

FIND_FILE( VCREDIST vcredist_x86.exe
	PATHS
	"${VCREDIST_DIR}"
 NO_DEFAULT_PATH )
 
 FIND_FILE( VCREDIST_X64 vcredist_x64.exe
	PATHS
	"${VCREDIST_X64_DIR}"
 NO_DEFAULT_PATH )
 
IF( VCREDIST )
      MESSAGE(STATUS "Found VCREDIST X86")
      SET( VCREDIST_FOUND YES )
ELSE (VCREDIST)
   SET( VCREDIST_FOUND NO )
   IF (VCRedist_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find VCREDIST X86")
   ENDIF (VCRedist_FIND_REQUIRED)
ENDIF (VCREDIST)

IF( VCREDIST_X64 )
      MESSAGE(STATUS "Found VCREDIST X64")
      SET( VCREDIST_X64_FOUND YES )
ELSE (VCREDIST_X64)
   SET( VCREDIST_X64_FOUND NO )
   IF (VCRedist_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find VCREDIST X64")
   ENDIF (VCRedist_FIND_REQUIRED)
ENDIF (VCREDIST_X64) 


