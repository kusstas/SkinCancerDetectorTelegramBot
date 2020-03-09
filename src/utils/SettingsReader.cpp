#include "SettingsReader.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>

#include <QLoggingCategory>


namespace utils
{
Q_LOGGING_CATEGORY(QLC_SETTINGS, "SettingsReader")

Settings SettingsReader::read(QString const& path) const
{
    Settings result{};
    QFile file(path);

    if (!file.open(QFile::ReadOnly))
    {
        qCCritical(QLC_SETTINGS) << "Failed to open settings file:" << path;
        return result;
    }

    QJsonParseError error;
    QJsonObject root = QJsonDocument::fromJson(file.readAll(), &error).object();

    if (error.error != QJsonParseError::NoError)
    {
        qCCritical(QLC_SETTINGS) << "Error parse:" << error.errorString();
        return result;
    }

    if (root.empty())
    {
        qCCritical(QLC_SETTINGS) << "Nothing constains in root";
        return result;
    }

    auto const TOKEN_KEY = "telegramToken";
    auto const SERVICE_KEY = "serviceUrl";
    auto const TR_DIR_KEY = "translationsDir";
    auto const DEFAULT_LANGUAGE_KEY = "defaultLanguage";
    auto const MAX_FILE_SIZE_KEY = "maxFileSize";
    auto const DATA_FOLDER_KEY = "dataFolder";
    auto const DATABASE_NAME_KEY = "databaseName";

    if (!checkRequirements(&root,
        {TOKEN_KEY, SERVICE_KEY, TR_DIR_KEY, DEFAULT_LANGUAGE_KEY, MAX_FILE_SIZE_KEY, DATA_FOLDER_KEY, DATABASE_NAME_KEY}))
    {
        return result;
    }

    result.setTelegramToken(root[TOKEN_KEY].toString());
    result.setServiceUrl(QUrl(root[SERVICE_KEY].toString()));
    result.setTranslationsDir(root[TR_DIR_KEY].toString());
    result.setDefaultLanguage(root[DEFAULT_LANGUAGE_KEY].toString());
    result.setMaxFileSize(root[MAX_FILE_SIZE_KEY].toInt());
    result.setDataFolder(root[DATA_FOLDER_KEY].toString());
    result.setDatabaseName(root[DATABASE_NAME_KEY].toString());

    return result;
}

bool SettingsReader::checkRequirements(QJsonObject const* src, QStringList const& list)
{
    QStringList lacks;

    for (auto const& key : list)
    {
        if (!src->contains(key))
        {
            lacks.append(key);
        }
    }

    if (!lacks.empty())
    {
        qCCritical(QLC_SETTINGS) << "Lacks this keys:" << lacks;
    }

    return lacks.empty();
}
}
