#include <iostream>
#include <string>
#include <sstream>


#include <gloox/client.h>


#include "configuredclientfactory.h"
#include "spacecontrolclient.h"

using namespace std;
using namespace gloox;
using namespace xmppsc;

class I2CHandler : public SpaceControlHandler {
public:
    virtual void handleSpaceCommand(SpaceCommand sc, SpaceCommand* &response);
};

void I2CHandler::handleSpaceCommand(SpaceCommand sc, SpaceCommand* &response) {
    cout << "Got command " << sc.cmd() << " from " << sc.peer().full() << endl;
    try {
        cout << "Param id: " << sc.param("id") << endl;
    } catch (std::out_of_range &oor) {
        cerr << "Parameter id is not available!" << endl;
    }
    response = new SpaceCommand(sc.peer(), "Hallo Welt!", SpaceCommand::space_command_params());
}



int main(int argc, char **argv) {
    cout << "Hello, world!" << endl;

    Client* client=0;
    try {
        xmppsc::ConfiguredClientFactory ccf("i2ccontrol.config");
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
