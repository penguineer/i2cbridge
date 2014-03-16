//TODO copyright notice
//TODO comments

#include "spacecontrolclient.h"

#include <iostream>

using namespace xmpppi;

SpaceCommand::SpaceCommand(gloox::JID _peer, const std::string _cmd,
                           std::map<const std::string, std::string> _params)
    : m_peer(_peer), m_cmd(_cmd), m_params(_params) {}

std::string SpaceCommand::cmd() {
    return this->m_cmd;
}

gloox::JID SpaceCommand::peer() {
    return this->m_peer;
}

std::string SpaceCommand::param(const std::string key) throw(std::out_of_range) {
    return m_params.at(key);
}


SpaceControlClient::SpaceControlClient(gloox::Client* _client, SpaceControlHandler* _hnd)
    : m_client(_client), m_hnd(_hnd) {
    if (_client)
        _client->registerMessageSessionHandler(this);
}

void SpaceControlClient::handleMessageSession(gloox::MessageSession* session) {
    std::cout << "Message session from " << session->target().full() << std::endl;
    session->registerMessageHandler(this);
}

void SpaceControlClient::handleMessage(const gloox::Message& msg, gloox::MessageSession* session) {
    // get the parameter map
    space_command_params params;
    //params["id"] = "id1";
    //TODO parse params from msg body
    
    // create the command
    SpaceCommand cmd(session->target(), msg.body(), params);

    SpaceCommand* response =  0;
    // call handler
    if (m_hnd) {
        m_hnd->handleSpaceCommand(cmd, response);
    }

    if (response && session) {
        session->send(response->cmd());

        delete response;
    }
}

gloox::Client* SpaceControlClient::client() {
    return this->m_client;
}

SpaceControlHandler* SpaceControlClient::handler() {
    return this->m_hnd;
}
