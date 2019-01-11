#-------------------------------------------------
#
# Project created by QtCreator 2016-08-04T14:53:53
#
#-------------------------------------------------

QT       += network xml

QT       -= gui

TARGET = rec_robotino_rpc
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD/lib $$OUT_PWD
message(Include path: $$INCLUDEPATH)

include($$PWD/lib/rec/robotino/rpc/rec_robotino_rpc.pri)

target.path = $$PWD/install/rec_robotino_rpc
INSTALLS += target
