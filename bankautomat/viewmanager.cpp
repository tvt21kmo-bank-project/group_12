#include "viewmanager.h"

/*
ViewManager::ViewManager(QObject *parent) : QObject(parent)
{

}

void ViewManager::loadViewDeposit(QString token)
{
    QNetworkReply *response = apiHandler->get("/event/balance", token);

    QObject::connect(response, &QNetworkReply::finished, [=](){
        if(response->error() == QNetworkReply::NoError){
            // Initialize Show Balance view
            ui->listWidgetShowBalance->clear();

            // Convert response to Json
            QString contents = QString::fromUtf8(response->readAll());
            QJsonDocument responseJson = QJsonDocument::fromJson(contents.toUtf8());

            // Get account balance from json response
            double balance = responseJson.object().value("balance").toDouble();
            QString owner = responseJson.object().value("owner").toString();

            // Get transaction history
            QJsonArray transactions = responseJson.object().value("transactions").toArray();
            foreach (const QJsonValue & value, transactions) {
                QJsonObject obj = value.toObject();
                QString row = "";
                double amount = obj.value("amount").toDouble();
                row = row + QString::number(amount, 'f', 2);
                int transType = obj.value("type").toInt();
                if(transType == 1){
                    row = row + " / Deposit";
                } else if (transType == 2){
                    row = row + " / Withdraw";
                }
                QString timestamp = obj.value("timestamp").toString();
                row = row + " / " + timestamp;

                ui->listWidgetShowBalance->addItem(row);
            }

            ui->label_showBalanceBalance->setText(QString::number(balance, 'f', 2));
            ui->label_showBalanceOwner->setText(owner);

            // Switch to show balance UI
            ui->stackedWidget->setCurrentIndex(pageIndexShowBalance);
        }
        else{
            int statusCode = response->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if(statusCode == 401){
                //apiHandler->setActiveToken(NULL);

                ui->label_loginStatus->setText("Session expired, please login again");
                ui->stackedWidget->setCurrentIndex(pageIndexLogin);
            }
        }
        response->deleteLater();
    });
}
*/
