#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>
#include <QNetworkReply>
#include <QJsonArray>
#include <apiHandler.h>

class SessionManager : public QObject{
    Q_OBJECT
public:
    SessionManager();
    virtual ~SessionManager() {};

    QString getToken();

    void login(QByteArray data);
    void logout();

    void loadViewWithdraw();
    void loadViewDeposit();
    void loadViewTransactions();
    void loadViewBalance();

    void loadActionWithdraw(double amount);
    void loadActionDeposit(double amount);

    void setCardType(int cardType);

    // User information
    QString userName;
    QString userAddress;
    QString userPhone;
    QString ownerName;
    QString ownerAddress;
    QString ownerPhone;
    QString currentBalance;
    QString currentCreditLimit;
    QJsonArray currentTransactions;
    int cardId;
    int accountId;
    int cardType;
    int sessionCardType;
    int activeTransactionsPage;

signals:
    // Session handling
    void signalLogin();
    void signalLoginError(QString message);
    void signalLogout(QString message);
    void signalSelectCardType();
    // Views
    void signalLoadViewWithdraw();
    void signalLoadViewDeposit();
    void signalLoadViewBalance();
    void signalLoadViewTransactions();
    // Actions
    void signalUpdateWithdrawStatus(QString amount, QString message);
    void signalUpdateDepositStatus(QString amount, QString message);

private:
    ApiHandler *apiHandler;
    QString token;
    void selectCardType();
};

#endif // SESSIONMANAGER_H
