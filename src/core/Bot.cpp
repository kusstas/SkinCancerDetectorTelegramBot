#include "Bot.h"

#include <QLoggingCategory>

#include <QRemoteObjectNode>
#include <rep_SkinCancerDetectorService_replica.h>

#include "utils/SettingsReader.h"


static constexpr auto SETTINGS_PATH = "settings.json";

namespace core
{
Q_LOGGING_CATEGORY(QLC_BOT, "Bot")

Bot::Bot(QObject* parent)
    : QObject(parent)
{
    createComponents();
}

void Bot::start()
{
    m_telegram->startMessagePulling();
}

void Bot::onNewMessage(TelegramBotUpdate const& update)
{
    qCDebug(QLC_BOT) << update->message->text << update->message->chat.username << update->message->from.languageCode;
}

void Bot::createComponents()
{
    m_settings = utils::SettingsReader().read(SETTINGS_PATH);

    if (!m_settings.valid())
    {
        auto const message = "Read settings failed";
        qCCritical(QLC_BOT) << message;
        throw std::runtime_error(message);
    }
    else
    {
        qCInfo(QLC_BOT) << m_settings;
    }

    connectToScdService(m_settings.serviceUrl());
    connectToTelegram(m_settings.telegramToken());
}

void Bot::connectToScdService(QUrl const& url)
{
    qCInfo(QLC_BOT) << "Try connect to node by url:" << url;

    auto const node = new QRemoteObjectNode(this);

    if (!node->connectToNode(url))
    {
        auto const message = QString("Cannot connect to node by url: \"%1\"").arg(url.toString());
        qCCritical(QLC_BOT) << qPrintable(message);
        throw std::runtime_error(qPrintable(message));
    }

    m_scdService = node->acquire<SkinCancerDetectorServiceReplica>();

    if (!m_scdService)
    {
        auto const message = QString("Cannot acquire scd service replica by url: \"%1\"").arg(url.toString());
        qCCritical(QLC_BOT) << qPrintable(message);
        throw std::runtime_error(qPrintable(message));
    }

    qCInfo(QLC_BOT) << "Acquire gesture replica by url:" << url << "successfully";

    if (!m_scdService->waitForSource())
    {
        auto const message = QString("Timeout for waiting source of gesture replica by url: \"%1\"").arg(url.toString());
        qCCritical(QLC_BOT) << qPrintable(message);
        throw std::runtime_error(qPrintable(message));
    }

    qCInfo(QLC_BOT) << "Connect to node by url:" << url << "successfully";
}

void Bot::connectToTelegram(QString const& token)
{
    m_telegram = new TelegramBot(token, this);

    connect(m_telegram, &TelegramBot::newMessage, this, &Bot::onNewMessage);
}
}
