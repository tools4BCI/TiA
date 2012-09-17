TEMPLATE = lib
VERSION = 1.0
CONFIG += thread \
    warn_on \
    exceptions \
    stl
QT -= core \
    gui

DEFINES += TIXML_USE_TICPP

CONFIG(debug, debug|release) {
    DEFINES += DEBUG
}

TARGET = tia

OBJECTS_DIR = tmp
INCLUDEPATH += . \
    include \
    extern/include

DEPENDPATH += $$INCLUDEPATH

linux:QMAKE_CXXFLAGS += -pedantic
QMAKE_CXXFLAGS += -Wall

macx:QMAKE_LFLAGS += -undefined dynamic_lookup

linux {
    contains(QMAKE_HOST.arch, x86_64) {
        message("x86_64 (64bit) build")
        DESTDIR = lib/amd64
    } else {
       message("x86 (32bit) build")
       DESTDIR = lib/x86
    }
} else {
    DESTDIR = lib/macx
}
