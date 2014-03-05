#include <iostream>

#include <gloox/jid.h>
#include <gloox/client.h>
#include <gloox/messagehandler.h>
#include <gloox/message.h>
#include <gloox/messagesession.h>
#include <gloox/messagesessionhandler.h>

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



    JID ui_jid("i2c@platon");
    Client* client = new Client(ui_jid, "i2c");

    I2CController* i2c = new I2CController(client);

    if (!client->connect(true))
        cerr << "could not connect!" << endl;




    delete client;


    return 0;
}
