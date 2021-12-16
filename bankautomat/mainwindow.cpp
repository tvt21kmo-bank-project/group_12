#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(pageIndexLogin);

    apiHandler = new ApiHandler();
    sessionManager = new SessionManager();

    // Init signals
    QObject::connect(sessionManager, &SessionManager::signalLogin, this, &MainWindow::uiSwitchToMain);
    QObject::connect(sessionManager, &SessionManager::signalLogout, this, &MainWindow::uiSwitchToLogin);
    QObject::connect(sessionManager, &SessionManager::signalLoginError, this, &MainWindow::uiSwitchToLogin);
    QObject::connect(sessionManager, &SessionManager::signalSelectCardType, this, &MainWindow::uiSwitchToSelectCardType);

    QObject::connect(sessionManager, &SessionManager::signalLoadViewWithdraw, this, &MainWindow::uiSwitchToWithdraw);
    QObject::connect(sessionManager, &SessionManager::signalLoadViewDeposit, this, &MainWindow::uiSwitchToDeposit);
    QObject::connect(sessionManager, &SessionManager::signalLoadViewBalance, this, &MainWindow::uiSwitchToBalance);
    QObject::connect(sessionManager, &SessionManager::signalLoadViewTransactions, this, &MainWindow::uiSwitchToTransactions);

    QObject::connect(sessionManager, &SessionManager::signalUpdateWithdrawStatus, this, &MainWindow::uiUpdateWithdrawStatus);
    QObject::connect(sessionManager, &SessionManager::signalUpdateDepositStatus, this, &MainWindow::uiUpdateDepositStatus);

    // Install event filter for session logout timer
    qApp->installEventFilter(this);
    logout_timer.setInterval(30000);
    close_view_timer.setInterval(10000);
    QObject::connect(&logout_timer, &QTimer::timeout, this, &MainWindow::sessionTimeout);
    QObject::connect(&close_view_timer, &QTimer::timeout, this, &MainWindow::closeView);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::KeyPress) {
        logout_timer.stop();
        close_view_timer.stop();
        logout_timer.start();
        close_view_timer.start();
    }
    return QWidget::eventFilter(object,event);
}

void MainWindow::sessionTimeout()
{
    logout_timer.stop();
    this->uiSwitchToLogin("Session timeout");
}

void MainWindow::closeView()
{
    if(
            ui->stackedWidget->currentIndex() == pageIndexBalance ||
            ui->stackedWidget->currentIndex() == pageIndexDeposit ||
            ui->stackedWidget->currentIndex() == pageIndexTransactions ||
            ui->stackedWidget->currentIndex() == pageIndexWithdraw
    ){
        close_view_timer.stop();
        this->uiSwitchToMain();
    }
}


void MainWindow::on_pushButtonLogin_clicked()
{
    // Get card id and pincode from UI inputs
    QString cardId = ui->lineEdit_cardNumber->text();
    QString cardPin = ui->lineEdit_cardPin->text();
    if(cardId.isEmpty() || cardPin.isEmpty()){
        ui->label_loginStatus->setText("Card id and pin code required");
    } else {
        // Build json object
        QJsonObject obj;
        obj["cardId"] = cardId;
        obj["cardPin"] = cardPin;
        QJsonDocument doc(obj);
        QByteArray data = doc.toJson();
        // Login user
        sessionManager->login(data);
        logout_timer.start();
    }
}

// Layout switches

void MainWindow::uiSwitchToMain(){
    QString cardType;
    if(sessionManager->sessionCardType == 1){
        cardType = "Debit";
    } else if (sessionManager->sessionCardType == 2){
        cardType = "Credit";
    }
    ui->label_main_cardType->setText(cardType);
    //ui->label_main_creditLimit->setText(sessionManager->currentCreditLimit);
    if(sessionManager->sessionCardType == 2){
        // Show credit limit in view if current sessions card type is credit
        ui->label_main_creditLimitTitle->setVisible(1);
        ui->label_main_creditLimit->setText(sessionManager->currentCreditLimit);
    } else {
        // Hide credit limit from view if current sessions card type is debit
        ui->label_main_creditLimitTitle->setVisible(0);
        ui->label_main_creditLimit->setVisible(0);
    }
    ui->label_main_owner->setText(sessionManager->ownerName);
    ui->label_mainUsername->setText(sessionManager->userName);
    ui->stackedWidget->setCurrentIndex(pageIndexMain);
}

void MainWindow::uiSwitchToSelectCardType()
{
    ui->stackedWidget->setCurrentIndex(pageIndexCardType);
}

void MainWindow::uiSwitchToWithdraw()
{
    QString cardType;
    if(sessionManager->sessionCardType == 1){
        cardType = "Debit";
    } else if (sessionManager->sessionCardType == 2){
        cardType = "Credit";
    }
    if(sessionManager->sessionCardType == 2){
        // Show credit limit in view if current sessions card type is credit
        ui->label_withdraw_creditLimitTitle->setVisible(1);
        ui->label_withdraw_creditLimit->setText(sessionManager->currentCreditLimit);
    } else {
        // Hide credit limit from view if current sessions card type is debit
        ui->label_withdraw_creditLimitTitle->setVisible(0);
        ui->label_withdraw_creditLimit->setVisible(0);
    }
    ui->label_withdraw_cardType->setText(cardType);
    ui->label_withdraw_balance->setText(sessionManager->currentBalance);
    close_view_timer.start();
    ui->stackedWidget->setCurrentIndex(pageIndexWithdraw);
}

void MainWindow::uiSwitchToDeposit()
{
    QString cardType;
    if(sessionManager->sessionCardType == 1){
        cardType = "Debit";
    } else if (sessionManager->sessionCardType == 2){
        cardType = "Credit";
    }
    if(sessionManager->sessionCardType == 2){
        // Show credit limit in view if current sessions card type is credit
        ui->label_deposit_creditLimitTitle->setVisible(1);
        ui->label_deposit_creditLimit->setText(sessionManager->currentCreditLimit);
    } else {
        // Hide credit limit from view if current sessions card type is debit
        ui->label_deposit_creditLimitTitle->setVisible(0);
        ui->label_deposit_creditLimit->setVisible(0);
    }
    ui->label_deposit_cardType->setText(cardType);
    ui->label_deposit_balance->setText(sessionManager->currentBalance);
    close_view_timer.start();
    ui->stackedWidget->setCurrentIndex(pageIndexDeposit);
}

void MainWindow::uiSwitchToBalance()
{
    ui->listWidget_balance_transactions->clear();
    if(!sessionManager->currentTransactions.isEmpty()){
        foreach (const QJsonValue & value, sessionManager->currentTransactions) {
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

            ui->listWidget_balance_transactions->addItem(row);
        }
    }
    QString cardType;
    if(sessionManager->sessionCardType == 1){
        cardType = "Debit";
    } else if (sessionManager->sessionCardType == 2){
        cardType = "Credit";
    }
    ui->label_balance_cardType->setText(cardType);
    if(sessionManager->sessionCardType == 2){
        // Show credit limit in view if current sessions card type is credit
        ui->label_balance_creditLimitTitle->setVisible(1);
        ui->label_balance_creditLimit->setText(sessionManager->currentCreditLimit);
    } else {
        // Hide credit limit from view if current sessions card type is debit
        ui->label_balance_creditLimitTitle->setVisible(0);
        ui->label_balance_creditLimit->setVisible(0);
    }
    ui->label_balance_ownerName->setText(sessionManager->ownerName);
    ui->label_balance_balance->setText(sessionManager->currentBalance);
    close_view_timer.start();
    ui->stackedWidget->setCurrentIndex(pageIndexBalance);
}

void MainWindow::uiSwitchToTransactions()
{
    ui->listWidget_transactions->clear();
    qInfo() << sessionManager->currentTransactions;
    if(!sessionManager->currentTransactions.isEmpty()){
        foreach (const QJsonValue & value, sessionManager->currentTransactions) {
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

            ui->listWidget_transactions->addItem(row);
        }
    }
    close_view_timer.start();
    ui->stackedWidget->setCurrentIndex(pageIndexTransactions);
}

void MainWindow::uiUpdateWithdrawStatus(QString amount, QString message)
{
    if(!amount.isNull()){
            ui->label_withdraw_balance->setText(amount);
    }
    ui->label_withdraw_status->setText(message);
}

void MainWindow::uiUpdateDepositStatus(QString amount, QString message)
{
    if(!amount.isNull()){
            ui->label_deposit_balance->setText(amount);
    }
    ui->label_deposit_status->setText(message);
}

void MainWindow::uiSwitchToLogin(QString message){
    ui->lineEdit_cardNumber->clear();
    ui->lineEdit_cardPin->clear();
    ui->label_loginStatus->setText(message);
    ui->stackedWidget->setCurrentIndex(pageIndexLogin);
}


// UI switch button clicks
void MainWindow::on_pushButtonDeposit_clicked()
{
    sessionManager->loadViewDeposit();
}

void MainWindow::on_pushButtonBackDeposit_clicked()
{
    this->uiSwitchToMain();
}


// UI event buttons clicked

void MainWindow::on_pushButtonLogout_clicked()
{
    sessionManager->logout();
}


void MainWindow::on_pushButtonSelectCredit_clicked()
{
    sessionManager->setCardType(1);
}


void MainWindow::on_pushButtonSelectDebit_clicked()
{
    sessionManager->setCardType(2);
}


void MainWindow::on_pushButton_main_withdraw_clicked()
{
    sessionManager->loadViewWithdraw();
}


void MainWindow::on_pushButton_withdraw_withdraw20_clicked()
{
    sessionManager->loadActionWithdraw(20.00);
}


void MainWindow::on_pushButton_withdraw_withdraw40_clicked()
{
    sessionManager->loadActionWithdraw(40.00);
}


void MainWindow::on_pushButton_withdraw_withdraw60_clicked()
{
    sessionManager->loadActionWithdraw(60.00);
}


void MainWindow::on_pushButton_withdraw_withdraw80_clicked()
{
    sessionManager->loadActionWithdraw(80.00);
}


void MainWindow::on_pushButton_withdraw_withdraw100_clicked()
{
    sessionManager->loadActionWithdraw(100.00);
}


void MainWindow::on_pushButton_withdraw_withdraw200_clicked()
{
    sessionManager->loadActionWithdraw(200.00);
}


void MainWindow::on_pushButton_withdraw_withdrawCustom_clicked()
{
    double amount = ui->lineEdit_withdraw_withdrawCustom->text().toDouble();
    sessionManager->loadActionWithdraw(amount);
}


void MainWindow::on_pushButton_withdraw_back_clicked()
{
    this->uiSwitchToMain();
}

// Deposit UI buttons
void MainWindow::on_pushButton_deposit_depositCustom_clicked()
{
    double amount = ui->lineEdit_deposit_amount->text().toDouble();
    sessionManager->loadActionDeposit(amount);
}

// Main UI buttons
void MainWindow::on_pushButton_main_balance_clicked()
{
    sessionManager->loadViewBalance();
}


void MainWindow::on_pushButton_balance_back_clicked()
{
    this->uiSwitchToMain();
}


void MainWindow::on_pushButton_main_transactions_clicked()
{
    sessionManager->activeTransactionsPage = 1;
    sessionManager->loadViewTransactions();
}


void MainWindow::on_pushButton_transactions_previous_clicked()
{
    if(sessionManager->activeTransactionsPage > 1){
        sessionManager->activeTransactionsPage = sessionManager->activeTransactionsPage - 1;
    } else {
        sessionManager->activeTransactionsPage = 1;
    }
    sessionManager->loadViewTransactions();
}


void MainWindow::on_pushButton_transactions_next_clicked()
{
    sessionManager->activeTransactionsPage = sessionManager->activeTransactionsPage + 1;
    sessionManager->loadViewTransactions();
}


void MainWindow::on_pushButton_transactions_back_clicked()
{
    this->uiSwitchToMain();
}




void MainWindow::on_pushButton_deposit_deposit20_clicked()
{
    sessionManager->loadActionDeposit(20.00);
}


void MainWindow::on_pushButton_deposit_deposit40_clicked()
{
    sessionManager->loadActionDeposit(40.00);
}


void MainWindow::on_pushButton_deposit_deposit660_clicked()
{
    sessionManager->loadActionDeposit(60.00);
}


void MainWindow::on_pushButton_deposit_deposit80_clicked()
{
    sessionManager->loadActionDeposit(80.00);
}


void MainWindow::on_pushButton_deposit_deposit100_clicked()
{
    sessionManager->loadActionDeposit(100.00);
}


void MainWindow::on_pushButton_deposit_deposit200_clicked()
{
    sessionManager->loadActionDeposit(200.00);
}

