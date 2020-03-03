#pragma once

#include <QObject>
#include <QList>
#include <QHash>

#include <rep_SkinCancerDetectorService_replica.h>

#include <memory>

#include <telegrambot.h>

#include "utils/Settings.h"
#include "utils/BotHelper.h"
#include "BotTalker.h"


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

    using Requests = QHash<quint64, TelegramBotUpdate>;

private:
    void createComponents();
    void connectToScdService(QUrl const& url);
    void connectToTelegram(QString const& token);

    void onMessage(TelegramBotUpdate const& update);
    void handleMessage(TelegramBotMessage const& message);
    void handleCallback(TelegramBotCallbackQuery const& callback);

    void onStart(TelegramBotMessage const& message);
    void onHelp(TelegramBotMessage const& message);
    void onReport(TelegramBotMessage const& message);

private:
    utils::BotHelper m_helper{};
    utils::Settings m_settings{};

    TelegramBot* m_telegram = nullptr;
    TelegramBotUser m_me{};
    SkinCancerDetectorServiceReplica* m_scdService = nullptr;
    std::unique_ptr<BotTalker> m_botTalker = nullptr;

    QList<MessageHandler> m_messageHandlers{};
    Requests m_requests{};
};
}
