#include "homepage.h"
#include "./ui_homepage.h"

Homepage::Homepage(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Homepage)
{
    ui->setupUi(this);
    setWindowTitle("eNose v1.0");
    ui->table->setVisible(false);
    ui->peaks->setVisible(false);
    ui->beerName->setVisible(false);
    ui->run->setVisible(false);
    ui->reset->setVisible(false);
    ui->postButton->setVisible(false);
    authhandler = new AuthHandler(fetchKey());
    //btfinder = new BTFinder();
    //connect(authhandler, &AuthHandler::userSignedIn, btfinder, &BTFinder::startSearch);
    connect(authhandler, SIGNAL(userSignedIn()), this, SLOT(signInSuccess()));
    connect(ui->signInButton, SIGNAL(clicked(bool)), this, SLOT(signIn()));
    connect(ui->run, SIGNAL(clicked(bool)), this, SLOT(startRun()));
    connect(ui->reset, SIGNAL(clicked(bool)), this, SLOT(resetNose()));
    connect(ui->postButton, SIGNAL(clicked(bool)), this, SLOT(createDbEntry()));
    QThread::currentThread()->setObjectName("HomepageThread");
}

Homepage::~Homepage()
{
    delete ui;
}

void Homepage::signIn()
{
    email = ui->emailField->text();
    password = ui->passwordField->text();
    switch (ui->checkBox->checkState()){
    case Qt::Checked:
        signUserUP = 1;
        authhandler->signUserUp(email, password);
        break;
    case Qt::Unchecked:
        signUserUP = 0;
        authhandler->signUserIn(email, password);
        break;
    default:
        break;
    }
}

void Homepage::signInSuccess()
{
    switch(signUserUP) {
    case 0:
        ui->emailField->setVisible(false);
        ui->passwordField->setVisible(false);
        ui->checkBox->setVisible(false);
        ui->signInButton->setVisible(false);
        ui->info->setText("Welcome " + email);
        disco = new QBluetoothDeviceDiscoveryAgent(this);
        connect(disco, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this, SLOT(foundDevice(QBluetoothDeviceInfo)));
        connect(disco, SIGNAL(finished()), this, SLOT(btScanComplete()));
        disco->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
        break;
    case 1:
        ui->emailField->clear();
        ui->passwordField->clear();
        ui->info->setText("user " + email + " created");
        break;
    }
}

void Homepage::foundDevice(QBluetoothDeviceInfo dev)
{
    qDebug() << dev.name();
    if (dev.name() == "eNose"){
        disco->stop();
        ui->info->setText("eNose found --> Connecting...");
        viewer = new ResultsViewer(&dev, this);
        connect(viewer, &ResultsViewer::discovering, this, &Homepage::discoveringServices);
        connect(viewer, &ResultsViewer::subscribedToNose, this, &Homepage::readyToRun);
        connect(viewer, &ResultsViewer::readingEnded, this, &Homepage::finishedRead);
        connect(viewer, &ResultsViewer::readingsProcessed, this, &Homepage::readingsProcessed);
        connect(this, &Homepage::reset, viewer, &ResultsViewer::reset);
    }
}

void Homepage::btScanComplete()
{
    ui->info->setText("eNose not found. Retrying...");
    disco->setLowEnergyDiscoveryTimeout(5000);
    disco->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void Homepage::discoveringServices()
{
    ui->info->setText("eNose connected --> setting up");
}

void Homepage::readyToRun()
{
    ui->info->setText("Ready");
    ui->table->setVisible(true);
    ui->peaks->setVisible(true);
    ui->beerName->setVisible(true);
    ui->run->setVisible(true);
    ui->reset->setVisible(true);
}

void Homepage::startRun()
{
    ui->info->setText("Running");
    viewer->startRun();
    ui->table->setModel(viewer->model);
    connect(viewer->service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),
            viewer, SLOT(processReadings()));
    QTimer::singleShot(15000, viewer, &ResultsViewer::stopReadings);
}

void Homepage::resetNose()
{
    emit reset();
}

void Homepage::finishedRead()
{
    ui->info->setText("Readings Done. Stopping...");
    ui->postButton->setVisible(true);
}

void Homepage::createDbEntry()
{
    QVariantMap mq135;
    mq135["value"] = viewer->peakValues.mq135;
    mq135["time"] = viewer->peakTimes.mq135;
    QVariantMap mq3;
    mq3["value"] = viewer->peakValues.mq3;
    mq3["time"] = viewer->peakTimes.mq3;
    QVariantMap mq7;
    mq7["value"] = viewer->peakValues.mq7;
    mq7["time"] = viewer->peakTimes.mq7;
    QVariantMap tgs2600;
    tgs2600["value"] = viewer->peakValues.tgs2600;
    tgs2600["time"] = viewer->peakTimes.tgs2600;
    QVariantMap tgs2602;
    tgs2602["value"] = viewer->peakValues.tgs2602;
    tgs2602["time"] = viewer->peakTimes.tgs2602;
    QVariantMap tgs2603;
    tgs2603["value"] = viewer->peakValues.tgs2603;
    tgs2603["time"] = viewer->peakTimes.tgs2603;
    QVariantMap tgs2610;
    tgs2610["value"] = viewer->peakValues.tgs2610;
    tgs2610["time"] = viewer->peakTimes.tgs2610;
    QVariantMap tgs2611;
    tgs2611["value"] = viewer->peakValues.tgs2611;
    tgs2611["time"] = viewer->peakTimes.tgs2611;
    QVariantMap tgs2620;
    tgs2620["value"] = viewer->peakValues.tgs2620;
    tgs2620["time"] = viewer->peakTimes.tgs2620;
    QVariantMap beer;
    beer["Beer"] = ui->beerName->text();
    beer["MQ135"] = mq135;
    beer["MQ3"] = mq3;
    beer["MQ7"] = mq7;
    beer["TGS2600"] = tgs2600;
    beer["TGS2602"] = tgs2602;
    beer["TGS2603"] = tgs2603;
    beer["TGS2610"] = tgs2610;
    beer["TGS2611"] = tgs2611;
    beer["TGS2620"] = tgs2620;
    authhandler->add_to_db(beer);
}

void Homepage::readingsProcessed()
{
    ui->table->scrollToBottom();
    ui->peaks->setText("TGS2600 -> " + QString::number(viewer->peakValues.tgs2600) + " " + "TGS2602 -> " + QString::number(viewer->peakValues.tgs2602) + " "
                       + "TGS2603 -> " + QString::number(viewer->peakValues.tgs2603) + " " + "TGS2610 -> " + QString::number(viewer->peakValues.tgs2610) + " "
                       + "MQ3 -> " + QString::number(viewer->peakValues.mq3) + " " + "MQ7 -> " + QString::number(viewer->peakValues.mq7) + " "
                       + "MQ135 -> " + QString::number(viewer->peakValues.mq135) + " " + "TGS2611 -> " + QString::number(viewer->peakValues.tgs2611) + " "
                       + "TGS2620 -> " + QString::number(viewer->peakValues.tgs2620));
}

QString Homepage::fetchKey()
{
    QFile file("C:\\Users\\basee\\OneDrive\\Documents\\NewControlApp\\eNose.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "check file path";
    }
    QTextStream in(&file);
    QString key = in.readLine();
    return key;
}




