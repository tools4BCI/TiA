function compile(reset_setting, use_lib_file)
  c_files = ' ';
  c_files = [c_files ' ../../../src/tia/datapacket/data_packet_impl.cpp ../../../src/tia/datapacket/raw_mem.cpp'];
  c_files = [c_files ' ../../../src/tia/datapacket/data_packet_3_impl.cpp ../../../src/tia/datapacket/raw_mem3.cpp ../../../src/tia/clock.cpp'];
  c_files = [c_files ' ../../../src/tia/constants.cpp ../../../src/tia/config/control_message_decoder.cpp'];
  c_files = [c_files ' ../../../src/tia/config/control_messages.cpp ../../../src/tia/config/control_message_encoder.cpp'];
  c_files = [c_files ' ../../../src/tia/tia_client.cpp ../../../src/tia/client/tia_client_impl.cpp'];
  c_files = [c_files ' ../../../src/tia/client/tia_new_client_impl.cpp'];
  c_files = [c_files ' ../../../src/tia/network/tcp_server.cpp'];
  c_files = [c_files ' ../../../src/tia/newtia/network_impl/boost_tcp_socket_impl.cpp ../../../src/tia/newtia/network_impl/boost_udp_read_socket.cpp'];
  c_files = [c_files ' ../../../src/tia/newtia/tia_datapacket_parser.cpp ../../../src/tia/newtia/tia_meta_info_parse_and_build_functions.cpp'];
  c_files = [c_files ' ../../../src/tia/newtia/messages_impl/tia_control_message_parser_1_0.cpp'];


  unix_build_command = 'mex -v -O -DTIXML_USE_TICPP  -I../../../include/  -I../../../extern/include/  -lboost_system -lstdc++ -outdir build';
  if(~isunix)

    if(~exist('libtia_matlab_client_win_settings.mat','file') || reset_setting)

      win_boost_path = 'C:\Programme\boost\boost_1_48_0';

      if(~exist(win_boost_path,'dir'))
          disp('Boost not found!');
          win_boost_path = uigetdir;
      end
      win_boost_path = ['''' win_boost_path ''''];

      prompt = {'Boost version:','Compiler:'};
      dlg_title = 'Boost library specifications';
      num_lines = 1;
      def = {'1_48','vc100'};
      answer = inputdlg(prompt,dlg_title,num_lines,def);

    else
      load('libtia_matlab_client_win_settings.mat');
    end

    win_build_command  = ['mex -v -O -DTIXML_USE_TICPP -DWIN32 -D_WIN32_WINNT=0x0501 -I' win_boost_path ' '];
    win_build_command  = [ win_build_command ' -I../../../include/  -I../../../extern/include/ -L' win_boost_path '\lib -outdir build'];
    win_libs = [' -llibboost_system-'    answer{2} '-mt-' answer{1} ...
                ' -llibboost_date_time-' answer{2} '-mt-' answer{1} ...
                ' -llibboost_regex-'     answer{2} '-mt-' answer{1} ...
                ' -llibboost_thread-'    answer{2} '-mt-' answer{1} ];

    save('libtia_matlab_client_win_settings.mat', 'answer', 'win_boost_path');
    clear answer win_boost_path;
  end

  ticpp_files = ' ../../../extern/include/ticpp/tinyxmlparser.cpp ../../../extern/include/ticpp/tinyxmlerror.cpp';
  ticpp_files = [ticpp_files ' ../../../extern/include/ticpp/tinystr.cpp ../../../extern/include/ticpp/tinyxml.cpp ../../../extern/include/ticpp/ticpp.cpp'];

  if(~exist('build','dir'))
    mkdir('build');
  end



  if(~use_lib_file)

    if(isunix)
      if(strcmp(computer, 'GLNX86'))
        lib_files = [' ../../../extern/lib/ticpp/linux/libticpp.a '];
      else
        lib_files = [' ../../../extern/lib/ticpp/linux/libticpp_64.a '];
      end

      eval([unix_build_command ' src/TiA_matlab_get_config.cpp ' c_files ticpp_files lib_files]);
      eval([unix_build_command ' src/TiA_simulink_get_data.cpp ' c_files ticpp_files lib_files]);
      eval([unix_build_command ' src/TiA_matlab_client.cpp '     c_files ticpp_files lib_files]);

    else
      if(strcmp(computer, 'PCWIN'))
        lib_files = [' ../../../extern/lib/ticpp/win/Win32/libticpp.lib  '];
      else
        lib_files = [' ../../../extern/lib/ticpp/win/x64/libticpp.lib  '];
      end

      eval([win_build_command ' src/TiA_matlab_get_config.cpp ' c_files ticpp_files lib_files win_libs]);
      eval([win_build_command ' src/TiA_simulink_get_data.cpp ' c_files ticpp_files lib_files win_libs]);
      eval([win_build_command ' src/TiA_matlab_client.cpp '     c_files ticpp_files lib_files win_libs]);
    end

  else

    if(isunix)
      if(strcmp(computer, 'GLNX86'))
        lib_files = [' ../../../lib/x86/libtia.a    ../../../extern/lib/ticpp/linux/libticpp.a '];
      else
        lib_files = [' ../../../lib/amd64/libtia.a  ../../../extern/lib/ticpp/linux/libticpp_64.a  '];
      end

      eval([unix_build_command  ' src/TiA_matlab_get_config.cpp ' lib_files]);
      eval([unix_build_command  ' src/TiA_simulink_get_data.cpp ' lib_files]);
      eval([unix_build_command  ' src/TiA_matlab_client.cpp '     lib_files]);

    else
      if(strcmp(computer, 'PCWIN'))
        lib_files = [' ../../../lib/Win32/tia.lib ../../../extern/lib/ticpp/win/Win32/libticpp.lib  '];
      else
        lib_files = [' ../../../lib/x64/tia.lib    ../../../extern/lib/ticpp/win/x64/libticpp.lib  '];
      end

      eval([win_build_command  ' src/TiA_matlab_get_config.cpp ' lib_files win_libs]);
      eval([win_build_command  ' src/TiA_simulink_get_data.cpp ' lib_files win_libs]);
      eval([win_build_command  ' src/TiA_matlab_client.cpp '     lib_files win_libs]);
    end

  end

end