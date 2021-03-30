#include "btwifisetup.h"
#include "bluetoothdeviceinfo.h"
#include "wirelessaccesspoints.h"
#include "wirelessaccesspoint.h"

#include <QJsonDocument>

static QBluetoothUuid wifiServiceUuid =                 QBluetoothUuid(QUuid("e081fec0-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid wifiCommanderCharacteristicUuid = QBluetoothUuid(QUuid("e081fec1-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid wifiResponseCharacteristicUuid =  QBluetoothUuid(QUuid("e081fec2-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid wifiStatusCharacteristicUuid =    QBluetoothUuid(QUuid("e081fec3-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid wifiModeCharacteristicUuid =      QBluetoothUuid(QUuid("e081fec4-f757-4449-b9c9-bfa83133f7fc"));

static QBluetoothUuid networkServiceUuid =                  QBluetoothUuid(QUuid("ef6d6610-b8af-49e0-9eca-ab343513641c"));
static QBluetoothUuid networkStatusCharacteristicUuid =     QBluetoothUuid(QUuid("ef6d6611-b8af-49e0-9eca-ab343513641c"));
static QBluetoothUuid networkCommanderCharacteristicUuid =  QBluetoothUuid(QUuid("ef6d6612-b8af-49e0-9eca-ab343513641c"));
static QBluetoothUuid networkResponseCharacteristicUuid =   QBluetoothUuid(QUuid("ef6d6613-b8af-49e0-9eca-ab343513641c"));
static QBluetoothUuid networkingEnabledCharacteristicUuid = QBluetoothUuid(QUuid("ef6d6614-b8af-49e0-9eca-ab343513641c"));
static QBluetoothUuid wirelessEnabledCharacteristicUuid =   QBluetoothUuid(QUuid("ef6d6615-b8af-49e0-9eca-ab343513641c"));

static QBluetoothUuid systemServiceUuid =                 QBluetoothUuid(QUuid("e081fed0-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid systemCommanderCharacteristicUuid = QBluetoothUuid(QUuid("e081fed1-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid systemResponseCharacteristicUuid =  QBluetoothUuid(QUuid("e081fed2-f757-4449-b9c9-bfa83133f7fc"));

BtWiFiSetup::BtWiFiSetup(QObject *parent) : QObject(parent)
{
    m_accessPoints = new WirelessAccessPoints(this);
    qRegisterMetaType<BluetoothDeviceInfo*>("const BluetoothDeviceInfo*");
}

BtWiFiSetup::~BtWiFiSetup()
{
    qDebug() << "Destroying BtWifiSetup";
}

void BtWiFiSetup::connectToDevice(const BluetoothDeviceInfo *device)
{
    qDebug() << "Connecting to device" << device->address() << device->name();
    if (m_btController) {
        delete m_btController;
        m_currentConnection = nullptr;
        currentConnectionChanged();
        m_accessPoints->clearModel();
        m_bluetoothStatus = BluetoothStatusDisconnected;
        emit bluetoothStatusChanged(m_bluetoothStatus);
    }

    m_btController = QLowEnergyController::createCentral(device->bluetoothDeviceInfo(), this);
    connect(m_btController, &QLowEnergyController::connected, this, [this](){
        qInfo() << "Bluetooth connected";
        m_btController->discoverServices();
        m_bluetoothStatus = BluetoothStatusConnectedToBluetooth;
        emit bluetoothStatusChanged(m_bluetoothStatus);
    }, Qt::QueuedConnection);

    connect(m_btController, &QLowEnergyController::disconnected, this, [this](){
        qInfo() << "Bluetooth disconnected";
        m_bluetoothStatus = BluetoothStatusDisconnected;
        emit bluetoothStatusChanged(m_bluetoothStatus);
        m_btController->deleteLater();
        m_btController = nullptr;
        m_currentConnection = nullptr;
        emit currentConnectionChanged();
        m_accessPoints->clearModel();
    }, Qt::QueuedConnection);

    typedef void (QLowEnergyController::*errorsSignal)(QLowEnergyController::Error);
    connect(m_btController, static_cast<errorsSignal>(&QLowEnergyController::error), this, [this](QLowEnergyController::Error error){
        qWarning() << "Bluetooth error:" << error;
        emit this->bluetoothConnectionError();
    }, Qt::QueuedConnection);

    connect(m_btController, &QLowEnergyController::discoveryFinished, this, [this](){
        qDebug() << "Bluetooth service discovery finished";
        setupServices();
    });

    m_bluetoothStatus = BluetoothStatusConnectingToBluetooth;
    emit bluetoothStatusChanged(m_bluetoothStatus);
    m_btController->connectToDevice();
}

void BtWiFiSetup::disconnectFromDevice()
{
    if (m_btController) {
        m_btController->disconnectFromDevice();
    }
}

void BtWiFiSetup::connectDeviceToWiFi(const QString &ssid, const QString &password)
{
    if (m_bluetoothStatus < BluetoothStatusConnectedToBluetooth) {
        qWarning() << "Cannot connect to wifi in state" << m_bluetoothStatus;
        return;
    }

    QVariantMap request;
    request.insert("c", (int)WirelessServiceCommandConnect);
    QVariantMap parameters;
    parameters.insert("e", ssid);
    parameters.insert("p", password);
    request.insert("p", parameters);
    streamData(m_wifiService, wifiCommanderCharacteristicUuid, request);
}

void BtWiFiSetup::startAccessPoint(const QString &ssid, const QString &password)
{
    qDebug() << "WifiSetupManager: Start wireless access point using SSID:" << ssid << " password:" << password;

    if (!m_wifiService) {
        qWarning() << "WifiSetupManager: Could not set wireless network. Service not valid";
        return;
    }

    QVariantMap request;
    request.insert("c", static_cast<int>(WirelessServiceCommandStartAccessPoint));
    QVariantMap parameters;
    parameters.insert("e", ssid);
    parameters.insert("p", password);
    request.insert("p", parameters);

    streamData(m_wifiService, wifiCommanderCharacteristicUuid, request);
}

void BtWiFiSetup::disconnectDeviceFromWiFi()
{
    if (m_bluetoothStatus != BluetoothStatusConnectedToBluetooth) {
        qWarning() << "Cannot disconnect from wifi in state" << m_bluetoothStatus;
    }
    QVariantMap request;
    request.insert("c", (int)WirelessServiceCommandDisconnect);
    streamData(m_wifiService, wifiCommanderCharacteristicUuid, request);
}

void BtWiFiSetup::scanWiFi()
{
    if (m_bluetoothStatus != BluetoothStatusConnectedToBluetooth) {
        qWarning() << "Cannot disconnect from wifi in state" << m_bluetoothStatus;
    }
    QVariantMap request;
    request.insert("c", (int)WirelessServiceCommandScan);
    streamData(m_wifiService, wifiCommanderCharacteristicUuid, request);
}

bool BtWiFiSetup::pressPushButton()
{
    if (!m_systemService) {
        qWarning() << "System service not available. Cannot perform push button pairing";
        return false;
    }
    QVariantMap request;
    request.insert("c", (int)SystemServiceCommandPushAuthentication);

    streamData(m_systemService, systemCommanderCharacteristicUuid, request);
    return true;
}

BtWiFiSetup::BluetoothStatus BtWiFiSetup::bluetoothStatus() const
{
    return m_bluetoothStatus;
}

QString BtWiFiSetup::modelNumber() const
{
    return m_modelNumber;
}

QString BtWiFiSetup::manufacturer() const
{
    return m_manufacturer;
}

QString BtWiFiSetup::softwareRevision() const
{
    return m_softwareRevision;
}

QString BtWiFiSetup::firmwareRevision() const
{
    return m_firmwareRevision;
}

QString BtWiFiSetup::hardwareRevision() const
{
    return m_hardwareRevision;
}

BtWiFiSetup::NetworkStatus BtWiFiSetup::networkStatus() const
{
    return m_networkStatus;
}

BtWiFiSetup::WirelessStatus BtWiFiSetup::wirelessStatus() const
{
    return m_wirelessStatus;
}

BtWiFiSetup::WirelessDeviceMode BtWiFiSetup::wirelessDeviceMode() const
{
    return m_wirelessDeviceMode;
}

bool BtWiFiSetup::networkingEnabled() const
{
    return m_networkingEnabled;
}

void BtWiFiSetup::setNetworkingEnabled(bool networkingEnabled)
{
    if (m_bluetoothStatus != BluetoothStatusConnectedToBluetooth) {
        qWarning() << "Cannot disconnect from wifi in state" << m_bluetoothStatus;
    }
    QLowEnergyCharacteristic characteristic = m_networkService->characteristic(networkCommanderCharacteristicUuid);
    m_networkService->writeCharacteristic(characteristic, networkingEnabled ? QByteArray::fromHex("00") : QByteArray::fromHex("01"));
}

bool BtWiFiSetup::wirelessEnabled() const
{
    return m_wirelessEnabled;
}

void BtWiFiSetup::setWirelessEnabled(bool wirelessEnabled) const
{
    if (m_bluetoothStatus != BluetoothStatusConnectedToBluetooth) {
        qWarning() << "Cannot disconnect from wifi in state" << m_bluetoothStatus;
    }
    QLowEnergyCharacteristic characteristic = m_networkService->characteristic(networkCommanderCharacteristicUuid);
    m_networkService->writeCharacteristic(characteristic, wirelessEnabled ? QByteArray::fromHex("02") : QByteArray::fromHex("03"));
}

WirelessAccessPoints *BtWiFiSetup::accessPoints() const
{
    return m_accessPoints;
}

WirelessAccessPoint *BtWiFiSetup::currentConnection() const
{
    return m_currentConnection;
}

void BtWiFiSetup::setupServices()
{
    qDebug() << "Setting up Bluetooth services";
    m_deviceInformationService = m_btController->createServiceObject(QBluetoothUuid::DeviceInformation, m_btController);
    m_networkService = m_btController->createServiceObject(networkServiceUuid, m_btController);
    m_wifiService = m_btController->createServiceObject(wifiServiceUuid, m_btController);
    m_systemService = m_btController->createServiceObject(systemServiceUuid, m_btController);

    if (!m_wifiService || !m_deviceInformationService || !m_networkService) {
        if (m_btController->property("retries").toInt() < 3) {
            qDebug() << "Required services not found on remote device. Retrying...";
            m_btController->discoverServices();
            m_btController->setProperty("retries", m_btController->property("retries").toInt() + 1);
        } else {
            qWarning() << "Required services not found on remote device. Disconnecting";
            m_btController->disconnectFromDevice();
        }
        return;
    }

    // Device information
    connect(m_deviceInformationService, &QLowEnergyService::stateChanged, this, [this](QLowEnergyService::ServiceState state) {
        if (state != QLowEnergyService::ServiceDiscovered)
            return;
        qDebug() << "Device info service discovered";
        m_manufacturer = QString::fromUtf8(m_deviceInformationService->characteristic(QBluetoothUuid::ManufacturerNameString).value());
        emit manufacturerChanged();
        m_modelNumber = QString::fromUtf8(m_deviceInformationService->characteristic(QBluetoothUuid::ModelNumberString).value());
        emit modelNumberChanged();
        m_softwareRevision = QString::fromUtf8(m_deviceInformationService->characteristic(QBluetoothUuid::SoftwareRevisionString).value());
        emit softwareRevisionChanged();
        m_firmwareRevision = QString::fromUtf8(m_deviceInformationService->characteristic(QBluetoothUuid::FirmwareRevisionString).value());
        emit firmwareRevisionChanged();
        m_hardwareRevision = QString::fromUtf8(m_deviceInformationService->characteristic(QBluetoothUuid::HardwareRevisionString).value());
        emit hardwareRevisionChanged();
    });
    m_deviceInformationService->discoverDetails();


    // network service
    connect(m_networkService, &QLowEnergyService::stateChanged, this, [this](QLowEnergyService::ServiceState state){
        if (state != QLowEnergyService::ServiceDiscovered)
            return;
        qDebug() << "Network service discovered";
        QLowEnergyCharacteristic networkCharacteristic = m_networkService->characteristic(networkStatusCharacteristicUuid);
        QLowEnergyCharacteristic networkingEnabledCharacteristic = m_networkService->characteristic(networkingEnabledCharacteristicUuid);
        QLowEnergyCharacteristic wirelessEnabledCharacteristic = m_networkService->characteristic(wirelessEnabledCharacteristicUuid);
        if (!networkCharacteristic.isValid() || !networkingEnabledCharacteristic.isValid() || !wirelessEnabledCharacteristic.isValid()) {
            qWarning() << "Required characteristics not found on remote device (NetworkService)";
            m_btController->disconnectFromDevice();
            return;
        }
        // Enable notifications
        m_networkService->writeDescriptor(networkCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration), QByteArray::fromHex("0100"));
        m_networkService->writeDescriptor(networkingEnabledCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration), QByteArray::fromHex("0100"));
        m_networkService->writeDescriptor(wirelessEnabledCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration), QByteArray::fromHex("0100"));

        m_networkStatus = static_cast<NetworkStatus>(networkCharacteristic.value().toHex().toUInt(nullptr, 16));
        emit networkStatusChanged();
        m_networkingEnabled = networkingEnabledCharacteristic.value().toHex().toUInt(nullptr, 16);
        emit networkingEnabledChanged();
        m_wirelessEnabled = wirelessEnabledCharacteristic.value().toHex().toUInt(nullptr, 16);
        emit wirelessEnabledChanged();

    });
    connect(m_networkService, &QLowEnergyService::characteristicChanged, this, &BtWiFiSetup::characteristicChanged);
    m_networkService->discoverDetails();

    // Wifi service
    connect(m_wifiService, &QLowEnergyService::stateChanged, this, [this](QLowEnergyService::ServiceState state){
        if (state != QLowEnergyService::ServiceDiscovered)
            return;

        qDebug() << "Wifi service discovered";

        // Enable notifations
        m_wifiService->writeDescriptor(m_wifiService->characteristic(wifiResponseCharacteristicUuid).descriptor(QBluetoothUuid::ClientCharacteristicConfiguration), QByteArray::fromHex("0100"));
        m_wifiService->writeDescriptor(m_wifiService->characteristic(wifiStatusCharacteristicUuid).descriptor(QBluetoothUuid::ClientCharacteristicConfiguration), QByteArray::fromHex("0100"));

        if (m_wifiService->characteristic(wifiModeCharacteristicUuid).isValid()) {
            m_wifiService->writeDescriptor(m_wifiService->characteristic(wifiModeCharacteristicUuid).descriptor(QBluetoothUuid::ClientCharacteristicConfiguration), QByteArray::fromHex("0100"));
            m_wirelessDeviceMode = static_cast<WirelessDeviceMode>(m_wifiService->characteristic(wifiModeCharacteristicUuid).value().toHex().toUInt(nullptr, 16));
            emit wirelessDeviceModeChanged();
        }

        qDebug() << "Fetching networks after init";
        loadNetworks();
    });
    connect(m_wifiService, &QLowEnergyService::characteristicChanged, this, &BtWiFiSetup::characteristicChanged);
    m_wifiService->discoverDetails();


    // System service (optional)
    if (m_systemService) {
        connect(m_systemService, &QLowEnergyService::stateChanged, this, [this](QLowEnergyService::ServiceState state){
            if (state != QLowEnergyService::ServiceDiscovered)
                return;
            qDebug() << "System service discovered";
            m_systemService->writeDescriptor(m_systemService->characteristic(systemResponseCharacteristicUuid).descriptor(QBluetoothUuid::ClientCharacteristicConfiguration), QByteArray::fromHex("0100"));
        });
        m_systemService->discoverDetails();
    }
}

void BtWiFiSetup::streamData(QLowEnergyService *service, const QUuid &characteristicUuid, const QVariantMap &request)
{
    QLowEnergyCharacteristic characteristic = service->characteristic(characteristicUuid);
    QByteArray data = QJsonDocument::fromVariant(request).toJson(QJsonDocument::Compact) + '\n';

    int sentDataLength = 0;
    QByteArray remainingData = data;
    while (!remainingData.isEmpty()) {
        QByteArray package = remainingData.left(20);
        sentDataLength += package.count();
        m_wifiService->writeCharacteristic(characteristic, package);
        remainingData = remainingData.remove(0, package.count());
    }
}

void BtWiFiSetup::processWiFiPacket(const QVariantMap &data)
{
    WirelessServiceCommand command = static_cast<WirelessServiceCommand>(data.value("c").toInt());
    WirelessServiceResponse responseCode = (WirelessServiceResponse)data.value("r").toInt();
    if (responseCode != WirelessServiceResponseSuccess) {
        qWarning() << "Error in wifi command" << command << ":" << responseCode;
        emit wifiSetupError();
        return;
    }

    qDebug() << "command reply:" << command;
    switch (command) {
    case WirelessServiceCommandGetNetworks:

        foreach (const QVariant &data, data.value("p").toList()) {
            bool found = false;
            for (int i = 0; i < m_accessPoints->rowCount(); i++) {
                WirelessAccessPoint *existingAp = m_accessPoints->get(i);
                if (existingAp->macAddress() == data.toMap().value("m").toString()) {
                    found = true;
                }
            }
            if (found) {
                continue;
            }

            WirelessAccessPoint *accessPoint = new WirelessAccessPoint(this);
            accessPoint->setSsid(data.toMap().value("e").toString());
            accessPoint->setMacAddress(data.toMap().value("m").toString());
            accessPoint->setSignalStrength(data.toMap().value("s").toInt());
            accessPoint->setProtected(data.toMap().value("p").toBool());
            accessPoint->setHostAddress("");
            m_accessPoints->addWirelessAccessPoint(accessPoint);

        }
        loadCurrentConnection();
        break;
    case WirelessServiceCommandConnect:
        qDebug() << "Connect call succeeded";
        break;
    case WirelessServiceCommandGetCurrentConnection: {
        // Find current network
        if (!data.value("p").toMap().value("m").toString().isEmpty() && data.value("p").toMap().value("i").toString().isEmpty()) {
            // There's a bug in libnymea-networkmanager that sometimes it emits current connection before it actually obtained the IP address
            qDebug() << "Retring to fetch the current connection because IP is not set yet.";
            loadCurrentConnection();
            return;
        }
        m_currentConnection = nullptr;
        QVariantMap currentConnection = data.value("p").toMap();
        QString macAddress = currentConnection.value("m").toString();
        if (m_wirelessDeviceMode == WirelessDeviceModeInfrastructure) {
            foreach (WirelessAccessPoint *accessPoint, m_accessPoints->wirelessAccessPoints()) {
                qDebug() << "Comparing with AP:" << accessPoint->ssid() << accessPoint->macAddress();
                if (accessPoint->macAddress() == macAddress) {
                    // Set the current network
                    m_currentConnection = accessPoint;
                    accessPoint->setHostAddress(currentConnection.value("i").toString());
                    break;
                }
            }
        } else {
            m_currentConnection = new WirelessAccessPoint(this);
            m_currentConnection->setSsid(currentConnection.value("e").toString());
            m_currentConnection->setHostAddress(currentConnection.value("i").toString());
            m_currentConnection->setMacAddress(currentConnection.value("m").toString());
            m_currentConnection->setProtected(currentConnection.value("p").toInt() == 1);
            m_accessPoints->addWirelessAccessPoint(m_currentConnection);
        }
        qDebug() << "current connection is:" << m_currentConnection << qUtf8Printable(QJsonDocument::fromVariant(data).toJson());
        emit currentConnectionChanged();

        if (m_bluetoothStatus != BluetoothStatusLoaded) {
            m_bluetoothStatus = BluetoothStatusLoaded;
            emit bluetoothStatusChanged(m_bluetoothStatus);
        }

        break;
    }
    case WirelessServiceCommandScan:
        if (responseCode == WirelessServiceResponseSuccess) {
            qDebug() << "Fetching networks after wifi scan";
            loadNetworks();
        }
        break;
    default:
        qWarning() << "Unhandled command reply";
    }
}

void BtWiFiSetup::loadNetworks()
{
    QVariantMap request;
    request.insert("c", (int)WirelessServiceCommandGetNetworks);
    streamData(m_wifiService, wifiCommanderCharacteristicUuid, request);
}

void BtWiFiSetup::loadCurrentConnection()
{
    QVariantMap request;
    request.insert("c", (int)WirelessServiceCommandGetCurrentConnection);
    streamData(m_wifiService, wifiCommanderCharacteristicUuid, request);
}

void BtWiFiSetup::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{    
    if (characteristic.uuid() == wifiResponseCharacteristicUuid) {

        m_inputBuffers[characteristic.uuid()].append(value);
        if (!m_inputBuffers[characteristic.uuid()].endsWith("\n")) {
            return;
        }
        QByteArray data = m_inputBuffers.take(characteristic.uuid());
        QJsonParseError error;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data.trimmed(), &error);
        if (error.error != QJsonParseError::NoError) {
            qWarning() << "Invalid json data received:" << error.errorString() << data.trimmed() << "from characteristic:" << characteristic.uuid();
            m_btController->disconnectFromDevice();
            return;
        }
        processWiFiPacket(jsonDoc.toVariant().toMap());

    } else if (characteristic.uuid() == wifiStatusCharacteristicUuid) {

        m_wirelessStatus = static_cast<WirelessStatus>(value.toHex().toInt(nullptr, 16));
        qDebug() << "Wireless status changed" << m_wirelessStatus;
        emit wirelessStatusChanged();

        if (m_wirelessStatus == WirelessStatusFailed) {
            emit wifiSetupError();
        } else if (m_wirelessStatus == WirelessStatusActivated) {
            loadCurrentConnection();
        } else if (m_wirelessStatus == WirelessStatusDisconnected) {
            m_currentConnection = nullptr;
            emit currentConnectionChanged();
        }

        // Note: wirelessEnabled characterristic seems broken server-side. Let's check the wifiStatus for it being enabled or not
        if (m_wirelessEnabled != (m_wirelessStatus != WirelessStatusUnavailable)) {
            m_wirelessEnabled = m_wirelessStatus != WirelessStatusUnavailable;
            emit wirelessEnabledChanged();
        }

    } else if (characteristic.uuid() == networkStatusCharacteristicUuid) {
        m_networkStatus = static_cast<NetworkStatus>(value.toHex().toInt(nullptr, 16));
        qDebug() << "Network status changed:" << m_networkStatus;
        if (m_networkStatus == NetworkStatusGlobal || m_networkStatus == NetworkStatusLocal || m_networkStatus == NetworkStatusConnectedSite) {
            loadCurrentConnection();
        }

        // Note: networkingEnabled characterristic seems broken server-side. Let's check the networkStatus for it being enabled or not
        if (m_wirelessEnabled != (m_networkStatus != NetworkStatusAsleep)) {
            m_networkingEnabled = m_networkStatus != NetworkStatusAsleep;
            emit wirelessEnabledChanged();
        }

    } else if (characteristic.uuid() == wifiModeCharacteristicUuid) {
        qDebug() << "Wireless mode changed:" << value.toHex().toUInt(nullptr, 16);
        m_wirelessDeviceMode = static_cast<WirelessDeviceMode>(value.toHex().toInt(nullptr, 16));
        emit wirelessDeviceModeChanged();

    } else {
        qWarning() << "Unhandled packet from characteristic" << characteristic.uuid();
    }
}
