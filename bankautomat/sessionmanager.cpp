#include "sessionmanager.h"

SessionManager::SessionManager()
{
    apiHandler = new ApiHandler();
}

QString SessionManager::getToken()
{
    return this->token;
}

void SessionManager::login(QByteArray data){
    QNetworkReply *response = apiHandler->post("/user/login", data, NULL);

    QObject::connect(response, &QNetworkReply::finished, [=](){
        if(response->error() == QNetworkReply::NoError){
            // Convert response to Json
            QString contents = QString::fromUtf8(response->readAll());
            QJsonDocument responseJson = QJsonDocument::fromJson(contents.toUtf8());
            qInfo() << responseJson;
            // Set jwt token from response as active token
            this->token = responseJson.object().value("token").toString();
            // Get user/account/card information
            this->userName = responseJson.object().value("userName").toString();
            this->userAddress = responseJson.object().value("userAddress").toString();
            this->userPhone = responseJson.object().value("userPhone").toString();
            this->ownerName = responseJson.object().value("ownerName").toString();
            this->accountId = responseJson.object().value("accountId").toInt();

            this->currentCreditLimit = QString::number(responseJson.object().value("creditLimit").toDouble(), 'f', 2);
            qInfo() << currentCreditLimit;
            int cardType = responseJson.object().value("cardType").toInt();
            if(cardType == 1){
                this->sessionCardType = 1;
                emit signalLogin();
            } else if (cardType == 2) {
                emit signalSelectCardType();
            }
        }
        else{
            // Parse status code from response
            int statusCode = response->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
            QString errorMsg;
            if(statusCode == 400){
                errorMsg = "Client error, contact support";
            } else if(statusCode == 403){
                errorMsg = "Invalid card id or pin code";
            } else if (statusCode == 500){
                errorMsg = "Server error, try again later";
            } else {
                errorMsg = "Cannot connect to server";
            }
            emit signalLoginError(errorMsg);
        }
        response->deleteLater();
    });
}

void SessionManager::logout(){
    // Clear session token
    this->token = NULL;

    // Send logout signal to other classes
    emit signalLogout("");
}

void SessionManager::loadViewWithdraw()
{
    QNetworkReply *response = apiHandler->get("/event/balance", this->getToken());
    QObject::connect(response, &QNetworkReply::finished, [=](){
        if(response->error() == QNetworkReply::NoError){
            // Convert response to Json
            QString contents = QString::fromUtf8(response->readAll());
            QJsonDocument responseJson = QJsonDocument::fromJson(contents.toUtf8());

            QString balance = QString::number(responseJson.object().value("balance").toDouble(), 'f', 2);
            //QString creditLimit = QString::number(responseJson.object().value("creditLimit").toDouble(), 'f', 2);

            this->currentBalance = balance;
            //this->currentCreditLimit = creditLimit;

            emit signalLoadViewWithdraw();
        }
        else{
            qInfo() << response->error();
            // Parse status code from response
            int statusCode = response->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
            QString errorMsg;
            if(statusCode == 400){
                errorMsg = "Client error, contact support";
            } else if (statusCode == 500){
                errorMsg = "Server error, try again later";
            } else {
                errorMsg = "Cannot connect to server";
            }
            emit signalLoginError(errorMsg);
        }
        response->deleteLater();
    });
}

void SessionManager::loadViewDeposit()
{
    QNetworkReply *response = apiHandler->get("/event/balance", this->getToken());
    QObject::connect(response, &QNetworkReply::finished, [=](){
        if(response->error() == QNetworkReply::NoError){
            // Convert response to Json
            QString contents = QString::fromUtf8(response->readAll());
            QJsonDocument responseJson = QJsonDocument::fromJson(contents.toUtf8());

            QString balance = QString::number(responseJson.object().value("balance").toDouble(), 'f', 2);
            //QString creditLimit = QString::number(responseJson.object().value("creditLimit").toDouble(), 'f', 2);

            this->currentBalance = balance;
            //this->currentCreditLimit = creditLimit;

            emit signalLoadViewDeposit();
        }
        else {
            QString errorMsg;
            // Parse status code from response
            int statusCode = response->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
            if(statusCode == 400){
                errorMsg = "Client error, contact support";
            } else if (statusCode == 401){
                errorMsg = "Session expired, log in again";
            } else if (statusCode == 500){
                errorMsg = "Server error, try again later";
            } else {
                errorMsg = "Cannot connect to server";
            }
            emit signalLoginError(errorMsg);
        }
        response->deleteLater();
    });
}

void SessionManager::loadViewTransactions()
{
    // Build json object for request
    QJsonObject obj;
    obj["page"] = this->activeTransactionsPage;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    QNetworkReply *response = apiHandler->post("/event/transactions", data, this->getToken());
    QObject::connect(response, &QNetworkReply::finished, [=](){
        if(response->error() == QNetworkReply::NoError){
            // Convert response to Json
            QString contents = QString::fromUtf8(response->readAll());
            QJsonDocument responseJson = QJsonDocument::fromJson(contents.toUtf8());
            // Get data from json
            this->currentBalance = QString::number(responseJson.object().value("balance").toDouble(), 'f', 2);
            //this->currentCreditLimit = QString::number(responseJson.object().value("creditLimit").toDouble(), 'f', 2);
            this->currentTransactions = responseJson.object().value("transactions").toArray();
            emit signalLoadViewTransactions();
        } else {
            qInfo() << response->error();
            QString errorMsg;
            int statusCode = response->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if(statusCode == 400){
                errorMsg = "Client error";
            } else if(statusCode == 401){
                errorMsg = "Session expired";
            } else if(statusCode == 500){
                errorMsg = "Server error";
            } else {
                errorMsg = "Cannot connect to server";
            }
            emit signalLoginError(errorMsg);
        }
        response->deleteLater();
    });
}

void SessionManager::loadViewBalance()
{
    QNetworkReply *response = apiHandler->get("/event/exbalance", this->getToken());
    QObject::connect(response, &QNetworkReply::finished, [=](){
        if(response->error() == QNetworkReply::NoError){
            // Convert response to Json
            QString contents = QString::fromUtf8(response->readAll());
            QJsonDocument responseJson = QJsonDocument::fromJson(contents.toUtf8());
            // Get data from json
            this->currentBalance = QString::number(responseJson.object().value("balance").toDouble(), 'f', 2);
            //this->currentCreditLimit = QString::number(responseJson.object().value("creditLimit").toDouble(), 'f', 2);
            this->currentTransactions = responseJson.object().value("transactions").toArray();
            emit signalLoadViewBalance();
            }
        else{
            qInfo() << response->error();
            QString errorMsg;
            int statusCode = response->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if(statusCode == 400){
                errorMsg = "Client error";
            } else if(statusCode == 401){
                errorMsg = "Session expired";
            } else if(statusCode == 500){
                errorMsg = "Server error";
            } else {
                errorMsg = "Cannot connect to server";
            }
            emit signalLoginError(errorMsg);
        }
        response->deleteLater();
    });
}

void SessionManager::loadActionWithdraw(double amount)
{
    // Build json object for request
    QJsonObject obj;
    obj["amount"] = amount;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    // Send request to API
    QNetworkReply *response = apiHandler->post("/event/withdraw", data, this->getToken());
    // Handle API response
    QObject::connect(response, &QNetworkReply::finished, [=](){
        if(response->error() == QNetworkReply::NoError){
            // Convert response to Json
            QString contents = QString::fromUtf8(response->readAll());
            QJsonDocument responseJson = QJsonDocument::fromJson(contents.toUtf8());
            QString newBalance = QString::number(responseJson.object().value("newBalance").toDouble(), 'f', 2);
            // Show status message in Withdraw view
            emit signalUpdateWithdrawStatus(newBalance, "Withdraw succesful");
        }
        else {
            QString errorMsg;
            // Parse status code from response
            int statusCode = response->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
            if(statusCode == 400 || statusCode == 401 || statusCode == 500){
                if(statusCode == 400){
                    errorMsg = "Client error, contact support";
                } else if(statusCode == 401){
                    errorMsg = "Session expired";
                } else if(statusCode == 500){
                    errorMsg = "Server error, try again later";
                }
                // If any of these status codes, logout and switch to login screen
                emit signalLoginError(errorMsg);
            }  else if(statusCode == 460 || statusCode == 461){
                if(statusCode == 460){
                    errorMsg = "Not enough balance";
                } else if (statusCode == 461){
                    errorMsg = "Not enough credit";
                }
                // If any of these status codes, show error status on Withdraw view
                emit signalUpdateWithdrawStatus(NULL, errorMsg);
            }
        }
        response->deleteLater();
    });
}

void SessionManager::loadActionDeposit(double amount)
{
    // Build json object for request
    QJsonObject obj;
    obj["amount"] = amount;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    // Send request to API
    QNetworkReply *response = apiHandler->post("/event/deposit", data, this->getToken());
    // Handle API response
    QObject::connect(response, &QNetworkReply::finished, [=](){
        if(response->error() == QNetworkReply::NoError){
            // Convert response to Json
            QString contents = QString::fromUtf8(response->readAll());
            QJsonDocument responseJson = QJsonDocument::fromJson(contents.toUtf8());
            QString newBalance = QString::number(responseJson.object().value("newBalance").toDouble(), 'f', 2);
            // Show status message in Withdraw view
            emit signalUpdateDepositStatus(newBalance, "Deposit succesful");
        }
        else {
            QString errorMsg;
            // Parse status code from response
            int statusCode = response->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
            if(statusCode == 400 || statusCode == 401 || statusCode == 500){
                if(statusCode == 400){
                    errorMsg = "Client error, contact support";
                } else if(statusCode == 401){
                    errorMsg = "Session expired";
                } else if(statusCode == 500){
                    errorMsg = "Server error, try again later";
                }
            } else {
                errorMsg = "Unknown error";
            }
            // If any of these status codes, logout and switch to login screen
            emit signalLoginError(errorMsg);
        }
        response->deleteLater();
    });
}

void SessionManager::setCardType(int cardType)
{
    // Build json object
    QJsonObject obj;
    obj["sessionCardType"] = cardType;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    QNetworkReply *response = apiHandler->put("/user/login", data, this->getToken());
    QObject::connect(response, &QNetworkReply::finished, [=](){
        if(response->error() == QNetworkReply::NoError){
            // Convert response to Json
            QString contents = QString::fromUtf8(response->readAll());
            QJsonDocument responseJson = QJsonDocument::fromJson(contents.toUtf8());
            // Set jwt token from response as active token
            QString token = responseJson.object().value("updatedToken").toString();
            this->token = token;
            this->sessionCardType = cardType;
            emit signalLogin();
        }
        else{
            qInfo() << response->error();
            // Parse status code from response
            int statusCode = response->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
            QString errorMsg;
            if(statusCode == 400){
                errorMsg = "Client error, contact support";
            } else if(statusCode == 403){
                errorMsg = "Invalid card id or pin code";
            } else if (statusCode == 500){
                errorMsg = "Server error, try again later";
            } else {
                errorMsg = "Cannot connect to server";
            }
            emit signalLoginError(errorMsg);
        }
        response->deleteLater();
    });
}
