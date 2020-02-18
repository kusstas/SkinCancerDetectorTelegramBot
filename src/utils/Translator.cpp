#include "Translator.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include <QLoggingCategory>


static const auto FILE_FILTER = QStringList{"*.json"};

namespace utils
{
Q_LOGGING_CATEGORY(QLC_TR, "Translator")

Translator::Translator(QString const& dir, QString const& defaultCode)
    : m_defaultCode(defaultCode)
{
    qCInfo(QLC_TR) << "Try create translator, dir:" << dir << "defaultCode:" << defaultCode;

    auto const files = getFiles(dir);

    if (files.isEmpty())
    {
        auto const message = "No files translations";
        qCCritical(QLC_TR) << message;
        throw std::runtime_error(message);
    }

    qCInfo(QLC_TR) << "Finded files in dir:" << files;

    if (!containsDefault(files))
    {
        auto const message = "No default file translations";
        qCCritical(QLC_TR) << message;
        throw std::runtime_error(message);
    }

    auto tmpData = m_data;

    for (auto const& file : files)
    {
        auto const data = parseFile(file);

        if (data.isEmpty())
        {
            auto const message = QString("Cannot parse file: %1").arg(file.filePath());
            qCCritical(QLC_TR) << qPrintable(message);
            throw std::runtime_error(qPrintable(message));
        }
        else
        {
            tmpData[file.baseName()] = data;
        }
    }

    m_data = tmpData;
    m_defaultData = m_data[m_defaultCode];

    qCInfo(QLC_TR) << "Translator created successfully";
}

QString Translator::get(QString const& token, QString const& languageCode) const
{
    return m_data.value(languageCode, m_defaultData).value(token, token).toString();
}

QList<QFileInfo> Translator::getFiles(QString const& path) const
{
    QDir dir(path);

    if (!dir.exists())
    {
        qCCritical(QLC_TR) << "Dir not exists:" << dir;
        return {};
    }

    return dir.entryInfoList(FILE_FILTER, QDir::Files);
}

QVariantHash Translator::parseFile(QFileInfo const& fileInfo) const
{
    QFile file(fileInfo.filePath());

    if (!file.open(QFile::ReadOnly))
    {
        qCCritical(QLC_TR) << "Cannot open file:" << fileInfo << "error:" << file.errorString();
        return {};
    }

    QJsonParseError jError;
    auto root = QJsonDocument::fromJson(file.readAll(), &jError).object();

    if (jError.error != QJsonParseError::NoError)
    {
        qCCritical(QLC_TR) << "Cannot parse json:" << jError.errorString();
        return {};
    }
    if (root.isEmpty())
    {
        qCCritical(QLC_TR) << "Root object is empty";
        return {};
    }

    return root.toVariantHash();
}

bool Translator::containsDefault(QList<QFileInfo> const& files) const
{
    for (auto const& file : files)
    {
        if (file.baseName() == m_defaultCode)
        {
            return true;
        }
    }

    return false;
}
}
