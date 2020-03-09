#include "Bot.h"

#include <QLoggingCategory>
#include <QDir>

#include <QRemoteObjectNode>

#include "utils/SettingsReader.h"
#include "database/IDbCommunicationHandler.h"


static constexpr auto SETTINGS_PATH = "settings.json";
static constexpr auto START_KEY = "/start";
static constexpr auto HELP_KEY = "/help";
static constexpr auto REPORT_KEY = "/report";
static constexpr auto DOWNLOAD_URL_FORMAT = "https://api.telegram.org/file/bot%1/%2";
static constexpr auto INFO_WAIT_MS = 1000;

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

void Bot::createComponents()
{
    m_settings = utils::SettingsReader().read(SETTINGS_PATH);
    m_urlDownloadFormat = QString(DOWNLOAD_URL_FORMAT).arg(m_settings.telegramToken());

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
    connect(&m_downloader, &utils::Downloader::complete, this, &Bot::onDownloadComplete);
    connect(m_scdService, &SkinCancerDetectorServiceReplica::resultReady, this, &Bot::onScdResultReady);
    connect(m_scdService, &SkinCancerDetectorServiceReplica::resultFailed, this, &Bot::onScdResultFailed);

    auto const translator = std::make_shared<utils::Translator>(m_settings.translationsDir(), m_settings.defaultLanguage());
    m_botTalker = std::make_unique<BotTalker>(m_telegram, translator);


    if (!QDir(m_settings.dataFolder()).exists())
    {
        QDir::current().mkdir(m_settings.dataFolder());
    }
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
    qCInfo(QLC_BOT) << "Trying connect to telegram";

    m_telegram = new TelegramBot(token, this);
    m_me = m_telegram->getMe();

    connect(m_telegram, &TelegramBot::newMessage, this, &Bot::onMessage);

    m_messageHandlers =
    {
        {START_KEY, &Bot::onStart},
        {HELP_KEY, &Bot::onHelp},
        {REPORT_KEY, &Bot::onReport}
    };

    qCInfo(QLC_BOT) <<  "Connect to telegram successfully";
}

void Bot::onMessage(TelegramBotUpdate const& update)
{
    switch (update->type)
    {
    case TelegramBotMessageType::Message:
        handleMessage(update);
        break;
    case TelegramBotMessageType::CallbackQuery:
        handleCallback(*update->callbackQuery);
        break;
    default:
        qCWarning(QLC_BOT) << "Received not handled update:" << update->type;
        break;
    }
}

void Bot::handleMessage(TelegramBotUpdate const& update)
{
    auto const& message = *update->message;

    qCInfo(QLC_BOT) << "Receive message from:" << message.from.username;

    if (!message.document.fileId.isEmpty())
    {
        handle(getRequestId(), message.document.fileId, update);
    }
    else if (!message.photo.isEmpty())
    {
        handle(getRequestId(), message.photo.last().fileId, update);
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
        m_dbCommunicationHandler->addUserReport(db::DbKey{callback.id.toInt(), response.reportedMessageId},
                                                QVariant(response.report).toString());
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
    if (message.replyToMessage.text.isEmpty()
            || message.replyToMessage.from.id != m_me.id
            || !m_dbCommunicationHandler->exist({message.chat.id, message.replyToMessage.messageId}))
    {
        m_botTalker->invalidReport(message);
    }
    else
    {
        auto callbacks = m_helper.makeCallbacks(message.replyToMessage.messageId);
        m_botTalker->report(message, callbacks.trueCallback, callbacks.falseCallback);
    }
}

void Bot::handle(quint64 id, QString const& fileId, TelegramBotUpdate const& update)
{
    auto const& file = m_telegram->getFile(fileId);

    if (file.fileSize > m_settings.maxFileSize())
    {
        m_botTalker->oversize(*update->message);
        return;
    }

    m_botTalker->inProgress(*update->message);

    QUrl const url(m_urlDownloadFormat.arg(file.filePath));

    m_requests.insert(id, {update, file});
    m_downloader.download(id, url);
}

void Bot::onDownloadComplete(quint64 id, QByteArray const& data)
{
    if (!data.isEmpty())
    {
        auto const& requestData = m_requests[id];
        auto scdRequestInfoReply = m_scdService->request(data);

        if(scdRequestInfoReply.waitForFinished(INFO_WAIT_MS))
        {
            auto const& scdRequestInfo = scdRequestInfoReply.returnValue();
            m_scdRequests.insert(scdRequestInfo.id(), {requestData, data, scdRequestInfo});
        }
        else
        {
            m_botTalker->scdFailed(*requestData.update->message,
                                   SkinCancerDetectorServiceReplica::System,
                                   requestData.update->message->messageId);
        }
    }
    m_requests.remove(id);
}

void Bot::onScdResultReady(quint64 id, SkinCancerDetectorResult const& result)
{
    auto const& requestInfo = m_scdRequests.take(id);

    TelegramBotMessage response;
    m_botTalker->scdSuccess(*requestInfo.request.update->message,
                            result.positive() > result.negative(),
                            requestInfo.request.update->message->messageId,
                            &response);

    if (response.messageId > 0)
    {
        QFileInfo fileInfo(requestInfo.request.file.filePath);
        QString fileName = QString::number(requestInfo.request.update->message->chat.id) + fileInfo.fileName();
        QFile file(QDir(m_settings.dataFolder()).filePath(fileName));

        if (file.open(QFile::WriteOnly))
        {
            file.write(requestInfo.data);
            file.close();

            // TODO: write to db
        }
    }
}

void Bot::onScdResultFailed(quint64 id, SkinCancerDetectorServiceReplica::ErrorType error)
{
    auto const& requestInfo = m_scdRequests[id];
    m_botTalker->scdFailed(*requestInfo.request.update->message, error, requestInfo.request.update->message->messageId);
    m_scdRequests.remove(id);
}

quint64 Bot::getRequestId()
{
    return ++m_requestsId;
}
}
