
HEADERS += include/tia/tia_server.h \
    include/tia/defines.h \
    include/tia/constants.h \
    include/tia-private/config/control_message_decoder.h \
    include/tia-private/config/control_message_encoder.h \
    include/tia-private/config/control_messages.h \
    include/tia/ss_meta_info.h \
    include/tia-private/datapacket/raw_mem.h \
    include/tia-private/datapacket/data_packet_selector.h \
    include/tia-private/network/control_connection.h \
    include/tia-private/network/control_connection_server.h \
    include/tia-private/network/tcp_data_server.h \
    include/tia-private/network/tcp_server.h \
    include/tia-private/network/udp_data_server.h \
    include/tia/tia_client.h \
    include/tia-private/client/tia_client_impl.h \
    include/tia-private/client/tia_client_impl_base.h \
    include/tia/ssconfig.h \
    include/tia-private/datapacket/raw_mem3.h \
    include/tia-private/clock.h \
    include/tia-private/newtia/network/socket.h \
    include/tia-private/newtia/server/data_server.h \
    include/tia-private/newtia/commands/get_data_connection_control_command.h \
    include/tia-private/newtia/messages/tia_control_message_parser.h \
    include/tia-private/newtia/messages/tia_control_message.h \
    include/tia-private/newtia/messages/tia_control_message_builder.h \
    include/tia-private/newtia/messages_impl/tia_control_message_parser_1_0.h \
    include/tia-private/newtia/messages/tia_control_message_tags_1_0.h \
    include/tia-private/newtia/tia_exceptions.h \
    include/tia-private/newtia/string_utils.h \
    include/tia-private/newtia/tia_control_command.h \
    include/tia-private/newtia/commands/check_protocol_version_control_command.h \
    include/tia-private/newtia/messages/standard_control_messages.h \
    include/tia-private/newtia/server_impl/fusty_data_server_impl.h \
    include/tia-private/newtia/messages_impl/tia_control_message_builder_1_0.h \
    include/tia-private/newtia/tia_control_command_context.h \
    include/tia-private/newtia/commands/start_data_transmission_control_command.h \
    include/tia-private/newtia/commands/stop_data_transmission_control_command.h \
    include/tia-private/newtia/commands/get_metainfo_control_command.h \
    include/tia-private/newtia/tia_meta_info_parse_and_build_functions.h \
    include/tia-private/newtia/hardware_interface.h \
    include/tia-private/newtia/fusty_hardware_interface_impl.h \
    include/tia-private/client/tia_new_client_impl.h \
    include/tia-private/newtia/network_impl/boost_udp_read_socket.h \
    include/tia-private/newtia/tia_datapacket_parser.h \
    include/tia-private/newtia/network/input_stream.h \
    include/tia-private/newtia/network/read_socket.h \
    include/tia-private/newtia/commands/get_serverstate_connection_command.h \
    include/tia-private/newtia/network/tcp_server_socket.h \
    include/tia-private/newtia/network/new_connection_listener.h \
    include/tia-private/newtia/network_impl/boost_tcp_server_socket_impl.h \
    include/tia-private/newtia/network/server_state_server.h \
    include/tia-private/newtia/server/server_states.h \
    include/tia-private/newtia/server/tia_server_state_server.h \
    include/tia-private/newtia/server_impl/tia_server_state_server_impl.h \
    include/tia-private/newtia/network_impl/boost_tcp_socket_impl.h \
    include/tia-private/newtia/server/control_connection_server_2.h \
    include/tia-private/newtia/server_impl/control_connection_server_2_impl.h \
    include/tia-private/newtia/server_impl/control_connection_2.h \
    include/tia-private/datapacket/data_packet_3_impl.h \
    include/tia/data_packet_interface.h \
    include/tia-private/datapacket/data_packet_impl.h \
    include/tia-private/newtia/tia_custom_signal_info_parse_and_build_functions.h \
    include/tia-private/newtia/tia_parse_and_build_helper_functions.h \
    include/tia-private/newtia/filter/tia_datapacket_custom_filter.h \
    include/tia-private/newtia/filter_impl/tia_customchannel_filterdecorator.h \
    include/tia-private/newtia/commands/set_custom_signalinfo_control_command.h \
    include/tia-private/newtia/filter_impl/tia_downsampling_filterdecorator.h \
    include/tia-private/newtia/filter_impl/downsampling_filterparam.h \
    include/tia/custom_signal_info.h

contains( DEFINES, TIMING_TEST )::HEADERS += extern/include/LptTools/LptTools.h

#---------------------------

SOURCES += src/tia/tia_server.cpp \
    src/tia/constants.cpp \
    src/tia/config/control_message_decoder.cpp \
    src/tia/config/control_message_encoder.cpp \
    src/tia/config/control_messages.cpp \
    src/tia/ss_meta_info.cpp \
    src/tia/datapacket/raw_mem.cpp \
    src/tia/datapacket/data_packet_selector.cpp \
    src/tia/network/control_connection.cpp \
    src/tia/network/control_connection_server.cpp \
    src/tia/network/tcp_data_server.cpp \
    src/tia/network/tcp_server.cpp \
    src/tia/network/udp_data_server.cpp \
    src/tia/client/tia_client_impl.cpp \
    src/tia/tia_client.cpp \
    src/tia/datapacket/raw_mem3.cpp \
    src/tia/clock.cpp \
    src/tia/newtia/commands/get_data_connection_control_command.cpp \
    src/tia/newtia/messages_impl/tia_control_message_parser_1_0.cpp \
    src/tia/newtia/network_impl/boost_tcp_socket_impl.cpp \
    src/tia/newtia/server_impl/fusty_data_server_impl.cpp \
    src/tia/newtia/commands/start_data_transmission_control_command.cpp \
    src/tia/newtia/commands/stop_data_transmission_control_command.cpp \
    src/tia/newtia/tia_meta_info_parse_and_build_functions.cpp \
    src/tia/client/tia_new_client_impl.cpp \
    src/tia/newtia/network_impl/boost_udp_read_socket.cpp \
    src/tia/newtia/tia_datapacket_parser.cpp \
    src/tia/newtia/network_impl/boost_tcp_server_socket_impl.cpp \
    src/tia/newtia/server_impl/tia_server_state_server_impl.cpp \
    src/tia/newtia/server_impl/control_connection_server_2_impl.cpp \
    src/tia/newtia/server_impl/control_connection_2.cpp \
    src/tia/datapacket/data_packet_impl.cpp \
    src/tia/datapacket/data_packet_3_impl.cpp \
    src/tia/newtia/tia_custom_signal_info_parse_and_build_functions.cpp \
    src/tia/newtia/tia_parse_and_build_helper_functions.cpp \
    src/tia/newtia/filter_impl/tia_customchannel_filterdecorator.cpp \
    src/tia/newtia/commands/set_custom_signalinfo_control_command.cpp \
    src/tia/newtia/filter_impl/tia_downsampling_filterdecorator.cpp

unix {
  contains( DEFINES, TIMING_TEST )::SOURCES += extern/include/LptTools/LptToolsLinux.cpp
}

win32 {
  contains( DEFINES, TIMING_TEST )::SOURCES += extern/include/LptTools/LptTools_.cpp
}


