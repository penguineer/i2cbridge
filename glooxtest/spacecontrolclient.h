//TODO copyright note
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


namespace xmpppi {


//! Exception on parsing errors in space command messages.
/*!
 * This exception is thrown when parsing a message body for SpaceCommandS
 * fails. A description of the failure and the message body are provided.
 */
class SpaceCommandFormatException : public std::exception {
public:
    //! Create a new exception
    /*!
     * \param _what The exception message, goes to std::exception::what()
     * \param _body The body of the original space command message.
     */
    SpaceCommandFormatException(std::string _what, std::string _body);

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

private:
    std::string m_what;
    std::string m_body;
};


//! Space Command representation.
/*!
 * This class represents a Space Command, consisting of the issuing/receiving
 * peer, the command name and the parameter map.
 */
class SpaceCommand {
public:
    //! Convenience typedef for the parameter map.
    typedef std::map<const std::string, std::string> space_command_params;

    //! Create a new Space Command representation.
    /*!
     * \param _peer   The communication peer for the Space Command
     * \param _cmd    The command name
     * \param _params The parameter map.
     */
    SpaceCommand(gloox::JID _peer, const std::string _cmd,
                 space_command_params _params);

    //! Get the communication peer.
    /*!
     * \returns The JID of the communication peer as provided by the GLOOX library.
     */
    gloox::JID peer();

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
    gloox::JID m_peer;
    std::string m_cmd;
    space_command_params m_params;
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
     * \param sc The incoming Space Command instance
     * \param response Put a pointer to the response command here, or NULL if a response shall not be sent.
     */
    virtual void handleSpaceCommand(SpaceCommand sc, SpaceCommand* &response) = 0;
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

protected:
    //! Parse an XMPP message body
    /*!
     * \param peer The communication peer as provided by GLOOX
     * \param body The message body
     * \returns A Space Command representation of the message body.
     * \throws SpaceCommandFormatException if the body could not be parsed.
     */
    SpaceCommand parseMessage(gloox::JID peer, std::string body) throw(SpaceCommandFormatException);

private:
    gloox::Client* m_client;
    SpaceControlHandler* m_hnd;
};

}

#endif // SPACECONTROLCLIENT_H__
