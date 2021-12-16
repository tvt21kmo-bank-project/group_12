#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QObject>

class ViewManager : public QObject
{
    Q_OBJECT
public:
    explicit ViewManager(QObject *parent = nullptr);

    //void loadViewDeposit();

signals:

};

#endif // VIEWMANAGER_H
