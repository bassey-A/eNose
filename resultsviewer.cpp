#include "resultsviewer.h"

#include <QThread>

ResultsViewer::ResultsViewer(QBluetoothDeviceInfo *dev, QObject *parent)
    : QObject{parent}
{
    device = dev;
    model = new QStandardItemModel(0, 9);
    leController = QLowEnergyController::createCentral(*device, this);
    connect(leController, SIGNAL(connected()), this, SLOT(connectSuccess()));
    connect(leController, SIGNAL(discoveryFinished()), this, SLOT(servicesDiscovered()));
    sample.timeUnits = 0;
    peakValues.tgs2600 = 0; peakValues.tgs2602 = 0; peakValues.tgs2603 = 0; peakValues.tgs2610 = 0;
    peakValues.tgs2611 = 0; peakValues.tgs2620 = 0; peakValues.mq3 = 0; peakValues.mq7 = 0; peakValues.mq135 = 0;
    attemptConnection();
}

void ResultsViewer::attemptConnection()
{
    leController->connectToDevice();
}

void ResultsViewer::connectSuccess()
{
    leController->discoverServices();
}

void ResultsViewer::servicesDiscovered()
{
    for (QBluetoothUuid _ : leController->services()){
        uuids.append(_);
    }
    qDebug() << uuids;
    QBluetoothUuid uuid = uuids.first();
    service = leController->createServiceObject(uuid);
    connect(service, &QLowEnergyService::stateChanged, this, &ResultsViewer::serviceDetailsDiscovered);
    service->discoverDetails();
}

void ResultsViewer::serviceDetailsDiscovered()
{
    switch (service->state()) {
    case QLowEnergyService::RemoteServiceDiscovering:
        emit discovering();
        break;
    case QLowEnergyService::RemoteServiceDiscovered:
        myCharacteristics = service->characteristics();
        readNose = &(myCharacteristics.first());
        writeNose = &(myCharacteristics.last());
        *readNose = service->characteristic(readNose->uuid());
        *writeNose = service->characteristic(writeNose->uuid());
        descriptor = readNose->descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
        service->writeDescriptor(descriptor, QByteArray::fromHex("0100"));
        emit subscribedToNose();
    default:
        break;
    }
}

void ResultsViewer::startRun()
{
    model->setHorizontalHeaderLabels(QStringList() << "TGS2600\nH, CO" << "TGS2602\nH2S, amm" << "TGS2603\namm, Sul" << "TGS2610\nLPGs" << "MQ3\nAlc"
                                                   << "MQ7\nCO" << "MQ135\nMix" << "TGS2611\nMeth" << "TGS2620\nM/Eth");
    service->writeCharacteristic(*writeNose, QByteArray::fromStdString("Run"));
}

void ResultsViewer::processReadings()
{
    //qInfo() << QThread::currentThread();
    sample.timeUnits++;
    readArray = readNose->value();
    QDataStream stream(&readArray, QIODeviceBase::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> sample.tgs2600;
    if(peakValues.tgs2600 < sample.tgs2600){
        peakValues.tgs2600 = sample.tgs2600;
        peakTimes.tgs2600 = sample.timeUnits;
    }
    stream >> sample.tgs2602;
    if(peakValues.tgs2602 < sample.tgs2602){
        peakValues.tgs2602 = sample.tgs2602;
        peakTimes.tgs2602 = sample.timeUnits;
    }
    stream >> sample.tgs2603;
    if(peakValues.tgs2603 < sample.tgs2603){
        peakValues.tgs2603 = sample.tgs2603;
        peakTimes.tgs2603 = sample.timeUnits;
    }
    stream >> sample.tgs2610;
    if(peakValues.tgs2610 < sample.tgs2610){
        peakValues.tgs2610 = sample.tgs2610;
        peakTimes.tgs2610 = sample.timeUnits;
    }
    stream >> sample.tgs2611;
    if(peakValues.tgs2611 < sample.tgs2611){
        peakValues.tgs2611 = sample.tgs2611;
        peakTimes.tgs2611 = sample.timeUnits;
    }
    stream >> sample.tgs2620;
    if(peakValues.tgs2620 < sample.tgs2620){
        peakValues.tgs2620 = sample.tgs2620;
        peakTimes.tgs2620 = sample.timeUnits;
    }
    stream >> sample.mq3;
    if(peakValues.mq3 < sample.mq3){
        peakValues.mq3 = sample.mq3;
        peakTimes.mq3 = sample.timeUnits;
    }
    stream >> sample.mq7;
    if(peakValues.mq7 < sample.mq7){
        peakValues.mq7 = sample.mq7;
        peakTimes.mq7 = sample.timeUnits;
    }
    stream >> sample.mq135;
    if(peakValues.mq135 < sample.mq135){
        peakValues.mq135 = sample.mq135;
        peakTimes.mq135 = sample.timeUnits;
    }

    QList<QStandardItem*> entry;
    entry.append(new QStandardItem(QString::number(sample.tgs2600)));
    entry.append(new QStandardItem(QString::number(sample.tgs2602)));
    entry.append(new QStandardItem(QString::number(sample.tgs2603)));
    entry.append(new QStandardItem(QString::number(sample.tgs2610)));
    entry.append(new QStandardItem(QString::number(sample.tgs2611)));
    entry.append(new QStandardItem(QString::number(sample.tgs2620)));
    entry.append(new QStandardItem(QString::number(sample.mq3)));
    entry.append(new QStandardItem(QString::number(sample.mq7)));
    entry.append(new QStandardItem(QString::number(sample.mq135)));
    model->appendRow(entry);

    emit readingsProcessed();
}

void ResultsViewer::reset()
{
    model->clear();
}

void ResultsViewer::stopReadings()
{
    service->writeCharacteristic(*writeNose, QByteArray::fromStdString("Stop"));
    emit readingEnded();
}

