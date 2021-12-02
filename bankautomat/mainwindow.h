#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <apiHandler.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonLogin_clicked();
    void on_pushButtonShowBalance_clicked();

    void on_pushButtonBackShowBalance_clicked();

    void on_pushButtonDeposit_clicked();

    void on_pushButtonBackDeposit_clicked();

    void on_pushButtonDepositDeposit_clicked();

private:
    Ui::MainWindow *ui;
    ApiHandler *apiHandler;
    const int pageIndexLogin = 0;
    const int pageIndexMain = 1;
    const int pageIndexShowBalance = 2;
    const int pageIndexDeposit = 3;
    void uiSwitchToMain();
};
#endif // MAINWINDOW_H
