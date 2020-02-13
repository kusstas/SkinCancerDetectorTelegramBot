#pragma once

#include "Settings.h"

namespace utils
{
class SettingsReader
{
public:
    SettingsReader() = default;

    Settings read(QString const& path) const;
};
}

