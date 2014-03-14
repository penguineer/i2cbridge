//TODO copyright note
//TODO comments
//TODO extrn?

#ifndef CONFIGUREDCLIENTFACTORY_H__
#define CONFIGUREDCLIENTFACTORY_H__

#include <exception>

#include <libconfig.h++>

#include <gloox/jid.h>
#include <gloox/client.h>

namespace xmpppi {

class ConfiguredClientFactoryException : public std::exception {
public:
    ConfiguredClientFactoryException(std::string _what);
    virtual ~ConfiguredClientFactoryException() throw();
    virtual const char* what() const throw();

private:
    std::string m_what;
};

class ConfiguredClientFactory {
public:
    ConfiguredClientFactory(std::string _filename);
    
    virtual ~ConfiguredClientFactory();

    std::string filename();

    gloox::Client* newClient(const char* _resource = 0) throw(ConfiguredClientFactoryException);

private:
    std::string m_filename;
    libconfig::Config* m_cfg;

    void loadConfig() throw (ConfiguredClientFactoryException);
};


}

#endif // CONFIGUREDCLIENTFACTORY_H__
