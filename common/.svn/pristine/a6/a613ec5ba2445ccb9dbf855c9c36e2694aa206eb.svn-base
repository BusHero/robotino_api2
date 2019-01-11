DEFINES += rec_cv_lt_EXPORTS

VERSION_FILE = $$OUT_PWD/rec/robotino/rec_cv_lt_version.h
VERSION_CONTENT = "$${LITERAL_HASH}define MajorVer 1"
VERSION_CONTENT += "$${LITERAL_HASH}define MinorVer 0"
VERSION_CONTENT += "$${LITERAL_HASH}define PatchVer 0"
VERSION_CONTENT += "$${LITERAL_HASH}define Suffix \"na\""
VERSION_CONTENT += "$${LITERAL_HASH}define BuildVer 100"
VERSION_CONTENT += "$${LITERAL_HASH}define VersionString \"100\""
write_file( $$VERSION_FILE, VERSION_CONTENT )

SOURCES += \
    $$PWD/jpeg_dec.cpp \
    $$PWD/jpeg_enc.cpp \
    $$PWD/rgb2argb32.cpp \
    $$PWD/rgb2bgr.cpp \
    $$PWD/yuyv2rgb.cpp
	
HEADERS += \
    $$VERSION_FILE \
    $$PWD/jpeg.h \
    $$PWD/jpeg_mem.hpp \
    $$PWD/rgb2argb32.h \
    $$PWD/rgb2bgr.h \
    $$PWD/yuyv2rgb.h

INCLUDEPATH += $$PWD/jpeg
include($$PWD/jpeg/jpeg.pri)
