#pragma once

#include <QString>
#include <QDateTime>

namespace db
{
struct DbKey
{
    qint32 chatId = 0;
    qint32 messageId = 0;
};

struct DbData
{
    DbKey key{};
    QString imagePath{};
    QString nnResult{};
    QString userReport{};
    QDateTime dateTime{};
};

class IDbCommunicationHandler
{
public:
    virtual ~IDbCommunicationHandler() { }

    virtual bool exist(DbKey const& key) const = 0;
    virtual bool addData(DbData const& data) = 0;
    virtual bool addUserReport(DbKey const& key, QString const& userReport) = 0;
    virtual QList<DbKey> getAllUndefineReportedKeys() const = 0;
    virtual DbData getData(DbKey const& key) const = 0;
};
}
