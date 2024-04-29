#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "authhandler.h"
#include "resultsviewer.h"

#include <QMainWindow>
#include <QDebug>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QLowEnergyController>
#include <QTimer>
#include <QtConcurrent/QtConcurrentRun>
#include <QThread>
#include <QThreadPool>
#include <QFile>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui {
class Homepage;
}
QT_END_NAMESPACE

class Homepage : public QMainWindow
{
    Q_OBJECT

public:
    Homepage(QWidget *parent = nullptr);
    ~Homepage();

public slots:
    void signIn();
    void signInSuccess();
    void foundDevice(QBluetoothDeviceInfo dev);
    void btScanComplete();
    void discoveringServices();
    void readyToRun();
    void startRun();
    void resetNose();
    void finishedRead();
    void createDbEntry();
    void readingsProcessed();

private slots:
    QString fetchKey();

signals:
    void run();
    void reset();

private:
    Ui::Homepage *ui;
    AuthHandler *authhandler;
    ResultsViewer *viewer;
    QBluetoothDeviceDiscoveryAgent *disco;
    QBluetoothDeviceInfo *device;
    QLowEnergyController *leController;
    int signUserUP {0};
    QString email;
    QString password;
    QThreadPool pool;
};
#endif // HOMEPAGE_H
