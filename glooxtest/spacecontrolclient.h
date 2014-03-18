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

//TODO extrn?
//TODO distinct class for message conversion

#ifndef SPACECONTROLCLIENT_H__
#define SPACECONTROLCLIENT_H__

#include <string>
#include <map>
#include <stdexcept>

#include <gloox/jid.h>
#include <gloox/client.h>
#include <gloox/message.h>
#include <gloox/messagesession.h>
#include <gloox/messagehandler.h>
#include <gloox/messagesessionhandler.h>


namespace xmppsc {


//! Exception on parsing errors in space command messages.
/*!
 * This exception is thrown when parsing a message body for SpaceCommandS
 * fails. A description of the failure and the message body are provided.
 */
class SpaceCommandFormatException : public std::exception {
public:
    //! Create a new exception
    /*!
     * \param _what        The exception message, goes to std::exception::what()
     * \param _body        The body of the original space command message.
     * \param _line_number The number of the errornous line, defaults to 0.
     */
    SpaceCommandFormatException(std::string _what, std::string _body, int _line_number);

    //! Virtual destructor derrived from std::exception.
    virtual ~SpaceCommandFormatException() throw();

    //! Get the exception message.
    /*!
     * \returns the exception message
     */
    virtual const char* what() const throw();

    //! Get the space command body
    /*!
     * The body is unaltered and can be used for further inspection.
     *
     * \returns the space command body in raw format
     */
    virtual const char* body() const throw();

    //! Get the line number.
    /*!
     * \returns the number of the errornous line.
     */
    virtual int line_number() const throw();

private:
    std::string m_what;
    std::string m_body;
    int m_line_number;
};


//! Space Command representation.
/*!
 * This class represents a Space Command, consisting of the issuing/receiving
 * peer, the command name and the parameter map.
 */
//TODO copy constructor
class SpaceCommand {
public:
    //! Convenience typedef for the parameter map.
    typedef std::map<const std::string, std::string> space_command_params;

    //! Create a new Space Command representation.
    /*!
     * \param _cmd    The command name
     * \param _params The parameter map.
     */
    SpaceCommand(const std::string _cmd, space_command_params _params);

    //! Get the command name.
    /*!
     * \returns The command name
     */
    std::string cmd();

    //! Get a parameter value
    /*!
     * \param key The parameter name.
     * \returns The value for the provided key.
     * \throws std::out_of_range if the parameter does not exist.
     */
    std::string param(const std::string key) throw(std::out_of_range);

    //! Create a message body representation of this Space Command.
    /*!
     * Converts the Space Command instance into an equivalent message body
     * that can be sent via XMPP to other peers.
     * \returns Message body representation of this Space Command.
     * \sa SpaceControlClient::parseMessage
     */
    std::string as_body();
private:
    std::string m_cmd;
    space_command_params m_params;
};


//! Interface to send out Space Commands
/*!
/* A sink that has been provided to a command handler, must not be disposed.
 * However, if the sink has been obtained explicitly, the caller is
 * responsible for disposing and deleting the sink object.
 */
class SpaceCommandSink {
public:
    //! Send a Space Command.
    /*!
     * \param sc The space command to be sent.
     */
    virtual void sendSpaceCommand(SpaceCommand* sc) = 0;

    //! Dispose of the sink.
    virtual void dispose() = 0;
};


//! Interface for space control handlers to receive and process messages.
/*!
 * This class must be derrived in order to handle message sent to the
 * Space Control client, i.e. this is the entry point for implementing
 * applications.
 */
class SpaceControlHandler {
public:
    //TODO allow for multiple responses -> rather a callback interface than a response pointer
    //! Handle an incoming Space Command
    /*!
     * \param peer The communication peer as provided by GLOOX
     * \param sc   The incoming Space Command instance.
     * \param sink The sink for response commands.
     */
    virtual void handleSpaceCommand(gloox::JID peer, SpaceCommand sc, SpaceCommandSink* sink) = 0;
};


//! The space control client.
/*!
 * An XMPP client, that listens for XMPP messages, converts them to
 * Space Commands and deals with the Space Command handler.
 */
class SpaceControlClient : public gloox::MessageSessionHandler, gloox::MessageHandler {
public:
    //! Create the space control client
    /*!
     * \param _client An already configured XMPP client.
     * \param _hnd the space control handler to process the received Space Commands
     */
    SpaceControlClient(gloox::Client* _client, SpaceControlHandler* _hnd);

    //! message session handler
    /*!
     * \sa gloox::MessageSessionHandler::handleMessageSession
     */
    virtual void handleMessageSession(gloox::MessageSession* session);

    //! message handler
    /*!
     * Converts an XMPP message into a Space Command and calls the
     * Space Command handler.
     * \sa gloox::MessageHandler::handleMessage
     */
    virtual void handleMessage(const gloox::Message& msg, gloox::MessageSession* session = 0);

    //! Get the XMPP client.
    /*!
     * \returns the XMPP client used by this space control client.
     */
    gloox::Client* client();

    //! Get the space control handler.
    /*!
     * \returns the space control handler used by this space control client.
     */
    SpaceControlHandler* handler();

    //! Create a space command sink with a new session.
    /*!
     * \param peer The communication peer for the session.
     * \returns a space command sink with a new session.
     */
    SpaceCommandSink* create_sink(gloox::JID peer);

protected:
    //! Parse an XMPP message body
    /*!
     * \param body The message body
     * \returns A Space Command representation of the message body.
     * \throws SpaceCommandFormatException if the body could not be parsed.
     */
    SpaceCommand parseMessage(std::string body) throw(SpaceCommandFormatException);

private:
    gloox::Client* m_client;
    SpaceControlHandler* m_hnd;
};

}

#endif // SPACECONTROLCLIENT_H__
