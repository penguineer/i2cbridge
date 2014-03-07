//TODO copyright notice
//TODO comments


#include <iostream>
#include <string>
#include <sstream>

#include <gloox/jid.h>
#include <gloox/client.h>

#include <libconfig.h++>

#include "configuredclientfactory.h"


using namespace xmpppi;


ConfiguredClientFactoryException::ConfiguredClientFactoryException(std::string _what)
    : m_what(_what) {}

ConfiguredClientFactoryException::~ConfiguredClientFactoryException() throw() {}

const char* ConfiguredClientFactoryException::what() const throw() {
    return m_what.c_str();
}




ConfiguredClientFactory::ConfiguredClientFactory(std::string _filename) : m_filename(_filename), m_cfg(0) {}

std::string ConfiguredClientFactory::filename() {
    return m_filename;
}

gloox::Client* ConfiguredClientFactory::newClient(const char* _resource) throw(ConfiguredClientFactoryException) {
    if (!m_cfg)
        loadConfig();

    gloox::Client* client = 0;
    try {
        std::string cfg_jid = m_cfg->lookup("xmpp.jid");
        std::string cfg_password = m_cfg->lookup("xmpp.password");

        gloox::JID jid(cfg_jid);
        client = new gloox::Client(jid, cfg_password);
    } catch (const libconfig::SettingNotFoundException &snfex) {
        std::ostringstream msg;
        msg << "Setting not found: " << snfex.getPath() << std::endl;
        throw ConfiguredClientFactoryException(msg.str());
    }
    return client;
}

void ConfiguredClientFactory::loadConfig() throw(ConfiguredClientFactoryException) {
    // delete old config
    if (m_cfg)
        delete m_cfg;

    //TODO check possible config directories
    
    m_cfg = new libconfig::Config();

    try {
        m_cfg->readFile(filename().c_str());
    } catch (const libconfig::FileIOException &fioex) {
        throw ConfiguredClientFactoryException("I/O error on reading config file!");
    } catch(const libconfig::ParseException &pex) {
        std::ostringstream msg;
        msg << "Parse error at " << pex.getFile() << ":" << pex.getLine()
            << " - " << pex.getError() << std::endl;
        throw ConfiguredClientFactoryException(msg.str());
    }
}

// End of File
