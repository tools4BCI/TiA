#ifndef SERVER_CONTROL_CONNECTION_H
#define SERVER_CONTROL_CONNECTION_H

#include "socket.h"
#include "tia_control_command.h"
#include "tia_control_message.h"
#include "tia_control_message_builder.h"
#include "tia_control_message_parser.h"
#include "data_server.h"

#include <map>
#include <memory>

namespace tia
{

//-----------------------------------------------------------------------------
class ServerControlConnection
{
public:
    ServerControlConnection (Socket& socket, DataServer& data_server);
    ~ServerControlConnection ();

    void start ();
    void stop ();

private:
    bool running_;
    Socket& socket_;
    DataServer& data_server_;
    typedef std::map<std::string, TiAControlCommand*> CommandMap;
    CommandMap command_map_;
    std::auto_ptr<TiAControlMessageParser> parser_;
    std::auto_ptr<TiAControlMessageBuilder> builder_;
};

}

#endif // CONTROL_CONNECTION_SERVER_2_H
