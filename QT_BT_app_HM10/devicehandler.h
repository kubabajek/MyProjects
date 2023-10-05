#ifndef DEVICEHANDLER_H
#define DEVICEHANDLER_H

#include <QLowEnergyController>

#define HM10_SERVICE_UUID QBluetoothUuid("0000ffe0-0000-1000-8000-00805f9b34fb")
#define HM10_CHAR_UUID QBluetoothUuid("0000ffe1-0000-1000-8000-00805f9b34fb")
#define BT_MAX_SEND_LEN 20

class DeviceHandler : public QObject { Q_OBJECT

public:
    DeviceHandler();
    ~DeviceHandler();
    void connect(QBluetoothDeviceInfo* device);
    void disconnect();
    void txlrstr(const QString& mystr);

public slots:
    void read();
    void write(const QString &command);

signals:
    void addToLogs(const QString &message);
    void buttonConnectEnable(bool state);
    void buttonDisconnectEnable(bool state);
    void enableControlButtons(bool state);
    void transmitterConnectionStatus(const QString &message);
    void dataReceived(const QString &data);

private slots:
    void connectionError();
    void connected();
    void disconnected();
    void serviceDiscovered(const QBluetoothUuid &gatt);
    void serviceScanDone();

    void serviceStateChanged(QLowEnergyService::ServiceState s);
    void characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic);
    void errorOccurred(QLowEnergyService::ServiceError error);

private:
    QLowEnergyController* m_control;
    QLowEnergyService* m_service;
    QString m_readDataBuffer;
    QString m_writeDataBuffer;
    bool m_disconnectFlag;
    bool m_serialServiceFoundFlag;

    QByteArray write_cmd(const QString &cmd);
    void read_cmd(const QByteArray &cmd);
};

#endif // DEVICEHANDLER_H
