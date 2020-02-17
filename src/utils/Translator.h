#pragma once

#include <QString>
#include <QVariantHash>

namespace utils
{
/**
 * @brief The Translator class - helper for get translate by token from json dictionary
 */
class Translator
{
public:
    Translator(QString const& dir, QString const& defaultCode);


private:
    QString m_defaultCode{};

    QHash<QString, QVariantHash> m_data{};
};
}

