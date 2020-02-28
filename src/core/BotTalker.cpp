#include "BotTalker.h"

#include <QLoggingCategory>

namespace core
{
Q_LOGGING_CATEGORY(QLC_BOT_TALKER, "BotTalker")

static constexpr auto START_TOKEN = "START";
static constexpr auto HELP_TOKEN = "HELP";
static constexpr auto REPORT_TOKEN = "REPORT";
static constexpr auto INVALID_REPORT_TOKEN = "INVALID_REPORT";
static constexpr auto FAILED_REPORT_TOKEN = "FAILED_REPORT";
static constexpr auto SUCCESS_REPORT_TOKEN = "SUCCESS_REPORT";
static constexpr auto TRUE_REPORT_TOKEN = "TRUE_REPORT";
static constexpr auto FALSE_REPORT_TOKEN = "FALSE_REPORT";
static constexpr auto UNDEF_TOKEN = "UNDEFINED";

BotTalker::BotTalker(TelegramBot* telegram, std::shared_ptr<utils::Translator> const& translator)
{
    if (telegram == nullptr || translator == nullptr)
    {
        auto message = "Invalid arguments for create bot talker";
        qCCritical(QLC_BOT_TALKER) << message;
        throw std::runtime_error(message);
    }

    m_telegram = telegram;
    m_translator = translator;
}

void BotTalker::hello(TelegramBotMessage const& message) const
{
    m_telegram->sendMessage(message.chat.id,
                            m_translator->get(START_TOKEN, message.from.languageCode).arg(message.from.firstName));
}

void BotTalker::help(TelegramBotMessage const& message) const
{
    m_telegram->sendMessage(message.chat.id,
                            m_translator->get(HELP_TOKEN, message.from.languageCode));
}

void BotTalker::undefined(TelegramBotMessage const& message) const
{
    m_telegram->sendMessage(message.chat.id,
                            m_translator->get(UNDEF_TOKEN, message.from.languageCode));
}

void BotTalker::report(const TelegramBotMessage& message, QString const& trueCallback, QString const& falseCallback) const
{
    TelegramBotKeyboardButtonRequest trueKey;
    TelegramBotKeyboardButtonRequest falseKey;

    trueKey.text = m_translator->get(TRUE_REPORT_TOKEN, message.from.languageCode);
    falseKey.text = m_translator->get(FALSE_REPORT_TOKEN, message.from.languageCode);

    trueKey.callbackData = trueCallback;
    falseKey.callbackData = falseCallback;

    m_telegram->sendMessage(message.chat.id,
                            m_translator->get(REPORT_TOKEN, message.from.languageCode),
                            0,
                            TelegramBot::TelegramFlags::NoFlag,
    {{trueKey, falseKey}});
}

void BotTalker::invalidReport(TelegramBotMessage const& message)
{
    m_telegram->sendMessage(message.chat.id,
                            m_translator->get(INVALID_REPORT_TOKEN, message.from.languageCode));
}

void BotTalker::failedReport(const TelegramBotMessage& message)
{
    m_telegram->sendMessage(message.chat.id,
                            m_translator->get(FAILED_REPORT_TOKEN, message.from.languageCode));
}

void BotTalker::successReport(const TelegramBotMessage& message)
{
    m_telegram->sendMessage(message.chat.id,
                            m_translator->get(SUCCESS_REPORT_TOKEN, message.from.languageCode));
}
}
