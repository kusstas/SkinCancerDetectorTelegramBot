#include "Bot.h"

#include <QLoggingCategory>

#include <QRemoteObjectNode>
#include <rep_SkinCancerDetectorService_replica.h>

#include "utils/SettingsReader.h"


static constexpr auto SETTINGS_PATH = "settings.json";
static constexpr auto START_KEY = "/start";
static constexpr auto HELP_KEY = "/help";
static constexpr auto REPORT_KEY = "/report";

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
    switch (update->type)
    {
    case TelegramBotMessageType::Message:
        handleMessage(*update->message);
        break;
    case TelegramBotMessageType::CallbackQuery:
        handleCallback(*update->callbackQuery);
        break;
    default:
        qCWarning(QLC_BOT) << "Received not hadnled update:" << update->type;
        break;
    }

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

    connectToScdService(m_settings.serviceUrl());
    connectToTelegram(m_settings.telegramToken());

    m_botTalker = std::move(std::make_unique<BotTalker>(m_telegram,
                                                        std::make_shared<utils::Translator>(m_settings.translationsDir(), m_settings.defaultLanguage())));
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
    m_me = m_telegram->getMe();

    connect(m_telegram, &TelegramBot::newMessage, this, &Bot::onMessage);

    m_messageHandlers =
    {
        {START_KEY, &Bot::onStart},
        {HELP_KEY, &Bot::onHelp},
        {REPORT_KEY, &Bot::onReport}
    };
}

void Bot::handleMessage(TelegramBotMessage const& message)
{
    qCInfo(QLC_BOT) << "Receive message from:" << message.from.username;

    if (!message.document.fileId.isEmpty())
    {
    }
    else if (!message.photo.isEmpty())
    {
    }
    else if (!message.text.isEmpty())
    {
        bool handled = false;

        for (auto const& handler : m_messageHandlers)
        {
            if (message.text.startsWith(handler.key))
            {
                (this->*handler.handler)(message);
                handled = true;
                break;
            }
        }

        if (!handled)
        {
            m_botTalker->undefined(message);
        }
    }
    else
    {
        qCWarning(QLC_BOT) << "Receive unhandled message from chat:" << message.chat.id;
    }
}

void Bot::handleCallback(TelegramBotCallbackQuery const& callback)
{
    auto response = m_helper.parseReportCallback(callback.data);

    if (response.valid)
    {
        m_botTalker->successReport(callback.message);
        // TODO: write to db
    }
    else
    {
        m_botTalker->failedReport(callback.message);
    }
}

void Bot::onStart(TelegramBotMessage const& message)
{
    m_botTalker->hello(message);
}

void Bot::onHelp(TelegramBotMessage const& message)
{
    m_botTalker->help(message);
}

void Bot::onReport(TelegramBotMessage const& message)
{
    if (message.replyToMessage.text.isEmpty() || message.replyToMessage.from.id != m_me.id)
    {
        m_botTalker->invalidReport(message);
    }
    else
    {
        auto callbacks = m_helper.makeCallbacks(message.replyToMessage.messageId);
        m_botTalker->report(message, callbacks.trueCallback, callbacks.falseCallback);
    }
}
}
