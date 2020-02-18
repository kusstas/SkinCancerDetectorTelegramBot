#include "Bot.h"

#include <QLoggingCategory>

#include <QRemoteObjectNode>
#include <rep_SkinCancerDetectorService_replica.h>

#include "utils/SettingsReader.h"


static constexpr auto SETTINGS_PATH = "settings.json";
static constexpr auto START_TOKEN = "START";

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

void Bot::onMessage(TelegramBotUpdate const& update)
{
    if (update->type != TelegramBotMessageType::Message || update->message == nullptr)
    {
        qCWarning(QLC_BOT) << "Received not message update:" << update->type;
        return;
    }

    handleMessage(update->message);
}

void Bot::createComponents()
{
    m_settings = utils::SettingsReader().read(SETTINGS_PATH);

    if (!m_settings.valid())
    {
        auto message = "Read settings failed";
        qCCritical(QLC_BOT) << message;
        throw std::runtime_error(message);
    }
    else
    {
        qCInfo(QLC_BOT) << m_settings;
    }

    m_translator.reset(new utils::Translator(m_settings.translationsDir(), m_settings.defaultLanguage()));
    connectToScdService(m_settings.serviceUrl());
    connectToTelegram(m_settings.telegramToken());
}

void Bot::connectToScdService(QUrl const& url)
{
    qCInfo(QLC_BOT) << "Try connect to node by url:" << url;

    auto node = new QRemoteObjectNode(this);

    if (!node->connectToNode(url))
    {
        auto message = QString("Cannot connect to node by url: \"%1\"").arg(url.toString());
        qCCritical(QLC_BOT) << qPrintable(message);
        throw std::runtime_error(qPrintable(message));
    }

    m_scdService = node->acquire<SkinCancerDetectorServiceReplica>();

    if (!m_scdService)
    {
        auto message = QString("Cannot acquire scd service replica by url: \"%1\"").arg(url.toString());
        qCCritical(QLC_BOT) << qPrintable(message);
        throw std::runtime_error(qPrintable(message));
    }

    qCInfo(QLC_BOT) << "Acquire gesture replica by url:" << url << "successfully";

    if (!m_scdService->waitForSource())
    {
        auto message = QString("Timeout for waiting source of gesture replica by url: \"%1\"").arg(url.toString());
        qCCritical(QLC_BOT) << qPrintable(message);
        throw std::runtime_error(qPrintable(message));
    }

    qCInfo(QLC_BOT) << "Connect to node by url:" << url << "successfully";
}

void Bot::connectToTelegram(QString const& token)
{
    m_telegram = new TelegramBot(token, this);
    connect(m_telegram, &TelegramBot::newMessage, this, &Bot::onMessage);
}

void Bot::handleMessage(TelegramBotMessage const* message)
{
    qCInfo(QLC_BOT) << "Receive message from:" << message->from.username;

    if (!message->document.fileId.isEmpty())
    {
        handleDocumentMessage(message->document, message->from);
    }
    else if (!message->photo.isEmpty())
    {
        handlePhotoMessage(message->photo, message->from);
    }
    else if (!message->text.isEmpty())
    {
        handleTextMessage(message->text, message->from);
    }
    else
    {
        qCWarning(QLC_BOT) << "Receive unhandled message from chat:" << message->chat.id;
    }
}

void Bot::handleDocumentMessage(TelegramBotDocument const& document, TelegramBotUser const& from)
{

}

void Bot::handlePhotoMessage(QList<TelegramBotPhotoSize> const& photos, TelegramBotUser const& from)
{

}

void Bot::handleTextMessage(QString const& text, TelegramBotUser const& from)
{

}
}
