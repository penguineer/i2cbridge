//TODO copyright note
//TODO comments
//TODO extrn?

#ifndef SPACECONTROLCLIENT_H__
#define SPACECONTROLCLIENT_H__

#include <gloox/client.h>
#include <gloox/message.h>
#include <gloox/messagesession.h>
#include <gloox/messagehandler.h>
#include <gloox/messagesessionhandler.h>


namespace xmpppi {
  
class SpaceControlClient : public gloox::MessageSessionHandler, gloox::MessageHandler {
private:
    gloox::Client* m_client;
public:
    SpaceControlClient(gloox::Client* _client);

    virtual void handleMessageSession(gloox::MessageSession* session);

    virtual void handleMessage(const gloox::Message& msg, gloox::MessageSession* session = 0);

    gloox::Client* client();
};  
  
}


#endif // SPACECONTROLCLIENT_H__
