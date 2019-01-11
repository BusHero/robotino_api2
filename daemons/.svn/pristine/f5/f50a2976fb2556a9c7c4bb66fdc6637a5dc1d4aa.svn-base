SET(
  NAME
  lcdd2cmd
)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../lcdd2
)

FILE( GLOB SRCS *.cpp )
FILE( GLOB HEADER *.h )

SET(
	LCDD2_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/../lcdd2/AbstractDisplayDriver.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../lcdd2/DisplayDriver.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../lcdd2/ParPort.cpp	
)

ADD_EXECUTABLE(
  ${NAME}
  ${SRCS}
  ${HEADER}
  ${LCDD2_SRCS}
)

IF(WIN32)
SET_TARGET_PROPERTIES(
  ${NAME}
  PROPERTIES
  COMPILE_FLAGS "/wd4251 /MP"
)
ENDIF( WIN32 )

TARGET_LINK_LIBRARIES(
	${NAME}
)

INSTALL(
  TARGETS ${NAME}
  RUNTIME DESTINATION bin
)

INSTALL(
	FILES
	lcdd2cmd.sh
	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
	DESTINATION "${INSTALL_DIR}/etc/init.d"
)

GENERATE_VERSION( ${NAME} "${CMAKE_CURRENT_BINARY_DIR}/lcdd2cmd_version.h" ${ROBOTINO_DAEMONS_VERSION} "Lcd Cmd" "lcdd2cmd" )

STRING( REPLACE ";" " " SRCLIST "${SRCS}" )
STRING( REPLACE ";" " " SRCLIST2 "${LCDD2_SRCS}" )

FILE( WRITE ${CMAKE_CURRENT_BINARY_DIR}/CreateStaticLcdd2cmd.cmake 
	"EXECUTE_PROCESS(
		COMMAND \"ln -s -f `g++ -print-file-name=libstdc++.a` ${CMAKE_CURRENT_BINARY_DIR}\"
		TIMEOUT 100
	)

	EXECUTE_PROCESS(
		COMMAND g++ -static -static-libgcc -L${CMAKE_CURRENT_BINARY_DIR} -I${CMAKE_CURRENT_SOURCE_DIR}/../lcdd2 -I${CMAKE_CURRENT_BINARY_DIR} -o ${GLOBAL_TARGET_DIR}/${NAME}static ${SRCLIST} ${SRCLIST2}
		TIMEOUT 100
	)
")

ADD_CUSTOM_TARGET(
	lcdd2cmdstatic
	COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/CreateStaticLcdd2cmd.cmake
	COMMENT "create static lcdd2cmd"
)

#ln -s `g++ -print-file-name=libstdc++.a`
#g++ -static -static-libgcc -L. -I/home/robotino/proj/svn.openrobotino.org/daemons/trunk/apps/lcdd2 -Iapps/lcdd2cmd -o lcdd2cmd /home/robotino/proj/svn.openrobotino.org/daemons/trunk/apps/lcdd2cmd/main.cpp /home/robotino/proj/svn.openrobotino.org/daemons/trunk/apps/lcdd2/AbstractDisplayDriver.cpp /home/robotino/proj/svn.openrobotino.org/daemons/trunk/apps/lcdd2/DisplayDriver.cpp /home/robotino/proj/svn.openrobotino.org/daemons/trunk/apps/lcdd2/ParPort.cpp