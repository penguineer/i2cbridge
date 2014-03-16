#include <iostream>
#include <string>
#include <sstream>


#include <gloox/client.h>


#include "configuredclientfactory.h"
#include "spacecontrolclient.h"

using namespace std;
using namespace gloox;
using namespace xmpppi;

class I2CHandler : public SpaceControlHandler {
public:
    virtual void handleSpaceCommand(SpaceCommand sc, SpaceCommand* &response);
};

void I2CHandler::handleSpaceCommand(SpaceCommand sc, SpaceCommand* &response) {
  cout << "Got command " << sc.cmd() << " from " << sc.peer().full() << endl;
  
  response = new SpaceCommand(sc.peer(), "Hallo Welt!");
}



int main(int argc, char **argv) {
    cout << "Hello, world!" << endl;

    Client* client=0;
    try {
        xmpppi::ConfiguredClientFactory ccf("i2ccontrol.config");
        client = ccf.newClient();
    } catch (xmpppi::ConfiguredClientFactoryException &ccfe) {
        cerr << "ConfiguredClientFactoryException: " << ccfe.what() << endl;
        return (-1);
    }


    if (client) {
        xmpppi::SpaceControlClient* scc = new xmpppi::SpaceControlClient(client, new I2CHandler());

        if (!client->connect(true))
            cerr << "could not connect!" << endl;

        delete client;
    }

    return 0;
}
