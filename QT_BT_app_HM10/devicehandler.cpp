#include "devicehandler.h"

DeviceHandler::DeviceHandler(){
    m_disconnectFlag = false;
    m_serialServiceFoundFlag = false;
    m_readDataBuffer = "";
}

DeviceHandler::~DeviceHandler(){
    delete m_control;
    delete m_service;
}

void DeviceHandler::connect(QBluetoothDeviceInfo* device){
    if (device == nullptr)
        emit addToLogs("No selected device to connect!");
    else{
        emit buttonConnectEnable(false);
        emit addToLogs("Connecting to: "+ device->name());
        m_control = QLowEnergyController::createCentral(*device, this);
        QObject::connect(m_control, &QLowEnergyController::connected, this, &DeviceHandler::connected);
        QObject::connect(m_control, &QLowEnergyController::disconnected, this, &DeviceHandler::disconnected);
        QObject::connect(m_control, &QLowEnergyController::errorOccurred, this, &DeviceHandler::connectionError);
        QObject::connect(m_control, &QLowEnergyController::serviceDiscovered, this, &DeviceHandler::serviceDiscovered);
        QObject::connect(m_control, &QLowEnergyController::discoveryFinished, this, &DeviceHandler::serviceScanDone);
        m_control->connectToDevice();
    }
}

void DeviceHandler::disconnect(){
    m_disconnectFlag = true;
    m_control->disconnectFromDevice();
}

void txlrstr(const QString& mystr){
    QString = cmd
}

void DeviceHandler::read(){
    const QLowEnergyCharacteristic hmChar = m_service->characteristic(HM10_CHAR_UUID);
    if(m_service && hmChar.isValid())
        m_service->readCharacteristic(hmChar);
    else
        emit addToLogs("Can't read! No device is connected");
}

void DeviceHandler::write(const QString &command){
    const QLowEnergyCharacteristic hmChar = m_service->characteristic(HM10_CHAR_UUID);
    if(m_service && hmChar.isValid())
        m_service->writeCharacteristic(hmChar, write_cmd(command), QLowEnergyService::WriteWithoutResponse);
    else
        emit addToLogs("Can't write! No device is connected");
}

void DeviceHandler::connectionError(){
    emit buttonConnectEnable(true);
    emit addToLogs("Cannot connect to device.");
}

void DeviceHandler::connected(){
    emit buttonConnectEnable(false);
    emit buttonDisconnectEnable(true);
    emit addToLogs("Connected!");
    m_control->discoverServices();
}

void DeviceHandler::disconnected(){
    emit buttonConnectEnable(true);
    emit buttonDisconnectEnable(false);
    emit enableControlButtons(false);
    emit transmitterConnectionStatus("Transmitter status: NOT CONNECTED");
    if (m_disconnectFlag)
        emit addToLogs("Disconnected");
    else
        emit addToLogs("Connection lost!");
    m_disconnectFlag = false; // reset flags
    m_serialServiceFoundFlag = false;
    delete m_control; // delete objects
    delete m_service;
    m_control = nullptr; // delete pointers
    m_service = nullptr;
}

void DeviceHandler::serviceDiscovered(const QBluetoothUuid &gatt){
    if (gatt == HM10_SERVICE_UUID)
        m_serialServiceFoundFlag = true;
}

void DeviceHandler::serviceScanDone(){ //create service object
    if(m_serialServiceFoundFlag == true)
        m_service = m_control->createServiceObject(HM10_SERVICE_UUID, this);
    if(m_service){
        QObject::connect(m_service, &QLowEnergyService::stateChanged, this, &DeviceHandler::serviceStateChanged);
        QObject::connect(m_service, &QLowEnergyService::characteristicRead, this, &DeviceHandler::characteristicRead);
        QObject::connect(m_service, &QLowEnergyService::characteristicChanged, this, &DeviceHandler::characteristicChanged);
        QObject::connect(m_service, &QLowEnergyService::characteristicWritten, this, &DeviceHandler::characteristicWritten);
        QObject::connect(m_service, &QLowEnergyService::errorOccurred, this, &DeviceHandler::errorOccurred);
        m_service->discoverDetails();
    }
    else{
        emit addToLogs("Error: Not found HM10 communication service.");
        disconnect();
    }
}

void DeviceHandler::serviceStateChanged(QLowEnergyService::ServiceState s){
    if (s == QLowEnergyService::RemoteServiceDiscovered){
        const QLowEnergyCharacteristic hmChar = m_service->characteristic(HM10_CHAR_UUID);
        if (!hmChar.isValid()) {
            emit addToLogs("HM10 Characteristics not found");
            disconnect();
            return;
        }
        QLowEnergyDescriptor m_notificationDesc = hmChar.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
        if (m_notificationDesc.isValid()){
            m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));
            emit transmitterConnectionStatus("Transmitter status: OK");
            emit enableControlButtons(true);
        }
        else{
            emit addToLogs("Not found ClientCharacteristicConfiguration");
            disconnect();
        }
    }
}

void DeviceHandler::characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value){
    emit addToLogs(characteristic.uuid().toString()+" read: " + QString::fromUtf8(value));
}

void DeviceHandler::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue){
    if(characteristic.uuid() == HM10_CHAR_UUID){
        read_cmd(newValue);
    }
}

void DeviceHandler::characteristicWritten(const QLowEnergyCharacteristic &characteristic){
    if(characteristic.uuid() == HM10_CHAR_UUID){
        if (m_readDataBuffer.isEmpty())
            emit addToLogs("Command sent");
        else
            write(m_readDataBuffer);
    }
}

void DeviceHandler::errorOccurred(QLowEnergyService::ServiceError error){
    switch (error) {
    case QLowEnergyService::NoError:
        emit addToLogs("NoError");
        break;
    case QLowEnergyService::OperationError:
        emit addToLogs("OperationError");
        break;
    case QLowEnergyService::CharacteristicReadError:
        emit addToLogs("CharacteristicReadError");
        break;
    case QLowEnergyService::CharacteristicWriteError:
        emit addToLogs("CharacteristicWriteError");
        break;
    case QLowEnergyService::DescriptorReadError:
        emit addToLogs("DescriptorReadError");
        break;
    case QLowEnergyService::DescriptorWriteError:
        emit addToLogs("DescriptorWriteError");
        break;
    case QLowEnergyService::UnknownError:
        emit addToLogs("UnknownError");
        break;
    default:
        emit addToLogs("UnknownEnumValue");
        break;
    }
}

QByteArray DeviceHandler::write_cmd(const QString &cmd){
    QString data = cmd;
    if (!data.endsWith("\r\n"))
        data += "\r\n";
    if (m_readDataBuffer.isEmpty()){
        m_readDataBuffer = data.mid(BT_MAX_SEND_LEN);
        data = data.left(BT_MAX_SEND_LEN);
    }
    return data.toUtf8();
}

void DeviceHandler::read_cmd(const QByteArray &cmd){
    QString data = QString::fromUtf8(cmd);
    m_readDataBuffer += data;
    if (data.endsWith("\r\n")){
        m_readDataBuffer.chop(2);
        emit dataReceived(m_readDataBuffer);
        m_readDataBuffer = "";
    }
}


