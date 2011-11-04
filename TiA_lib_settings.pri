TEMPLATE = lib
VERSION = 1.0
CONFIG += thread \
    release \
    warn_on \
    exceptions \
    stl
QT -= core \
    gui

DEFINES += TIXML_USE_TICPP
#DEFINES += TIMING_TEST

TARGET = tia
DESTDIR = lib
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

#unix {
#    LIBS += -lboost_thread \
#        -lboost_system

#    HARDWARE_PLATFORM = $$system(uname -m)
#    contains( HARDWARE_PLATFORM, x86_64 ):: {
#        message(Building 64 bit )

#        # 64-bit Linux
#        LIBS += extern/lib/ticpp/linux/libticpp_64.a
#    }
#    else:: {
#        # 32-bit Linux
#        message(Building 32 bit )
#        LIBS += extern/lib/ticpp/linux/libticpp.a
#    }
#}

