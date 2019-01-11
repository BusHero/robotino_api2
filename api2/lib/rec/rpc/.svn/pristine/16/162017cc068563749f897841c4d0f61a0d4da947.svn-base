DEFINES += rec_rpc_EXPORTS

VERSION_FILE = $$OUT_PWD/rec/rpc/rec_rpc_version.h
VERSION_CONTENT = "$${LITERAL_HASH}define MajorVer 1"
VERSION_CONTENT += "$${LITERAL_HASH}define MinorVer 0"
VERSION_CONTENT += "$${LITERAL_HASH}define PatchVer 0"
VERSION_CONTENT += "$${LITERAL_HASH}define Suffix \"na\""
VERSION_CONTENT += "$${LITERAL_HASH}define BuildVer 100"
VERSION_CONTENT += "$${LITERAL_HASH}define VersionString \"100\""
write_file( $$VERSION_FILE, VERSION_CONTENT )

SOURCES += \
    $$PWD/rec_rpc_Client.cpp \
    $$PWD/rec_rpc_common.cpp \
    $$PWD/rec_rpc_Exception.cpp \
    $$PWD/rec_rpc_Server.cpp \
    $$PWD/rec_rpc_Socket.cpp
	
HEADERS += \
	$$VERSION_FILE \
	$$PWD/defines.h \
	$$PWD/Client.h \
	$$PWD/Server.h \
    $$PWD/rec_rpc_Client.h \
    $$PWD/rec_rpc_ClientInfo.h \
    $$PWD/rec_rpc_common.h \
    $$PWD/rec_rpc_common_internal.hpp \
    $$PWD/rec_rpc_Exception.h \
    $$PWD/rec_rpc_Server.h \
    $$PWD/rec_rpc_Socket.hpp \
    $$PWD/rec_rpc_utils.h
	
include($$PWD/client/client.pri)
include($$PWD/configuration/configuration.pri)
include($$PWD/messages/messages.pri)
include($$PWD/serialization/serialization.pri)
include($$PWD/server/server.pri)
