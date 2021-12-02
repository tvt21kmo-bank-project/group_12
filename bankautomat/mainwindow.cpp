#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(pageIndexLogin);
    apiHandler = new ApiHandler();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButtonLogin_clicked()
{
    // Build json object for request
    QString cardNumber = ui->lineEdit_cardNumber->text();
    QString cardPin = ui->lineEdit_cardPin->text();
    if(cardNumber.isEmpty() || cardPin.isEmpty()){
        ui->label_loginStatus->setText("Card number and pin code required");
    } else{
        QJsonObject obj;
        obj["cardNumber"] = cardNumber;
        obj["cardPin"] = cardPin;
        QJsonDocument doc(obj);
        QByteArray data = doc.toJson();

        QNetworkReply *response = apiHandler->post("/user/login", data, false);

        QObject::connect(response, &QNetworkReply::finished, [=](){
            if(response->error() == QNetworkReply::NoError){
                // Convert response to Json
                QString contents = QString::fromUtf8(response->readAll());
                QJsonDocument responseJson = QJsonDocument::fromJson(contents.toUtf8());

                // Set jwt token from response as active token
                QString token = responseJson.object().value("token").toString();
                apiHandler->setActiveToken(token);

                // Set username from json response to main UI
                QString username = responseJson.object().value("username").toString();
                ui->label_mainUsername->setText(username);

                // Switch UI view to main page
                ui->stackedWidget->setCurrentIndex(pageIndexMain);
            }
            else{
                int statusCode = response->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
                QString errorMsg;
                if(statusCode == 400){
                    errorMsg = "Client error, contact support";
                } else if(statusCode == 403){
                    errorMsg = "Invalid card number or pin code";
                } else if (statusCode == 500){
                    errorMsg = "Server error, try again later";
                }
                ui->label_loginStatus->setText(errorMsg);
            }
            response->deleteLater();
        });
    }
}

void MainWindow::on_pushButtonShowBalance_clicked()
{
    if(apiHandler->getActiveToken().isNull()){
        ui->stackedWidget->setCurrentIndex(0);
    } else {
        QNetworkReply *response = apiHandler->get("/event/balance", true);

        QObject::connect(response, &QNetworkReply::finished, [=](){
            if(response->error() == QNetworkReply::NoError){

                // Convert response to Json
                QString contents = QString::fromUtf8(response->readAll());
                QJsonDocument responseJson = QJsonDocument::fromJson(contents.toUtf8());

                // Get account balance from json response
                double balance = responseJson.object().value("balance").toDouble();
                QString owner = responseJson.object().value("owner").toString();

                ui->label_showBalanceBalance->setText(QString::number(balance, 'f', 2));
                ui->label_showBalanceOwner->setText(owner);

                // Switch to show balance UI
                ui->stackedWidget->setCurrentIndex(pageIndexShowBalance);
            }
            else{
                int statusCode = response->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                if(statusCode == 401){
                    apiHandler->setActiveToken(NULL);
                    ui->label_loginStatus->setText("Session expired, please login again");
                    ui->stackedWidget->setCurrentIndex(pageIndexLogin);
                }
            }
            response->deleteLater();
        });
    }
}


void MainWindow::on_pushButtonBackShowBalance_clicked()
{
    this->uiSwitchToMain();
}

void MainWindow::uiSwitchToMain(){
    ui->stackedWidget->setCurrentIndex(pageIndexMain);
}


void MainWindow::on_pushButtonDeposit_clicked()
{
    ui->stackedWidget->setCurrentIndex(pageIndexDeposit);
}


void MainWindow::on_pushButtonBackDeposit_clicked()
{
    this->uiSwitchToMain();
}


void MainWindow::on_pushButtonDepositDeposit_clicked()
{
    QString amount = ui->lineEdit_depositAmount->text();

    QJsonObject obj;
    obj["amount"] = amount;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    QNetworkReply *response = apiHandler->post("/event/deposit", data, true);

    QObject::connect(response, &QNetworkReply::finished, [=](){
        if(response->error() == QNetworkReply::NoError){
            ui->lineEdit_depositAmount->setText("");
            ui->label_depositStatus->setText("Deposit completed");
        }
        else{
            int statusCode = response->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
            QString errorMsg;
            if(statusCode == 400){
                errorMsg = "Client error, contact support";
            } else if (statusCode == 500){
                errorMsg = "Server error, try again later";
            }
            ui->label_loginStatus->setText(errorMsg);
        }
        response->deleteLater();
    });
}

