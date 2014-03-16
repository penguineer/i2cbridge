//TODO copyright notice
//TODO comments

#include "spacecontrolclient.h"

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>


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
    // the command
    std::string command;
    // the parameter map
    space_command_params params;

    //parse command and params from msg body
    std::stringstream ss(msg.body());
    std::string line;
    while (std::getline(ss, line, '\n')) {
        // first line is command
        if (command.empty())
            command = line;
        else {
            /* Note: std::stoi is not available yet :(
             *
             *            // split the parameter line
             *            const size_t idx = 0;
             *            // first is the number of lines containing the value
             *            const int parlines = std::stoi(line, &idx);
             *            // after the space is the parameter name
             *            const std::string key = line.substr(idx+1);
             */

            // find the space character
            const size_t idx = line.find(' ');
            if (idx == std::string::npos) {
                //TODO throw exception
            }

            // get the number of parameter lines
            const std::string s_parlines = line.substr(0, idx);
            int parlines = strtol(s_parlines.c_str(), 0, 10);
            if (parlines == 0) {
                //TODO throw exception
            }

            // get the parameter key
            const std::string key = line.substr(idx+1);

            // get the parameter lines
            std::string value;
            while (parlines--) {
                std::string parline;
                if (!std::getline(ss, parline, '\n')) {
                    //TODO throw exception
                }
                if (!value.empty())
                    value.append("\n");
                value.append(parline);
            }
            
            // add to parameter map
            params[key] = value;
        }
    }

    // create the command
    const SpaceCommand cmd(session->target(), command, params);

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
