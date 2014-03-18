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

//TODO comments
//TODO fix the newline specification

#include "spacecontrolclient.h"

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>


using namespace xmppsc;

SpaceCommandFormatException::SpaceCommandFormatException(std::string _what,
        std::string _body, int _line_number=0)
    : m_what(_what), m_body(_body), m_line_number(_line_number) {}

SpaceCommandFormatException::~SpaceCommandFormatException() throw() {}

const char* SpaceCommandFormatException::what() const throw() {
    return m_what.c_str();
}

const char* SpaceCommandFormatException::body() const throw() {
    return m_body.c_str();
}

int SpaceCommandFormatException::line_number() const throw() {
    return m_line_number;
}

SpaceCommand::SpaceCommand(const std::string _cmd,
                           std::map<const std::string, std::string> _params)
    : m_cmd(_cmd), m_params(_params) {}

std::string SpaceCommand::cmd() {
    return this->m_cmd;
}

std::string SpaceCommand::param(const std::string key) throw(std::out_of_range) {
    return m_params.at(key);
}

std::string SpaceCommand::as_body() {
    // build parameter list
    std::stringstream params("");

    space_command_params::iterator iter;

    for (iter = m_params.begin(); iter != m_params.end(); iter++) {
        // count lines in parameter value
        int paramlines = 0;

        if (iter->second.empty())
            paramlines = 0;
        else {
            paramlines = 1;
            const char* val = iter->second.c_str();
            int i;
            for (i = iter->second.size(); i; i--)
                if (val[i] == '\n')
                    paramlines++;
        }

        if (!params.str().empty())
            params << std::endl;
        params << paramlines << " " << iter->first << std::endl;
        params << iter->second;
    }


    // build message body
    std::stringstream body;
    body << this->cmd();
    if (!params.str().empty())
        body << std::endl << params.str();

    return body.str();
}

// local helper class
class Sink : public SpaceCommandSink {
public:
    Sink(gloox::MessageSession* _session, bool _shared=false) : m_session(_session), m_shared(_shared) {}
    virtual ~Sink();

    virtual void sendSpaceCommand(SpaceCommand* sc);

private:
    gloox::MessageSession* m_session;
    bool m_shared;
};

Sink::~Sink() {
    if (!m_shared && m_session)
        delete m_session;
}

void Sink::sendSpaceCommand(SpaceCommand* sc) {
    if (m_session && sc) {
        m_session->send(sc->as_body());
    }
    //TODO else
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
    if (session)
        try {
            // create the command
            // may throw a SpaceCommandFormatException
            const SpaceCommand cmd = parseMessage(msg.body());

            // create shared sink
            Sink sink(session, true);

            // call handler
            if (m_hnd)
                m_hnd->handleSpaceCommand(session->target(), cmd, &sink);

        } catch (SpaceCommandFormatException& scfe) {
            SpaceCommand::space_command_params par;
            par["what"] = scfe.what();
            par["body"] = scfe.body();
            // add line number if available
            if (scfe.line_number()) {
                // it would be nicer to use std::to_string here, when available
                std::stringstream s;
                s << scfe.line_number();
                par["line number"] = s.str();
            }
            SpaceCommand ex("exception", par);
            session->send(ex.as_body());
        }
    //TODO else warn
}

gloox::Client* SpaceControlClient::client() {
    return this->m_client;
}

SpaceControlHandler* SpaceControlClient::handler() {
    return this->m_hnd;
}

SpaceCommandSink* SpaceControlClient::create_sink(gloox::JID peer) {
    gloox::MessageSession* session = new gloox::MessageSession(this->m_client, peer);

    // return sink that is not shared
    return new Sink(session, false);
}



SpaceCommand SpaceControlClient::parseMessage(std::string body)
throw(SpaceCommandFormatException) {
    // the command
    std::string command;
    // the parameter map
    SpaceCommand::space_command_params params;

    //parse command and params from msg body
    int line_number = 0;
    std::stringstream ss(body);
    std::string line;
    while (std::getline(ss, line, '\n')) {
        line_number++;

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
            if (idx == std::string::npos)
                throw SpaceCommandFormatException("Missing space character in parameter key line.", body, line_number);

            // get the number of parameter lines
            const std::string s_parlines = line.substr(0, idx);
            int parlines = strtol(s_parlines.c_str(), 0, 10);
            if (parlines == 0)
                throw SpaceCommandFormatException("Invalid integer for parameter line count.", body, line_number);

            // get the parameter key
            const std::string key = line.substr(idx+1);

            // get the parameter lines
            std::string value;
            while (parlines--) {
                std::string parline;
                if (!std::getline(ss, parline, '\n'))
                    throw SpaceCommandFormatException("There are less lines that stated in the parameter line count!", body, line_number);
                line_number++;

                if (!value.empty())
                    value.append("\n");
                value.append(parline);
            }

            // add to parameter map
            params[key] = value;
        }
    }

    // create the command
    return SpaceCommand(command, params);
}
