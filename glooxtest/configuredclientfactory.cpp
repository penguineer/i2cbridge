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

//TODO copyright notice
//TODO comments

#include "configuredclientfactory.h"



#include <iostream>
#include <string>
#include <sstream>


using namespace xmppsc;


ConfiguredClientFactoryException::ConfiguredClientFactoryException(std::string _what)
    : m_what(_what) {}

ConfiguredClientFactoryException::~ConfiguredClientFactoryException() throw() {}

const char* ConfiguredClientFactoryException::what() const throw() {
    return m_what.c_str();
}




ConfiguredClientFactory::ConfiguredClientFactory(std::string _filename) : m_filename(_filename), m_cfg(0) {}

ConfiguredClientFactory::~ConfiguredClientFactory() {
    if (m_cfg)
        delete m_cfg;
}


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
