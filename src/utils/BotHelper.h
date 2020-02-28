#pragma once

#include <QString>


namespace utils
{
struct BotReportCallbacks
{
    QString trueCallback{};
    QString falseCallback{};
};

struct BotReportResponse
{
    bool valid = true;
    bool report = false;
    qint32 reportedMessageId = 0;
};

class BotHelper
{
public:
    BotHelper() = default;

    BotReportCallbacks makeCallbacks(qint32 reportMessageId) const;
    BotReportResponse parseReportCallback(QString const& callback) const;
};
}
