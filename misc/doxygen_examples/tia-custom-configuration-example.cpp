
/**
* @file tia-custom-configuration-example.cpp
*
* @brief The file briefly demonstartes how to use custom configuration on client side.
**/

//stl
#include <iostream>
#include <exception>

//tia
#include "tia/tia_client.h"
#include "tia/custom_signal_info.h"

//tia private just for getting serialized form for printing to cout and cerr
#include "tia-private/newtia/tia_meta_info_parse_and_build_functions.h"
#include "tia-private/newtia/tia_custom_signal_info_parse_and_build_functions.h"

using namespace tia;

int main()
{
    //---------------------------------------------
    //------ establish connection to server -------
    //---------------------------------------------

    TiAClient client (true);

    const char *server_ip = "127.0.0.1";
    const uint32_t server_port = 9000;

    try
    {
        client.connect(server_ip,server_port);
    }
    catch(std::exception &ex)
    {
        std::cerr << "ERROR: Unable to connect server @ " << server_ip << ":"
                  << server_port << std::endl << " -- " << ex.what() << std::endl;
        return 1;
    }

    if(client.connected())
    {
        std::cout << "Estabalished connection to server @ "
                  << server_ip << ":" << server_port << std::endl;

        //------------------------------------
        //------ request configuration -------
        //------------------------------------

        try
        {
          client.requestConfig();

          std::cout << "Requested default configuration" << std::endl;
        }
        catch(std::exception& ex)
        {
          std::cerr << "ERROR: Unable to request config -- "
                    << ex.what() << std::endl;
          return 1;
        }

        SSConfig default_config = client.config();

        std::cout << "Received default configuration: "
                  << std::endl << std::endl <<  buildTiAMetaInfoXMLString(default_config);

        if(default_config.signal_info.signals().size() > 1)
        {
            //-------------------------------------
            //------ perform customisations -------
            //-------------------------------------


            CustomSignalInfoPtr custom_info = client.getConfigAsCustomConfig();

            std::cout << "Starting customising the configuration.." << std::endl;

            // check if channel reduction is possible
            if(custom_info->signals().begin()->second.channels().size() > 1)
            {
                CustomSignal &sig_to_mod = custom_info->signals().begin()->second;

                std::cout << "Deselecting channel '" << sig_to_mod.channels().begin()->id()
                          << "' of signal type '" << sig_to_mod.type() << "'" << std::endl;

                sig_to_mod.channels().erase(sig_to_mod.channels().begin());
            }
            else // if no single channel can be removed, remove the whole signal type
            {
                std::cout << "Deselecting signal type '"
                          << custom_info->signals().begin()->first << "'" << std::endl;

                custom_info->signals().erase(custom_info->signals().begin());
            }

            CustomSignal &sig_to_decimate = custom_info->signals().begin()->second;

            std::cout << "Reducing the sampling rate of signal type '"
                      << sig_to_decimate.type() << "' by factor 2" << std::endl;


            sig_to_decimate.setDSFactor((boost::uint16_t) 2u);

            std::cout << "Custom configuration after customisations:" << std::endl
                      << std::endl << buildTiACustomSignalInfoXMLString(custom_info);

            //--------------------------------------------
            //------ send customisations to server -------
            //--------------------------------------------

            try
            {
                std::string error_container;

                std::cout << "Sending custom configuration to server.." << std::endl;

                bool accepted = client.trySetCustomSignalInfo(custom_info, error_container);

                if(!accepted)
                    std::cerr << "Server returned validation error! -- '"
                              << error_container << "'" << std::endl;
                else
                    std::cout << "Server returned NO error!" << std::endl;

            }catch (std::exception ex)
            {
                std::cerr << "ERROR: Exception during trying to set custom config -- "
                          << ex.what() << std::endl;
                return 1;
            }

            //------------------------------------------------------
            //------ request configuraiton from server again -------
            //------------------------------------------------------

            try
            {
              client.requestConfig();

              std::cout << "Requested default configuration again" << std::endl;
            }
            catch(std::exception& ex)
            {
              std::cerr << "ERROR: Unable to request config again -- "
                        << ex.what() << std::endl;
              return 1;
            }

            SSConfig new_default_config = client.config();

            std::cout << "Received new configuration: " << std::endl
                      << std::endl <<  buildTiAMetaInfoXMLString(new_default_config);

        }
        else
        {
            std::cerr << "ERROR: This example needs at least two signale types!"
                      << std::endl;
            return 1;
        }

        //-------------------------------------
        //------ disconnect from server -------
        //-------------------------------------

        client.disconnect();
        std::cout << "Closed connection to server @ "
                  << server_ip << ":" << server_port << std::endl;
    }
    else
    {
        std::cerr << "ERROR: Not connected to server!"
                  << std::endl;
        return 1;
    }

    return 0;
}
