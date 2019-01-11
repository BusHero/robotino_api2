#-------------------------------------------------
#
# Project created by QtCreator 2016-08-04T14:53:53
#
#-------------------------------------------------

QT       += network xml

QT       -= gui

TARGET = rec_robotino_api2
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD/lib $$OUT_PWD
message(Include path: $$INCLUDEPATH)

include($$PWD/lib/rec/robotino/api2/rec_robotino_api2.pri)

target.path = $$PWD/install/rec_robotino_api2
INSTALLS += target
