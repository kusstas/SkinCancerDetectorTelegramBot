#pragma once

#include "Settings.h"


class QJsonObject;

namespace utils
{
/**
 * @brief The SettingsReader class - read settings from json file
 */
class SettingsReader
{
public:
    SettingsReader() = default;

    /**
     * @brief read settings from json file by path
     * @param path
     * @return settings
     */
    Settings read(QString const& path) const;

private:
    static bool checkRequirements(QJsonObject const* src, QStringList const& list);
};
}

