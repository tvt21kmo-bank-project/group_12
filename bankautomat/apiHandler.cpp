#include <apiHandler.h>

QNetworkReply* ApiHandler::get(QString endPoint, QString token = NULL)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    const QUrl url(apiBaseUrl + endPoint);
    QNetworkRequest request(url);
    if(!token.isNull()){
        request.setRawHeader(QByteArray("Authorization"), QByteArray("Bearer " + token.toUtf8()));
    }
    QNetworkReply *reply = manager->get(request);
    return reply;
}

QNetworkReply* ApiHandler::post(QString endPoint, QByteArray data, QString token = NULL)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    const QUrl url(apiBaseUrl + endPoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if(!token.isNull()){
        request.setRawHeader(QByteArray("Authorization"), QByteArray("Bearer " + token.toUtf8()));
    }
    QNetworkReply *reply = manager->post(request, data);
    return reply;
}

QNetworkReply *ApiHandler::put(QString endPoint, QByteArray data, QString token)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    const QUrl url(apiBaseUrl + endPoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if(!token.isNull()){
        request.setRawHeader(QByteArray("Authorization"), QByteArray("Bearer " + token.toUtf8()));
    }
    QNetworkReply *reply = manager->put(request, data);
    return reply;
}
