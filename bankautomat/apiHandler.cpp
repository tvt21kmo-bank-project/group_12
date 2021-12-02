#include <apiHandler.h>

void ApiHandler::setActiveToken(QString token){
    this->activeToken = token;
}

QString ApiHandler::getActiveToken(){
    return this->activeToken;
}

QNetworkReply* ApiHandler::get(QString endPoint, bool tokenRequired)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    const QUrl url(apiBaseUrl + endPoint);
    QNetworkRequest request(url);
    if(tokenRequired == true){
        request.setRawHeader(QByteArray("Authorization"), QByteArray("Bearer " + this->activeToken.toUtf8()));
    }
    QNetworkReply *reply = manager->get(request);
    return reply;
}

QNetworkReply* ApiHandler::post(QString endPoint, QByteArray data, bool tokenRequired)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    const QUrl url(apiBaseUrl + endPoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if(tokenRequired == true){
        request.setRawHeader(QByteArray("Authorization"), QByteArray("Bearer " + this->activeToken.toUtf8()));
    }
    QNetworkReply *reply = manager->post(request, data);
    return reply;
}
