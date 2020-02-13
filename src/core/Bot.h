#pragma once

#include <QObject>


namespace core
{
class Bot : public QObject
{
    Q_OBJECT
public:
    explicit Bot(QObject* parent = nullptr);

signals:

};
}
