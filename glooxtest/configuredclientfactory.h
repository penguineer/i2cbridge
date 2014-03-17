//TODO copyright note
//TODO extrn?

#ifndef CONFIGUREDCLIENTFACTORY_H__
#define CONFIGUREDCLIENTFACTORY_H__

#include <exception>

#include <libconfig.h++>

#include <gloox/jid.h>
#include <gloox/client.h>

namespace xmpppi {

//! Exception on errors with the client factory configuration.
class ConfiguredClientFactoryException : public std::exception {
public:
    //! Create a new exception.
    /*!
     * \param _what The exception message.
     */
    ConfiguredClientFactoryException(std::string _what);

    //! Virtual destructor.
    virtual ~ConfiguredClientFactoryException() throw();

    //! Get the exception message.
    /*!
     * Derrived from std::exception.
     * \returns the exception message.
     * \sa std::exception
     */
    virtual const char* what() const throw();

private:
    std::string m_what;
};


//! XMPP client factory that uses configuration file to get parameters.
/*!
 * This class is a factory for the gloox::Client.
 *
 * Parameters are loaded from the provided config file and used to initialize
 * the GLOOX XMPP client.
 */
class ConfiguredClientFactory {
public:
    //! Create a factory for the given configuration file.
    /*!
     * \param _filename The path to the configuration file.
     */
    ConfiguredClientFactory(std::string _filename);

    //! Virtual destructor for the client factory.
    virtual ~ConfiguredClientFactory();

    //! Get the path to the configuration file.
    /*!
     * \returns the path to the configuration file.
     */
    std::string filename();

    //! Create a new client.
    /*!
     * Create a new client configured with the values from the configuration file.
     * \param _resource The XMPP resource identifier to use for the connection.
     * \returns a gloox::Client
     * \throws ConfiguredClientFactoryException if the configuration is invalid.
     * \sa gloox::Client
     */
    gloox::Client* newClient(const char* _resource = 0) throw(ConfiguredClientFactoryException);

private:
    std::string m_filename;
    libconfig::Config* m_cfg;

    void loadConfig() throw(ConfiguredClientFactoryException);
};


}

#endif // CONFIGUREDCLIENTFACTORY_H__
