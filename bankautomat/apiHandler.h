#ifndef APIHANDLER_H
#define APIHANDLER_H

#endif // APIHANDLER_H
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

    void setActiveToken(QString token);
    QString getActiveToken();

private:
    const QString apiBaseUrl = "http://localhost:3001";
    QString activeToken;

public slots:
    QNetworkReply* get(QString url, bool tokenRequired);
    QNetworkReply* post(QString url, QByteArray data, bool tokenRequired);
};
