TEMPLATE = app
CONFIG += console \
    thread \
    warn_on \
    exceptions \
    stl
QT -= core \
    gui

DEFINES += TIXML_USE_TICPP

CONFIG(debug, debug|release) {
    DEFINES += DEBUG
}

TARGET = tia-client

OBJECTS_DIR = tmp
INCLUDEPATH += . \
    include
DEPENDPATH += $$INCLUDEPATH
INCLUDEPATH += extern/include

unix:!macx:QMAKE_CXXFLAGS += -pedantic
QMAKE_CXXFLAGS += -Wall

DESTDIR = bin

SOURCES += src/tia_client_main.cpp

unix:!macx {
    LIBS += -Lextern/lib/ticpp/linux \
            -lboost_thread -lboost_system

    contains(QMAKE_HOST.arch, x86_64) {
        message("x86_64 (64bit) build")
        LIBS += -Llib/amd64 -ltia -lticpp_64
    } else {
        message("x86 (32bit) build")
        LIBS += -Llib/x86 -ltia -lticpp
    }
}

macx:LIBS += -L/opt/local/lib -Lextern/lib/ticpp/macx -Llib/macx\
    -lboost_thread-mt -lboost_system-mt -ltia
win32:LIBS += lib/tia.lib

# Note: It is assumed that the boost libraries can be automatically detected by the linker
# through #pragma comment(lib, xxx) declarations in boost.
