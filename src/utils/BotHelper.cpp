#include "BotHelper.h"

#include <QLoggingCategory>

static constexpr auto DELIMETER = ':';
static constexpr int COUNT_COMPONENTS = 2;
static auto const TRUE_STRING = QVariant(true).toString();
static auto const FALSE_STRING = QVariant(false).toString();

namespace utils
{
Q_LOGGING_CATEGORY(QLC_BOT_HELPER, "BotHelper")

BotReportCallbacks BotHelper::makeCallbacks(qint32 reportMessageId) const
{
    BotReportCallbacks result;

    QStringList components =
    {
        QString(), QString::number(reportMessageId)
    };

    components.first() = TRUE_STRING;
    result.trueCallback = components.join(DELIMETER);

    components.first() = FALSE_STRING;
    result.falseCallback = components.join(DELIMETER);

    return result;
}

BotReportResponse BotHelper::parseReportCallback(QString const& callback) const
{
    BotReportResponse result;

    auto components = callback.splitRef(DELIMETER);

    if (components.size() != COUNT_COMPONENTS)
    {
        return result;
    }

    if (components.first() == TRUE_STRING)
    {
        result.report = true;
    }
    else if (components.first() == FALSE_STRING)
    {
        result.report = false;
    }
    {
        return result;
    }

    result.reportedMessageId = components[1].toInt();

    result.valid = true;
    return result;
}
}
