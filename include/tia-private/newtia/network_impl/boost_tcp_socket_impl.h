/*
    This file is part of the TOBI Interface A (TiA) library.

    Commercial Usage
    Licensees holding valid Graz University of Technology Commercial
    licenses may use this file in accordance with the Graz University
    of Technology Commercial License Agreement provided with the
    Software or, alternatively, in accordance with the terms contained in
    a written agreement between you and Graz University of Technology.

    --------------------------------------------------

    GNU Lesser General Public License Usage
    Alternatively, this file may be used under the terms of the GNU Lesser
    General Public License version 3.0 as published by the Free Software
    Foundation and appearing in the file lgpl.txt included in the
    packaging of this file.  Please review the following information to
    ensure the GNU General Public License version 3.0 requirements will be
    met: http://www.gnu.org/copyleft/lgpl.html.

    In case of GNU Lesser General Public License Usage ,the TiA library
    is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the TiA library. If not, see <http://www.gnu.org/licenses/>.

    Copyright 2010 Graz University of Technology
    Contact: TiA@tobi-project.org
*/

#ifndef BOOST_TCP_SOCKET_IMPL_H
#define BOOST_TCP_SOCKET_IMPL_H

#include "tia-private/newtia/network/socket.h"

#include <boost/asio.hpp>

//namespace tobiss
//{
//class TCPConnection;
//}

namespace tia
{

//-----------------------------------------------------------------------------
class BoostTCPSocketImpl : public Socket
{
  public:
    BoostTCPSocketImpl (boost::asio::io_service& io_service,
                        boost::asio::ip::tcp::endpoint const& endpoint, unsigned buffer_size);


//    BoostTCPSocketImpl (boost::shared_ptr<tobiss::TCPConnection> con);

    BoostTCPSocketImpl (boost::shared_ptr<boost::asio::ip::tcp::socket> boost_socket);

    virtual ~BoostTCPSocketImpl ();

    virtual void setReceiveBufferSize (unsigned size);
    virtual std::string readLine (unsigned max_length);
    virtual std::string readString (unsigned length);
    virtual char readCharacter ();
    virtual void waitForData ();
    virtual void sendString (std::string const& str) throw (TiALostConnection);

    virtual std::string getRemoteEndPointAsString();
    virtual std::string getLocalEndPointAsString();

  private:
    void readBytes (unsigned num_bytes);
//    boost::shared_ptr<tobiss::TCPConnection> fusty_connection_;
    boost::shared_ptr<boost::asio::ip::tcp::socket> socket_;
    std::string buffered_string_;

    std::string remote_endpoint_str_;
    std::string local_endpoint_str_;
};

}

#endif // BOOST_TCP_SOCKET_IMPL_H
