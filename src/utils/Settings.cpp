#include "Settings.h"


namespace utils
{
bool Settings::valid() const
{
    return !telegramToken().isEmpty()
            && serviceUrl().isValid()
            && !serviceUrl().isEmpty()
            && !translationsDir().isEmpty()
            && !defaultLanguage().isEmpty()
            && maxFileSize() > 0
            && !dataFolder().isEmpty()
            && !databaseName().isEmpty();
}

QString const& Settings::telegramToken() const
{
    return m_telegramToken;
}

QUrl const& Settings::serviceUrl() const
{
    return m_serviceUrl;
}

QString const& Settings::translationsDir() const
{
    return m_translationsDir;
}

QString const& Settings::defaultLanguage() const
{
    return m_defaultLanguage;
}

int Settings::maxFileSize() const
{
    return m_maxFileSize;
}

QString Settings::dataFolder() const
{
    return m_dataFolder;
}

QString Settings::databaseName() const
{
    return m_databaseName;
}

void Settings::setTelegramToken(QString const& telegramToken)
{
    m_telegramToken = telegramToken;
}

void Settings::setServiceUrl(QUrl const& serviceUrl)
{
    m_serviceUrl = serviceUrl;
}

void Settings::setTranslationsDir(QString const& translationsDir)
{
    m_translationsDir = translationsDir;
}

void Settings::setDefaultLanguage(QString const& defaultLanguage)
{
    m_defaultLanguage = defaultLanguage;
}

void Settings::setMaxFileSize(int maxFileSize)
{
    m_maxFileSize = maxFileSize;
}

void Settings::setDataFolder(QString const& dataFolder)
{
    m_dataFolder = dataFolder;
}

void Settings::setDatabaseName(QString const& databaseName)
{
    m_databaseName = databaseName;
}

QDebug operator<<(QDebug d, Settings const& obj)
{
    if (!obj.valid())
    {
        d << "Settings(invalid)";
    }
    else
    {
        d << "Settings("
          << "telegramToken:" << obj.telegramToken()
          << "serviceUrl:" << obj.serviceUrl()
          << "translationsDir:" << obj.translationsDir()
          << "defaultLanguage:" << obj.defaultLanguage()
          << "maxFileSize:" << obj.maxFileSize()
          << "dataFolder:" << obj.dataFolder()
          << "databaseName:" << obj.databaseName()
          << ")";
    }

    return d;
}
}
