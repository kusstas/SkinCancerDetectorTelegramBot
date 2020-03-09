#pragma once

#include <QObject>
#include <QList>
#include <QHash>

#include <rep_SkinCancerDetectorService_replica.h>

#include <memory>

#include <telegrambot.h>

#include "utils/Settings.h"
#include "utils/BotHelper.h"
#include "utils/Downloader.h"
#include "BotTalker.h"


namespace db
{
class IDbCommunicationHandler;
}

namespace core
{
class BotTalker;

class Bot : public QObject
{
    Q_OBJECT
public:
    explicit Bot(QObject* parent = nullptr);

    void start();

private:
    struct MessageHandler
    {
        using Handler = void (Bot::*)(TelegramBotMessage const&);

        QString key{};
        Handler handler = nullptr;
    };

    struct Request
    {
        TelegramBotUpdate update;
        TelegramBotFile file;
    };

    struct ScdRequest
    {
        Request request;
        QByteArray data;
        SkinCancerDetectorRequestInfo info;
    };

    using Requests = QHash<quint64, Request>;
    using ScdRequests = QHash<quint64, ScdRequest>;

private:
    void createComponents();
    void connectToScdService(QUrl const& url);
    void connectToTelegram(QString const& token);

    void onMessage(TelegramBotUpdate const& update);
    void handleMessage(TelegramBotUpdate const& update);
    void handleCallback(TelegramBotCallbackQuery const& callback);

    void onStart(TelegramBotMessage const& message);
    void onHelp(TelegramBotMessage const& message);
    void onReport(TelegramBotMessage const& message);

    void handle(quint64 id, QString const& fileId, TelegramBotUpdate const& update);
    void onDownloadComplete(quint64 id, QByteArray const& data);
    void onScdResultReady(quint64 id, SkinCancerDetectorResult const& result);
    void onScdResultFailed(quint64 id, SkinCancerDetectorServiceReplica::ErrorType error);
    QString saveFile(QByteArray const& data);

    quint64 getRequestId();

private:
    utils::BotHelper m_helper{};
    utils::Settings m_settings{};
    utils::Downloader m_downloader{};
    SkinCancerDetectorServiceReplica* m_scdService = nullptr;
    db::IDbCommunicationHandler* m_dbCommunicationHandler = nullptr;
    QString m_urlDownloadFormat{};

    TelegramBot* m_telegram = nullptr;
    TelegramBotUser m_me{};
    std::unique_ptr<BotTalker> m_botTalker = nullptr;

    QList<MessageHandler> m_messageHandlers{};
    Requests m_requests{};
    ScdRequests m_scdRequests{};
    quint64 m_requestsId = 0;
};
}
