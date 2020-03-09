#pragma once

#include <QObject>
#include <QQueue>
#include <QNetworkAccessManager>


namespace utils
{
class Downloader : public QObject
{
    Q_OBJECT

public:
    explicit Downloader(QObject* parent = nullptr);

    size_t maxParallels() const;
    void setMaxParallel(size_t maxParallels);

public slots:
    void download(quint64 id, QUrl const& url);

signals:
    void complete(quint64 id, QByteArray const& data);

private:
    bool needDeffered() const;
    void onComplete(quint64 id, QNetworkReply* reply, bool success);
    void checkDefferedRequests();

private:
    struct Request
    {
        quint64 id;
        QUrl url;
    };

private:
    size_t m_maxParallels = 1;

    QNetworkAccessManager m_networkAccessManager{};
    QQueue<Request> m_defferedRequests{};
    size_t m_inProgress = 0;
};
}
