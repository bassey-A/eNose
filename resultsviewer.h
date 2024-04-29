#ifndef RESULTSVIEWER_H
#define RESULTSVIEWER_H

#include <QObject>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyService>
#include <QLowEnergyController>
#include <QBluetoothServiceInfo>
#include <QtEndian>
#include <QDataStream>
#include <QTableView>
#include <QStandardItemModel>

typedef struct {
    uint64_t timeUnits;
    uint16_t tgs2600;
    uint16_t tgs2602;
    uint16_t tgs2603;
    uint16_t tgs2610;
    uint16_t tgs2611;
    uint16_t tgs2620;
    uint16_t mq3;
    uint16_t mq7;
    uint16_t mq135;
} sensorReadings;

class ResultsViewer : public QObject
{
    Q_OBJECT
public:
    explicit ResultsViewer(QBluetoothDeviceInfo *dev, QObject *parent = nullptr);

public slots:
    void attemptConnection();
    void connectSuccess();
    void servicesDiscovered();
    void serviceDetailsDiscovered();
    void startRun();
    void processReadings();
    void reset();
    void stopReadings();
    /*void charRead();
    void charWritten();
    void errorRorW();
    void sensorUpdate();
    void serviceDetailsDiscovered();
    void continueRead();
    void serviceError();
    void confirmDescription();
    void runButtonClicked();
    void spinUpThread();
    void timer();
    void readButtonClicked();*/

private:
    QBluetoothDeviceInfo *device;
    QLowEnergyController *leController = nullptr;
    QLowEnergyService *ledService = nullptr;
    QLowEnergyCharacteristic *readNose;
    QLowEnergyCharacteristic *writeNose;
    QLowEnergyService *service = nullptr;
    QList<QLowEnergyService> myServices;
    QList<QLowEnergyCharacteristic> myCharacteristics;
    QList<QBluetoothUuid> uuids;
    QByteArray readArray;
    QByteArray writeArray;
    QLowEnergyDescriptor descriptor;
    sensorReadings sample;
    sensorReadings peakValues;
    sensorReadings peakTimes;
    QStandardItemModel *model;

signals:
    void discovering();
    void subscribedToNose();
    void readingEnded();
    void readingsProcessed();

friend class Homepage;
};

#endif // RESULTSVIEWER_H
