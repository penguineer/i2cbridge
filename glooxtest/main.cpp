#include <iostream>
#include <string>
#include <sstream>

/*
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <gloox/client.h>

#include <iostream>

#include "configuredclientfactory.h"
#include "spacecontrolclient.h"

using namespace std;
using namespace gloox;
using namespace xmppsc;

class I2CHandler : public SpaceControlHandler {
public:
    virtual void handleSpaceCommand(SpaceCommand sc, SpaceCommandSink* sink);
};

void I2CHandler::handleSpaceCommand(SpaceCommand sc, SpaceCommandSink* sink) {
    cout << "Got command " << sc.cmd() << " from " << sc.peer().full() << endl;

    xmppsc::SpaceCommand response(sc.peer(), "Hallo Welt!",
                                  xmppsc::SpaceCommand::space_command_params());

    sink->sendSpaceCommand(&response);

    try {
        xmppsc::SpaceCommand::space_command_params params;
        params["id"] = sc.param("id");
        xmppsc::SpaceCommand idcmd(sc.peer(), "id", params);
        sink->sendSpaceCommand(&idcmd);
    } catch (std::out_of_range &oor) {
        cerr << "Parameter id is not available!" << endl;
    }

}



int main(int argc, char **argv) {
    Client* client=0;
    try {
        xmppsc::ConfiguredClientFactory ccf("spacecontrol.config");
        client = ccf.newClient();
    } catch (xmppsc::ConfiguredClientFactoryException &ccfe) {
        cerr << "ConfiguredClientFactoryException: " << ccfe.what() << endl;
        return (-1);
    }


    if (client) {
        xmppsc::SpaceControlClient* scc = new xmppsc::SpaceControlClient(client, new I2CHandler());

        if (!client->connect(true))
            cerr << "could not connect!" << endl;

        delete client;
    }

    return 0;
}
