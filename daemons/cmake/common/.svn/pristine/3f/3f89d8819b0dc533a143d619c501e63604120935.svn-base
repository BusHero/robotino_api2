IF( MSVC90 )
	SET( ATL_INCLUDE_PATH "$ENV{ProgramFiles(x86)}/Microsoft Visual Studio 9.0/VC/atlmfc/include" )
ELSEIF( MSVC10 )
	SET( ATL_INCLUDE_PATH "$ENV{ProgramFiles(x86)}/Microsoft Visual Studio 10.0/VC/atlmfc/include" )
ENDIF( MSVC90 )

IF( ATL_INCLUDE_PATH )
	FIND_FILE(
		ATL
		NAMES atlstr.h
		PATHS "${ATL_INCLUDE_PATH}"
		NO_DEFAULT_PATH
	)
	IF( ATL )
		ADD_DEFINITIONS( -DHAVE_ATL )
		SET( ATL_FOUND 1 )
	ELSE( ATL )
		SET( ATL_FOUND 0 )
	ENDIF( ATL )
ENDIF( ATL_INCLUDE_PATH )