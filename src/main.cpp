#include <QCoreApplication>

#include "core/Bot.h"


int main(int argc, char *argv[])
{
    qSetMessagePattern("%{time hh:mm::ss.zzz} [%{type}] %{category}: %{message}");

    QCoreApplication a(argc, argv);

    core::Bot bot;

    bot.start();

    return a.exec();
}
