#include "connection.h"

Connection::Connection(QObject *parent)
    : QObject{parent}
{
    connect(&manager, &QNetworkAccessManager::finished, this, &Connection::recievedReply);
}

void Connection::recievedReply(QPointer<QNetworkReply> reply)
{
    if (!reply->error())
    {
        emit RecievedString(QString(reply->readAll()));
    }
    else
    {
        qDebug() << reply->errorString();
    }
}

void Connection::GetRequest(const QString ip, const QString path)
{
    QUrl url = "http://"+ip+path;
    auto request = QNetworkRequest();
    request.setUrl(url);
    manager.get(request);
}
