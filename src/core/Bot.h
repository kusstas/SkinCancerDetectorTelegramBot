#pragma once

#include <QObject>
#include <rep_SkinCancerDetectorService_replica.h>

#include <memory>

#include "utils/Settings.h"
#include "utils/Translator.h"
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

private slots:
    void onMessage(TelegramBotUpdate const& update);

private:
    void createComponents();
    void connectToScdService(QUrl const& url);
    void connectToTelegram(QString const& token);

    void handleMessage(TelegramBotMessage const* message);
    void handleDocumentMessage(TelegramBotDocument const& document, TelegramBotUser const& from);
    void handlePhotoMessage(QList<TelegramBotPhotoSize> const& photos, TelegramBotUser const& from);
    void handleTextMessage(QString const& text, TelegramBotUser const& from);

private:
    utils::Settings m_settings{};
    std::unique_ptr<utils::Translator> m_translator = nullptr;

    TelegramBot* m_telegram = nullptr;
    SkinCancerDetectorServiceReplica* m_scdService = nullptr;
};
}
