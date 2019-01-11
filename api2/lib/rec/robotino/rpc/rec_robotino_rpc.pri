DEFINES += rec_robotino_rpc_EXPORTS

VERSION_FILE = $$OUT_PWD/rec/robotino/rpc/rec_robotino_rpc_version.h
VERSION_CONTENT = "$${LITERAL_HASH}define MajorVer 1"
VERSION_CONTENT += "$${LITERAL_HASH}define MinorVer 0"
VERSION_CONTENT += "$${LITERAL_HASH}define PatchVer 0"
VERSION_CONTENT += "$${LITERAL_HASH}define Suffix \"na\""
VERSION_CONTENT += "$${LITERAL_HASH}define BuildVer 100"
VERSION_CONTENT += "$${LITERAL_HASH}define VersionString \"100\""
write_file( $$VERSION_FILE, VERSION_CONTENT )

SOURCES += \
    $$PWD/Client.cpp \
    $$PWD/Server.cpp \
    $$PWD/common.cpp
	
HEADERS += \
    $$VERSION_FILE \
    $$PWD/defines.h \
    $$PWD/Client.h \
    $$PWD/Server.h \
    $$PWD/common.h \
    $$PWD/Actor.h \
    $$PWD/GrapplerServoInfo.h \
    $$PWD/ProcessStatus.h \
    $$PWD/ProcessOutput.h \
	
include($$PWD/messages/messages.pri)
