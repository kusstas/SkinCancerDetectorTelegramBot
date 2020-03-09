#include "Downloader.h"

#include <QLoggingCategory>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMetaEnum>


namespace utils
{
Q_LOGGING_CATEGORY(QLC_DOWNLOADER, "Downloader")

Downloader::Downloader(QObject* parent)
    : QObject(parent)
{
}

size_t Downloader::maxParallels() const
{
    return m_maxParallels;
}

void Downloader::setMaxParallel(size_t maxParallels)
{
    m_maxParallels = maxParallels;
}

void Downloader::download(quint64 id, QUrl const& url)
{
    qCDebug(QLC_DOWNLOADER) << "Trying download:" << url;

    if (needDeffered())
    {
        m_defferedRequests.append({id, url});
        return;
    }
    ++m_inProgress;

    QNetworkRequest request(url);
    auto const reply = m_networkAccessManager.get(request);

    if (!reply)
    {
        onComplete(id, reply, false);
    }

    connect(reply, &QNetworkReply::finished, this, [this, id, reply]
    {
        onComplete(id, reply, true);
    });

    connect(reply, qOverload<QNetworkReply::NetworkError>(&QNetworkReply::error), this, [this, id, reply]
            (QNetworkReply::NetworkError error)
    {
        qCCritical(QLC_DOWNLOADER) << "Error on download:"
                                   << QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(static_cast<int>(error));
        onComplete(id, reply, false);
    });
}

bool Downloader::needDeffered() const
{
    return m_inProgress >= maxParallels();
}

void Downloader::onComplete(quint64 id, QNetworkReply* reply, bool success)
{
    --m_inProgress;

    if (!reply)
    {
        qCritical(QLC_DOWNLOADER) << "Missing reply of network access manager";
        return;
    }

    if (success)
    {
        qInfo(QLC_DOWNLOADER) << reply->url() << "successfully downloaded by id:" << id;
        emit complete(id, reply->readAll());
    }
    else
    {
        qCritical(QLC_DOWNLOADER) << reply->url() << "downloading failed by id:" << id;
        emit complete(id, {});
    }

    reply->deleteLater();
    checkDefferedRequests();
}

void Downloader::checkDefferedRequests()
{
    while (!needDeffered() && !m_defferedRequests.empty())
    {
        auto request = m_defferedRequests.takeFirst();
        download(request.id, request.url);
    }
}
}
