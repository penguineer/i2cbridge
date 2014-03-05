#include <iostream>

#include <gloox/jid.h>
#include <gloox/client.h>
#include <gloox/messagehandler.h>
#include <gloox/message.h>
#include <gloox/messagesession.h>

using namespace std;
using namespace gloox;

class EchoMessageHandler : public MessageHandler {
public:
    Client* client;

    virtual void handleMessage(const Message& msg,
                               MessageSession* session = 0) {
        cout << "Received message " << msg.body() << endl;

        if (session) {
            session->send("received");
        } else {
            cout << "No message session." << endl;

            client->send(Message(Message::Chat, msg.from(), string("Hallo Welt!")));
            cout << client->streamErrorText() << endl;
        }
        client->disconnect();
    }
};

int main(int argc, char **argv) {

    JID ui_jid("tux@platon");

    Client* client = new Client(ui_jid, "penguin");
    EchoMessageHandler* emh = new EchoMessageHandler();
    emh->client = client;
    client->registerMessageHandler(emh);
    client->connect(true);

    cout << "Hello, world!" << endl;


    delete client;
    delete emh;

    return 0;
}
