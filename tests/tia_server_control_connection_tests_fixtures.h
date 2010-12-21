#ifndef TIA_SERVER_CONTROL_CONNECTION_TESTS_FIXTURES_H
#define TIA_SERVER_CONTROL_CONNECTION_TESTS_FIXTURES_H

#include "tia-private/server/socket.h"
#include "tia-private/server/data_server.h"

#include "UnitTest++/UnitTest++.h"

#include <map>

//-----------------------------------------------------------------------------
/// TestSocket
///
/// test implementation of the Socket class
class TestSocket : public tia::Socket
{
public:
    virtual void sendString (std::string const& string) {sent_string_ = string;}
    virtual std::string readString () {return string_to_return_on_read_;}

    std::string transmittedString () {return sent_string_;}
    void setStringToBeRead (std::string const& read_string) {string_to_return_on_read_ = read_string;}

private:
    std::string sent_string_;
    std::string string_to_return_on_read_;
};


//-----------------------------------------------------------------------------
/// TestSocket
///
/// test implementation of the Socket class
class TestDataServer : public tia::DataServer
{
public:
    TestDataServer () : next_free_id_ (0), next_free_port_ (1234) {}

    unsigned connectionCount () const {return connection_transmission_map_.size ();}

    tia::ConnectionID mostRecentConnectionID () const {return most_recent_connection_id_;}

    bool transmissionEnabled (tia::ConnectionID connection) const {return connection_transmission_map_.at (connection);}

    virtual tia::Port localPort (tia::ConnectionID connection) const {return connection_port_map_.at (connection);}

    virtual tia::ConnectionID addConnection ()
    {
        most_recent_connection_id_ = next_free_id_;
        connection_transmission_map_.insert (std::pair<tia::ConnectionID, bool> (most_recent_connection_id_, false));
        connection_port_map_.insert (std::pair<tia::ConnectionID, tia::Port> (most_recent_connection_id_, next_free_port_));
        next_free_id_++;
        next_free_port_++;
        return most_recent_connection_id_;
    }

    virtual bool removeConnection (tia::ConnectionID connection)
    {
        connection_transmission_map_.erase (connection);
        connection_port_map_.erase (connection);
        return true;
    }

    virtual void startTransmission (tia::ConnectionID connection) {if (connection_transmission_map_.count (connection)) connection_transmission_map_[connection] = true;}

    virtual void stopTransmission (tia::ConnectionID connection) {if (connection_transmission_map_.count (connection)) connection_transmission_map_[connection] = false;}

private:
    tia::ConnectionID next_free_id_;
    tia::Port next_free_port_;
    std::map<tia::ConnectionID, bool> connection_transmission_map_;
    std::map<tia::ConnectionID, tia::Port> connection_port_map_;
    tia::ConnectionID most_recent_connection_id_;
};

//-----------------------------------------------------------------------------
///
struct TiAServerControlConnectionFixture
{
    TestDataServer test_data_server;
    TestSocket test_control_socket;
};


#endif // TIA_SERVER_CONTROL_CONNECTION_TESTS_FIXTURES_H