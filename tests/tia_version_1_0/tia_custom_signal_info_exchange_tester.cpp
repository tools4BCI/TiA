/*
    This file is part of the TOBI SignalServer test routine.

    The TOBI SignalServer test routine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The TOBI SignalServer test routine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the TOBI SignalServer test routine. If not, see <http://www.gnu.org/licenses/>.

    Copyright 2012 Reinmar Kobler
    Contact: reinmar.kobler@student.tugraz.at
*/

#include "../UnitTest++/UnitTest++.h"

#include "tia-private/client/tia_new_client_impl.h"
#include "tia/ssconfig.h"

#include <iostream>
#include <stdexcept>

using namespace tia;
using namespace std;


SUITE (TiAVersion10)
{
    TEST(TiaCustomSignalInfoExchangeTest)
    {
        TiANewClientImpl client;

        try{

        client.connect("127.0.0.1",9000);

        }catch(exception ex)
        {
            cout << "Was not able to connect to server; reason:" << ex.what() << endl;
        }

        if(client.connected())
        {
            client.requestConfig();

            SSConfig meta_info = client.config();

            meta_info.signal_info.signals().clear();

            std::string error_container;

            client.trySetCustomSignalInfo(meta_info.signal_info,error_container);

            std::cout << "Error containter content: "<< error_container << std::endl;

            client.disconnect();
        }

    }
}
