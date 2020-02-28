#pragma once

#include <memory>

#include <telegrambot.h>

#include "utils/Translator.h"

namespace core
{
class BotTalker
{
public:
    BotTalker(TelegramBot* telegram, std::shared_ptr<utils::Translator> const& translator);

    void hello(TelegramBotMessage const& message) const;
    void help(TelegramBotMessage const& message) const;
    void undefined(TelegramBotMessage const& message) const;
    void report(TelegramBotMessage const& message, QString const& trueCallback, QString const& falseCallback) const;
    void invalidReport(TelegramBotMessage const& message);
    void failedReport(TelegramBotMessage const& message);
    void successReport(TelegramBotMessage const& message);

private:
    TelegramBot* m_telegram = nullptr;
    std::shared_ptr<utils::Translator> m_translator = nullptr;
};
}
