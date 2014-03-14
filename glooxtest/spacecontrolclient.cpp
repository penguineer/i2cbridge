//TODO copyright notice
//TODO comments

#include "spacecontrolclient.h"

#include <iostream>

using namespace xmpppi;

SpaceControlClient::SpaceControlClient(gloox::Client* _client) : m_client(_client) {
    if (_client)
        _client->registerMessageSessionHandler(this);
}

void SpaceControlClient::handleMessageSession(gloox::MessageSession* session) {
    std::cout << "Message session from " << session->target().full() << std::endl;
    session->registerMessageHandler(this);
}

void SpaceControlClient::handleMessage(const gloox::Message& msg, gloox::MessageSession* session) {
    std::cout << "Message: " << msg.body() << std::endl;
    if (session) {
        std::cout << "With session. :) " << std::endl;
        session->send("Hallo Welt!");
    }
}


gloox::Client* SpaceControlClient::client() {
    return this->m_client;
}