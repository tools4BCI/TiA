TEMPLATE += app
CONFIG += console
DESTDIR = tests
TARGET = tia_test
OBJECTS_DIR = tmp/tests/

QT -= core \
      gui


INCLUDEPATH += . \
    include \
    extern/include
DEPENDPATH += $$INCLUDEPATH

#PRE_TARGETDEPS += lib/libtia.a

DEFINES += TIXML_USE_TICPP
#DEFINES += DEBUG

#QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
#QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
#LIBS += -lgcov

QMAKE_CXXFLAGS_WARN_ON = -Wall \
    -pedantic

HARDWARE_PLATFORM = $$system(uname -m)

contains( HARDWARE_PLATFORM, x86_64 )::{
  LIBS += lib/amd64/libtia.a
  LIBS += extern/lib/ticpp/linux/libticpp_64.a
  }
  else:: {
  LIBS += lib/x86/libtia.a
  LIBS += extern/lib/ticpp/linux/libticpp.a
  }

LIBS += -lboost_thread \
        -lboost_system \
        -lboost_filesystem \
        -Lextern/lib \
        -Lextern/lib/ticpp/linux \
        -Ltests/UnitTest++

contains( HARDWARE_PLATFORM, x86_64 )::{
  LIBS += -lUnitTest++_64
  }
  else:: {
  LIBS += -lUnitTest++
  }


SOURCES += \
    tests/main.cpp \
    tests/datapacket_tests.cpp \
    tests/tia_client_tests.cpp \
    tests/tia_clock_tests.cpp \
    tests/tia_server_tests.cpp \
    #tests/server_control_connection_tests.cpp \
    tests/tia_version_1_0/tia_control_message_parser_1_0_tests.cpp \
    tests/test_socket.cpp \
    tests/boost_socket_tests.cpp \
    tests/control_commands/get_data_transmission.cpp \
    tests/tia_version_1_0/tia_tcp_server_socket_tests.cpp \
    tests/tia_server_state_server_tests.cpp \
    tests/tia_version_1_0/tia_custom_signal_info_build_functions_test.cpp

HEADERS += \
    tests/datapacket_tests_fixtures.h \
    tests/raw_datapacket_definition.h \
    #tests/tia_server_control_connection_tests_fixtures.h \
    tests/tia_control_messages_definition.h \
    tests/test_socket.h \
    tests/test_data_server.h
