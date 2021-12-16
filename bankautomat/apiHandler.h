#ifndef APIHANDLER_H
#define APIHANDLER_H


#include <QObject>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>


class ApiHandler : public QObject
{
    Q_OBJECT
public:
    ApiHandler() {}
    virtual ~ApiHandler() {}

private:
    const QString apiBaseUrl = "http://localhost:3001";

public slots:
    QNetworkReply* get(QString url, QString token);
    QNetworkReply* post(QString url, QByteArray data, QString token);
    QNetworkReply* put(QString url, QByteArray data, QString token);

};
#endif // APIHANDLER_H
