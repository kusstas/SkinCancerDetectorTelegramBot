#pragma once

#include <QObject>
#include <rep_SkinCancerDetectorService_replica.h>

#include "utils/Settings.h"
#include "telegrambot.h"


class TelegramBot;
class SkinCancerDetectorServiceReplica;

namespace core
{
class Bot : public QObject
{
    Q_OBJECT
public:
    explicit Bot(QObject* parent = nullptr);

    void start();

signals:

private slots:
    void onNewMessage(TelegramBotUpdate const& update);

private:
    void createComponents();
    void connectToScdService(QUrl const& url);
    void connectToTelegram(QString const& token);

private:
    utils::Settings m_settings;

    TelegramBot* m_telegram = nullptr;
    SkinCancerDetectorServiceReplica* m_scdService = nullptr;
};
}
