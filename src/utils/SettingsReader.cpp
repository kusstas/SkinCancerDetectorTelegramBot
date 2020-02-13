#include "SettingsReader.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <QLoggingCategory>

namespace utils
{
Q_LOGGING_CATEGORY(QLC_SETTINGS)

Settings SettingsReader::read(QString const& path) const
{
    QFile file(path);

    if (!file.open(QFile::ReadOnly))
    {
        return {};
    }
}
}
