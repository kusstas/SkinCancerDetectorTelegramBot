#pragma once

#include <QString>
#include <QVariantHash>
#include <QFileInfo>

namespace utils
{
/**
 * @brief The Translator class - helper for get translate by token from json dictionary
 */
class Translator
{
public:
    /**
     * @brief Translator
     * @param dir - path to directory which contains json translation files
     * @param defaultCode - default language code
     */
    Translator(QString const& dir, QString const& defaultCode);

    /**
     * @brief get translation by token
     * @param token - token which contains translation
     * @param languageCode - language code
     * @return
     */
    QString get(QString const& token, QString const& languageCode) const;

private:
    QList<QFileInfo> getFiles(QString const& path) const;
    QVariantHash parseFile(QFileInfo const& fileInfo) const;
    bool containsDefault(QList<QFileInfo> const& files) const;

private:
    QString m_defaultCode{};
    QHash<QString, QVariantHash> m_data{};
    QVariantHash m_defaultData{};
};
}

