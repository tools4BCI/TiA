#-----------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += TiA_lib_static.pro \
           TiA_lib_shared.pro \
           TiA_client.pro \
           #TiA_server.pri \
           TiA_tests.pro

#TOBI_tia_tests.pri.depends = TOBI_SignalServer_tialib_static.pri

#-----------------------------------------------------------------------
#! end of file
