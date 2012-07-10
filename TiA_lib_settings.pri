TEMPLATE = lib
VERSION = 1.0
CONFIG += thread \
    warn_on \
    exceptions \
    stl
QT -= core \
    gui

DEFINES += TIXML_USE_TICPP
#DEFINES += TIMING_TEST

CONFIG( debug, debug|release ){
    DEFINES += DEBUG
}else{

}

TARGET = tia

OBJECTS_DIR = tmp
INCLUDEPATH += . \
    include \
    extern/include/LptTools

DEPENDPATH += $$INCLUDEPATH
INCLUDEPATH += extern/include

unix:QMAKE_CXXFLAGS += -pedantic
QMAKE_CXXFLAGS_WARN_ON = -Wall \
    -pedantic

#--------------------------------------------------------------------

HARDWARE_PLATFORM = $$system(uname -m)
contains( HARDWARE_PLATFORM, x86_64 )::{
    message(Building 64 bit )
    DESTDIR = lib/amd64
  }else::{
    message(Building 32 bit )
    DESTDIR = lib/x86
  }

