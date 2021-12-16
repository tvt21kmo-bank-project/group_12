#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QJsonArray>
#include <QTimer>
#include <apiHandler.h>
#include <sessionmanager.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public slots:
    void sessionTimeout();
    void closeView();
    void uiSwitchToLogin(QString message);
    void uiSwitchToMain();
    void uiSwitchToSelectCardType();
    void uiSwitchToWithdraw();
    void uiSwitchToDeposit();
    void uiSwitchToBalance();
    void uiSwitchToTransactions();
    void uiUpdateWithdrawStatus(QString amount, QString message);
    void uiUpdateDepositStatus(QString amount, QString message);

private slots:
    // Login UI actions
    void on_pushButtonLogin_clicked();

    // Select card type UI Actions
    void on_pushButtonSelectCredit_clicked();
    void on_pushButtonSelectDebit_clicked();

    // Main UI actions
    void on_pushButton_main_withdraw_clicked();
    void on_pushButton_main_transactions_clicked();
    void on_pushButton_main_balance_clicked();
    void on_pushButtonDeposit_clicked();
    void on_pushButtonLogout_clicked();

    // Withdraw UI actions
    void on_pushButton_withdraw_withdraw20_clicked();
    void on_pushButton_withdraw_withdraw40_clicked();
    void on_pushButton_withdraw_withdraw60_clicked();
    void on_pushButton_withdraw_withdraw80_clicked();
    void on_pushButton_withdraw_withdraw100_clicked();
    void on_pushButton_withdraw_withdraw200_clicked();
    void on_pushButton_withdraw_withdrawCustom_clicked();
    void on_pushButton_withdraw_back_clicked();

    // Deposit UI actions
    void on_pushButton_deposit_depositCustom_clicked();
    void on_pushButton_deposit_deposit20_clicked();
    void on_pushButton_deposit_deposit40_clicked();
    void on_pushButton_deposit_deposit660_clicked();
    void on_pushButton_deposit_deposit80_clicked();
    void on_pushButton_deposit_deposit100_clicked();
    void on_pushButton_deposit_deposit200_clicked();
    void on_pushButtonBackDeposit_clicked();

    // Balance UI actions
    void on_pushButton_balance_back_clicked();

    // Transactions UI actions
    void on_pushButton_transactions_previous_clicked();
    void on_pushButton_transactions_next_clicked();
    void on_pushButton_transactions_back_clicked();



private:
    Ui::MainWindow *ui;
    ApiHandler *apiHandler;
    SessionManager *sessionManager;
    const int pageIndexLogin = 0;
    const int pageIndexCardType = 1;
    const int pageIndexMain = 2;
    const int pageIndexBalance = 3;
    const int pageIndexDeposit = 4;
    const int pageIndexWithdraw = 6;
    const int pageIndexTransactions = 5;
    QTimer logout_timer;
    QTimer close_view_timer;
    bool eventFilter(QObject *object, QEvent *event);
};
#endif // MAINWINDOW_H
