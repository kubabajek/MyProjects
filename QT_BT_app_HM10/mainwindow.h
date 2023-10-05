#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>

#include "devicefinder.h"
#include "devicehandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow{Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void addToLogs(const QString &message);

signals:
    void changedSelectedDeviceID(const QString &devID);

private slots:
    void on_pushButtonSearch_clicked();
    void on_pushButtonConnect_clicked();
    void on_pushButtonDisconnect_clicked();
    void on_pushButtonEnginesON_clicked();
    void on_pushButtonEnginesOFF_clicked();
    void on_pushButtonReverse_clicked();
    void on_pushButtonGetStatus_clicked();
    void on_pushButtonRaiseLeak_clicked();
    void on_pushButtonResetLeak_clicked();
    void on_pushButtonAscent_clicked();
    void on_pushButtonStay_clicked();
    void on_pushButtonDescent_clicked();
    void on_pushButtonLeft_clicked();
    void on_pushButtonStraight_clicked();
    void on_pushButtonRight_clicked();

    //MySlots
    void enableControlButtons(bool state);


private:
    Ui::MainWindow* ui;
    DeviceFinder* df;
    DeviceHandler* dh;
    uint8_t cnt;
};
#endif // MAINWINDOW_H
