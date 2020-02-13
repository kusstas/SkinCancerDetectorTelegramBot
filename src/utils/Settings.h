#pragma once

#include <QUrl>
#include <QString>

namespace utils
{
/**
 * @brief The Settings class - contains all settings
 */
class Settings
{
public:
    Settings() = default;

    /**
     * @brief valid settings
     * @return
     */
    bool valid() const;

    /**
     * @brief telegram access token
     * @return
     */
    QString const& telegramToken() const;

    /**
     * @brief service url for handle requests
     * @return
     */
    QUrl const& serviceUrl() const;

    /**
     * @brief translations dir
     * @return
     */
    QString const& translationsDir() const;

    /**
     * @brief default language
     * @return
     */
    QString const& defaultLanguage() const;

    /**
     * @brief set telegram token
     * @param telegramToken
     */
    void setTelegramToken(QString const& telegramToken);

    /**
     * @brief set service url
     * @param serviceUrl
     */
    void setServiceUrl(QUrl const& serviceUrl);

    /**
     * @brief set translations dir
     * @param translationsDir
     */
    void setTranslationsDir(QString const& translationsDir);

    /**
     * @brief set defaultLlanguage
     * @param defaultLanguage
     */
    void setDefaultLanguage(QString const& defaultLanguage);

private:
    QString m_telegramToken{};
    QUrl m_serviceUrl{};
    QString m_translationsDir{};
    QString m_defaultLanguage{};
};
}
