#ifndef DEVICEFINDER_H
#define DEVICEFINDER_H

#include <QBluetoothDeviceDiscoveryAgent>

class DeviceFinder : public QObject { Q_OBJECT

public:
    DeviceFinder();
    ~DeviceFinder();
    QBluetoothDeviceInfo* currentDevice();
    void search();
    void stop();

public slots:
    void updateSelectedDeviceIt(const QString &devID);

signals:
    void addToLogs(const QString &message);
    void addToDevList(const QString &devID);
    void buttonSearchEnable(bool state);
    void devicesListClear();

private slots:
    void addDevice(const QBluetoothDeviceInfo &device);
    void scanError();
    void scanFinished();
    void scanCanceled();

private:
    QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent;
    std::vector<QBluetoothDeviceInfo*> m_devices;
    std::vector<QBluetoothDeviceInfo*>::iterator m_selectedDeviceIt = m_devices.end();

    void clearDeviceList();
};

#endif // DEVICEFINDER_H
