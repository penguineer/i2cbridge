//TODO copyright note
//TODO comments
//TODO extrn?

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

typedef std::map<const std::string, std::string> space_command_params;

class SpaceCommand {
private:
    gloox::JID m_peer;
    std::string m_cmd;
    space_command_params m_params;
public:
    SpaceCommand(gloox::JID _peer, const std::string _cmd,
                 space_command_params _params);

    std::string cmd();
    gloox::JID peer();
    std::string param(const std::string key) throw(std::out_of_range);
};

class SpaceControlHandler {
public:
    virtual void handleSpaceCommand(SpaceCommand sc, SpaceCommand* &response) = 0;
};


class SpaceControlClient : public gloox::MessageSessionHandler, gloox::MessageHandler {
private:
    gloox::Client* m_client;
    SpaceControlHandler* m_hnd;
public:
    SpaceControlClient(gloox::Client* _client, SpaceControlHandler* _hnd);

    virtual void handleMessageSession(gloox::MessageSession* session);

    virtual void handleMessage(const gloox::Message& msg, gloox::MessageSession* session = 0);

    gloox::Client* client();

    SpaceControlHandler* handler();
};

}

#endif // SPACECONTROLCLIENT_H__