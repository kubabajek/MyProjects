#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    this->df = new DeviceFinder();
    this->dh = new DeviceHandler();
    cnt = 0;

    connect(ui->DevicesList, &QComboBox::currentTextChanged, df, &DeviceFinder::updateSelectedDeviceIt);
    connect(df, &DeviceFinder::addToDevList, this, [=](const QString &devID){ui->DevicesList->addItem(devID);});
    connect(df, &DeviceFinder::addToLogs, this, &MainWindow::addToLogs);
    connect(df, &DeviceFinder::buttonSearchEnable, this, [=](bool state){ui->pushButtonSearch->setEnabled(state);});
    connect(df, &DeviceFinder::devicesListClear, this, [=](){ui->DevicesList->clear();});

    connect(dh, &DeviceHandler::addToLogs, this, &MainWindow::addToLogs);
    connect(dh, &DeviceHandler::buttonConnectEnable, this, [=](bool state){ui->pushButtonConnect->setEnabled(state);});
    connect(dh, &DeviceHandler::buttonDisconnectEnable, this, [=](bool state){ui->pushButtonDisconnect->setEnabled(state);});
    connect(dh, &DeviceHandler::enableControlButtons, this, &MainWindow::enableControlButtons);
    connect(dh, &DeviceHandler::transmitterConnectionStatus, this, [=](const QString &message){ui->lineEditTransmitterConnection->setText(message);});

    connect(dh, &DeviceHandler::dataReceived, this, &MainWindow::addToLogs);
}

MainWindow::~MainWindow(){
    delete ui;
    delete df;
    delete dh;
}

void MainWindow::addToLogs(const QString &message){
    QString currentTime = QDateTime::currentDateTime().toString("hh:mm:ss"); //"yyyy.MM.dd hh:mm:ss"
    ui->textEditLogs->append(currentTime + " " + message); // \t - tabulator
}

void MainWindow::enableControlButtons(bool state){
    ui->pushButtonEnginesON->setEnabled(state);
    ui->pushButtonEnginesOFF->setEnabled(state);
    ui->pushButtonAscent->setEnabled(state);
    ui->pushButtonReverse->setEnabled(state);
    ui->pushButtonGetStatus->setEnabled(state);
    ui->pushButtonStay->setEnabled(state);
    ui->pushButtonRaiseLeak->setEnabled(state);
    ui->pushButtonResetLeak->setEnabled(state);
    ui->pushButtonDescent->setEnabled(state);
    ui->pushButtonLeft->setEnabled(state);
    ui->pushButtonStraight->setEnabled(state);
    ui->pushButtonRight->setEnabled(state);
    ui->horizontalSliderEngineSpeed->setEnabled(state);
    ui->horizontalSliderRotatingSpeed->setEnabled(state);
    ui->horizontalSliderTurningSpeed->setEnabled(state);
}

void MainWindow::on_pushButtonSearch_clicked(){
    df->search();
}

void MainWindow::on_pushButtonConnect_clicked(){
    df->stop();
    dh->connect(df->currentDevice());
}

void MainWindow::on_pushButtonDisconnect_clicked(){
    dh->disconnect();
}


void MainWindow::on_pushButtonEnginesON_clicked(){
    dh->write("AT");
}


void MainWindow::on_pushButtonEnginesOFF_clicked(){
    dh->read();
}

void MainWindow::on_pushButtonAscent_clicked(){
    dh->write("AT+MODE=TEST");
}

void MainWindow::on_pushButtonReverse_clicked(){
    dh->write("AT+TEST=RXLRPKT");
}

void MainWindow::on_pushButtonGetStatus_clicked(){
    dh->write("AT+TEST=TXLRSTR,'Me123Be'");
}

void MainWindow::on_pushButtonStay_clicked(){
    dh->write("AT+TEST=TXLRPKT,"+QString::number(cnt, 16));
    cnt++;
}


void MainWindow::on_pushButtonRaiseLeak_clicked(){
}


void MainWindow::on_pushButtonResetLeak_clicked()
{
    qDebug() << "ResetLeak button clicked";
}


void MainWindow::on_pushButtonDescent_clicked()
{
    qDebug() << "Descent button clicked";
}


void MainWindow::on_pushButtonLeft_clicked()
{
    qDebug() << "Left button clicked";
}


void MainWindow::on_pushButtonStraight_clicked()
{
    qDebug() << "Straight button clicked";
}


void MainWindow::on_pushButtonRight_clicked()
{
    qDebug() << "Right button clicked";
}

