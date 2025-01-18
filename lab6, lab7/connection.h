#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QString>
#include <QPointer>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = nullptr);

private:
    QNetworkAccessManager manager;

private slots:
    void recievedReply(QPointer<QNetworkReply> reply);

signals:
    void RecievedString(QString string);

public:
    void GetRequest(const QString ip, const QString path);
};

#endif
