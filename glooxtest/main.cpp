#include <iostream>
#include <string>


#include <gloox/jid.h>
#include <gloox/client.h>
#include <gloox/messagehandler.h>
#include <gloox/message.h>
#include <gloox/messagesession.h>
#include <gloox/messagesessionhandler.h>

#include <libconfig.h++>

using namespace std;
using namespace gloox;


class I2CController  : public MessageSessionHandler, MessageHandler {
private:
    Client* m_client;
public:
    I2CController(Client* _client);

    virtual void handleMessageSession(MessageSession* session);

    virtual void handleMessage(const Message& msg, MessageSession* session = 0);

    Client* client();
};

I2CController::I2CController(Client* _client) : m_client(_client) {
    if (_client)
        _client->registerMessageSessionHandler(this);
}

void I2CController::handleMessageSession(MessageSession* session) {
    cout << "Message session from " << session->target().full() << endl;
    session->registerMessageHandler(this);
}

void I2CController::handleMessage(const Message& msg, MessageSession* session) {
    cout << "Message: " << msg.body() << endl;
    if (session) {
        cout << "With session. :) " << endl;
        session->send("Hallo Welt!");
    }
}


Client* I2CController::client() {
    return this->m_client;
}

int main(int argc, char **argv) {
    cout << "Hello, world!" << endl;

    libconfig::Config cfg;

    try {
        cfg.readFile("i2ccontrol.config");
    } catch (const libconfig::FileIOException &fioex) {
        cerr << "I/O error on reading config file: " << fioex.what() << endl;
        return (-1);
    } catch(const libconfig::ParseException &pex) {
        cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
             << " - " << pex.getError() << endl;
        return(-1);
    }

    Client* client = 0;
    try {
        string jid = cfg.lookup("xmpp.jid");
        string password = cfg.lookup("xmpp.password");

        cout << "Got jid " << jid << endl;

        JID ui_jid(jid);
        client = new Client(ui_jid, password);
    } catch (const libconfig::SettingNotFoundException &snfex) {
        cerr << "Setting not found: " << snfex.getPath() << endl;
    }


    if (client) {
        I2CController* i2c = new I2CController(client);

        if (!client->connect(true))
            cerr << "could not connect!" << endl;




        delete client;
    }

    return 0;
}
