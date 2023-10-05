#include "devicefinder.h"

DeviceFinder::DeviceFinder(){
    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(20000);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &DeviceFinder::addDevice);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred, this, &DeviceFinder::scanError);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &DeviceFinder::scanFinished);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &DeviceFinder::scanCanceled);
}

DeviceFinder::~DeviceFinder(){
    delete m_deviceDiscoveryAgent;
}

QBluetoothDeviceInfo* DeviceFinder::currentDevice() {
    if (m_selectedDeviceIt == m_devices.end())
        return nullptr;
    return *m_selectedDeviceIt;
}

void DeviceFinder::search(){
    emit addToLogs("Searching devices...");
    emit buttonSearchEnable(false);
    emit devicesListClear();
    clearDeviceList();
    m_deviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void DeviceFinder::stop(){
    m_deviceDiscoveryAgent->stop();
}

void DeviceFinder::updateSelectedDeviceIt(const QString &devID){
    if(!m_devices.empty()){
        m_selectedDeviceIt = std::find_if(m_devices.begin(), m_devices.end(),
                                         [devID](QBluetoothDeviceInfo *dev)
                                         {return devID == (dev->name() + " " + dev->address().toString());});
    }
}

void DeviceFinder::addDevice(const QBluetoothDeviceInfo &device){
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration){
        auto devInfo = new QBluetoothDeviceInfo(device);
        auto it = std::find_if(m_devices.begin(), m_devices.end(),
                               [devInfo](QBluetoothDeviceInfo *dev)
                               {return devInfo->address() == dev->address();});
        if (it == m_devices.end()) {
            m_devices.push_back(devInfo);
            emit  addToDevList(device.name() + " " + device.address().toString());
        }
        else
            delete devInfo;
    }
}

void DeviceFinder::scanError(){
    emit buttonSearchEnable(true);
    emit addToLogs("Search error.");
}


void DeviceFinder::scanFinished(){
    emit buttonSearchEnable(true);
    emit addToLogs("Search finished.");
}

void DeviceFinder::scanCanceled(){
    emit buttonSearchEnable(true);
    emit addToLogs("Search canceled.");
}

void DeviceFinder::clearDeviceList(){
    if(!m_devices.empty()){
        for (auto devInfo : m_devices)
            delete devInfo;
        m_devices.clear();
        m_selectedDeviceIt = m_devices.end();
    }
}
