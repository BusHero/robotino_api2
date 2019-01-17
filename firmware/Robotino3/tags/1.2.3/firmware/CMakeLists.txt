cmake_minimum_required(VERSION 2.6)

PROJECT( ROBOTINO_FIRMWARE )

SET(
	INSTALL_DIR
	"${CMAKE_CURRENT_BINARY_DIR}/install"
)

SET(
	CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/common
)

SET( ARCHITECTURE amd64 )

SET(VERSIONFILE "${CMAKE_CURRENT_SOURCE_DIR}/src/version.c")

FILE(READ "${VERSIONFILE}" version_content)
STRING(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" VERSION_STRING ${version_content})
STRING(REGEX MATCH "(/\\*\\*).*(\\*/)" DETAILED_DESCRIPTION ${version_content})
STRING(REPLACE "/**" " " DETAILED_DESCRIPTION ${DETAILED_DESCRIPTION} )
STRING(REPLACE "*/" "" DETAILED_DESCRIPTION ${DETAILED_DESCRIPTION} )
STRING(REGEX REPLACE "\n" "\n "  DETAILED_DESCRIPTION ${DETAILED_DESCRIPTION} )

INCLUDE( Codename )
if( CODENAME STREQUAL "precise" )
  SET( LOGFILE /var/log/robotino3_firmware_update.log )
endif()

#MESSAGE( ${DETAILED_DESCRIPTION} )

FILE( MAKE_DIRECTORY ${INSTALL_DIR} )
FILE( MAKE_DIRECTORY ${INSTALL_DIR}/DEBIAN )
FILE( MAKE_DIRECTORY ${INSTALL_DIR}/usr/local/robotino/firmware )
FILE( MAKE_DIRECTORY ${INSTALL_DIR}/etc/udev/rules.d )

CONFIGURE_FILE( linux_install/DEBIAN/control  ${INSTALL_DIR}/DEBIAN/control @ONLY )
CONFIGURE_FILE( linux_install/DEBIAN/postinst  ${INSTALL_DIR}/DEBIAN/postinst COPYONLY )
CONFIGURE_FILE( linux_install/common/usr/local/robotino/firmware/install.sh  ${INSTALL_DIR}/usr/local/robotino/firmware/install.sh @ONLY )

if( CODENAME STREQUAL "xenial" )
  INSTALL(
    DIRECTORY
    "linux_install/xenial/"
    DESTINATION "${INSTALL_DIR}"
    PATTERN ".svn" EXCLUDE
  )
else()
  INSTALL(
    DIRECTORY
    "linux_install/precise/"
    DESTINATION "${INSTALL_DIR}"
    PATTERN ".svn" EXCLUDE
  )
endif()

CONFIGURE_FILE( linux_install/create_packages.sh.in  ${CMAKE_CURRENT_BINARY_DIR}/create_packages.sh @ONLY )

SET( CHANGELOG_NAME CHANGELOG_robotino3_firmware.txt )
SET( CHANGELOG_FILE "${INSTALL_DIR}/usr/local/robotino/firmware/${CHANGELOG_NAME}" )
FILE(WRITE "${CHANGELOG_FILE}" ${DETAILED_DESCRIPTION} )
FILE(APPEND "${CHANGELOG_FILE}" "\n\r" )

INSTALL(
  FILES
  keil/firmware.bin
  DESTINATION "${INSTALL_DIR}/usr/local/robotino/firmware"
  RENAME firmware-${VERSION_STRING}.bin
)

SET( PACKAGE_NAME robotino3-firmware )
SET( PACKAGE_VERSION ${VERSION_STRING} )
INCLUDE( CreateInstaller )