/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Copyright (C) 2018 Simon Stuerz <simon.stuerz@guh.io>                  *
 *                                                                         *
 *  This file is part of nymea:app                                         *
 *                                                                         *
 *  This library is free software; you can redistribute it and/or          *
 *  modify it under the terms of the GNU Lesser General Public             *
 *  License as published by the Free Software Foundation; either           *
 *  version 2.1 of the License, or (at your option) any later version.     *
 *                                                                         *
 *  This library is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *  Lesser General Public License for more details.                        *
 *                                                                         *
 *  You should have received a copy of the GNU Lesser General Public       *
 *  License along with this library; If not, see                           *
 *  <http://www.gnu.org/licenses/>.                                        *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "wirelesssetupmanager.h"
#include "wirelessaccesspoint.h"
#include "wirelessaccesspoints.h"
#include "wirelessaccesspointsproxy.h"

#include <QJsonDocument>

static QBluetoothUuid wifiServiceUuid =                 QBluetoothUuid(QUuid("e081fec0-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid wifiCommanderCharacteristicUuid = QBluetoothUuid(QUuid("e081fec1-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid wifiResponseCharacteristicUuid =  QBluetoothUuid(QUuid("e081fec2-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid wifiStatusCharacteristicUuid =    QBluetoothUuid(QUuid("e081fec3-f757-4449-b9c9-bfa83133f7fc"));

static QBluetoothUuid networkServiceUuid =                  QBluetoothUuid(QUuid("ef6d6610-b8af-49e0-9eca-ab343513641c"));
static QBluetoothUuid networkStatusCharacteristicUuid =     QBluetoothUuid(QUuid("ef6d6611-b8af-49e0-9eca-ab343513641c"));
static QBluetoothUuid networkCommanderCharacteristicUuid =  QBluetoothUuid(QUuid("ef6d6612-b8af-49e0-9eca-ab343513641c"));
static QBluetoothUuid networkResponseCharacteristicUuid =   QBluetoothUuid(QUuid("ef6d6613-b8af-49e0-9eca-ab343513641c"));
static QBluetoothUuid networkingEnabledCharacteristicUuid = QBluetoothUuid(QUuid("ef6d6614-b8af-49e0-9eca-ab343513641c"));
static QBluetoothUuid wirelessEnabledCharacteristicUuid =   QBluetoothUuid(QUuid("ef6d6615-b8af-49e0-9eca-ab343513641c"));

static QBluetoothUuid systemServiceUuid =                 QBluetoothUuid(QUuid("e081fed0-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid systemCommanderCharacteristicUuid = QBluetoothUuid(QUuid("e081fed1-f757-4449-b9c9-bfa83133f7fc"));
static QBluetoothUuid systemResponseCharacteristicUuid =  QBluetoothUuid(QUuid("e081fed2-f757-4449-b9c9-bfa83133f7fc"));


WirelessSetupManager::WirelessSetupManager(const QBluetoothDeviceInfo &deviceInfo, QObject *parent) :
    BluetoothDevice(deviceInfo, parent),
    m_accessPoints(new WirelessAccessPoints(this))
{
    connect(this, &WirelessSetupManager::connectedChanged, this, &WirelessSetupManager::onConnectedChanged);
    connect(this, &WirelessSetupManager::serviceDiscoveryFinished, this, &WirelessSetupManager::onServiceDiscoveryFinished);
}

QString WirelessSetupManager::modelNumber() const
{
    return m_modelNumber;
}

QString WirelessSetupManager::manufacturer() const
{
    return m_manufacturer;
}

QString WirelessSetupManager::softwareRevision() const
{
    return m_softwareRevision;
}

QString WirelessSetupManager::firmwareRevision() const
{
    return m_firmwareRevision;
}

QString WirelessSetupManager::hardwareRevision() const
{
    return m_hardwareRevision;
}

bool WirelessSetupManager::initialized() const
{
    return m_initialized;
}

bool WirelessSetupManager::initializing() const
{
    return m_initializing;
}

bool WirelessSetupManager::working() const
{
    return m_working;
}

WirelessSetupManager::NetworkStatus WirelessSetupManager::networkStatus() const
{
    return m_networkStatus;
}

WirelessSetupManager::WirelessStatus WirelessSetupManager::wirelessStatus() const
{
    return m_wirelessStatus;
}

bool WirelessSetupManager::networkingEnabled() const
{
    return m_networkingEnabled;
}

bool WirelessSetupManager::wirelessEnabled() const
{
    return m_wirelessEnabled;
}

WirelessAccessPoints *WirelessSetupManager::accessPoints() const
{
    return m_accessPoints;
}

void WirelessSetupManager::reloadData()
{
    loadNetworks();
}

void WirelessSetupManager::loadNetworks()
{
    qDebug() << "WifiSetupManager: Start loading wifi networks";

    if (!m_wifiService) {
        qWarning() << "WifiSetupManager: Could not send command. Service not valid";
        return;
    }

    QLowEnergyCharacteristic characteristic = m_wifiService->characteristic(wifiCommanderCharacteristicUuid);
    if (!characteristic.isValid()) {
        qWarning() << "WifiSetupManager: Could not send command. Characteristic is not valid";
        return;
    }

    m_readingResponse = true;
    m_inputDataStream.clear();

    setStatusText("WifiSetupManager: Loading wifi network list...");
    setWorking(true);

    QVariantMap request;
    request.insert("c", (int)WirelessServiceCommandGetNetworks);
    streamData(request);
}

void WirelessSetupManager::loadCurrentConnection()
{
    qDebug() << "WifiSetupManager: Start loading current connection data";

    if (!m_wifiService) {
        qWarning() << "WifiSetupManager: Could not send command. Service not valid";
        return;
    }

    QLowEnergyCharacteristic characteristic = m_wifiService->characteristic(wifiCommanderCharacteristicUuid);
    if (!characteristic.isValid()) {
        qWarning() << "WifiSetupManager: Could not send command. Characteristic is not valid";
        return;
    }

    m_readingResponse = false;
    m_inputDataStream.clear();

    setStatusText("WifiSetupManager: Loading current connection data");
    setWorking(true);

    QVariantMap request;
    request.insert("c", (int)WirelessServiceCommandGetCurrentConnection);
    streamData(request);
}

void WirelessSetupManager::performWifiScan()
{
    qDebug() << "WifiSetupManager: Start loading wifi networks";

    if (!m_wifiService) {
        qWarning() << "WifiSetupManager: Could not send command. Service not valid";
        return;
    }

    QLowEnergyCharacteristic characteristic = m_wifiService->characteristic(wifiCommanderCharacteristicUuid);
    if (!characteristic.isValid()) {
        qWarning() << "WifiSetupManager: Could not send command. Characteristic is not valid";
        return;
    }

    setStatusText("WifiSetupManager: Perform refresh...");
    setWorking(true);

    QVariantMap request;
    request.insert("c", (int)WirelessServiceCommandScan);
    streamData(request);
}

void WirelessSetupManager::enableNetworking(bool enable)
{
    qDebug() << "WifiSetupManager: Send networking" << enable;

    if (!m_netwokService) {
        qWarning() << "WifiSetupManager: Could not set networking. Service not valid";
        return;
    }

    QLowEnergyCharacteristic characteristic = m_netwokService->characteristic(networkCommanderCharacteristicUuid);
    if (!characteristic.isValid()) {
        qWarning() << "WifiSetupManager: Could not set networking. Characteristic is not valid";
        return;
    }

    m_netwokService->writeCharacteristic(characteristic, enable ? QByteArray::fromHex("00") : QByteArray::fromHex("01"));
}

void WirelessSetupManager::enableWireless(bool enable)
{
    qDebug() << "WifiSetupManager: Send wireless networking" << enable;

    if (!m_netwokService) {
        qWarning() << "WifiSetupManager: Could not enable/disable wireless. Service not valid";
        return;
    }

    QLowEnergyCharacteristic characteristic = m_netwokService->characteristic(networkCommanderCharacteristicUuid);
    if (!characteristic.isValid()) {
        qWarning() << "WifiSetupManager: Could not enable/disable wireless. Characteristic is not valid";
        return;
    }

    m_netwokService->writeCharacteristic(characteristic, enable ? QByteArray::fromHex("02") : QByteArray::fromHex("03"));
}

void WirelessSetupManager::connectWirelessNetwork(const QString &ssid, const QString &password)
{
    qDebug() << "WifiSetupManager: Connect wireless network" << ssid << password;

    m_ssid = ssid;
    m_password = password;

    if (!m_wifiService) {
        qWarning() << "WifiSetupManager: Could not set wireless network. Service not valid";
        return;
    }

    QLowEnergyCharacteristic ssidCharacteristic = m_wifiService->characteristic(wifiCommanderCharacteristicUuid);
    if (!ssidCharacteristic.isValid()) {
        qWarning() << "WifiSetupManager: Could not connect. Characteristic is not valid";
        return;
    }

    QVariantMap request;
    request.insert("c", (int)WirelessServiceCommandConnect);
    QVariantMap parameters;
    parameters.insert("e", ssid);
    parameters.insert("p", password);
    request.insert("p", parameters);

    streamData(request);
}

void WirelessSetupManager::disconnectWirelessNetwork()
{
    qDebug() << "WifiSetupManager: Disconnect wireless network";

    if (!m_wifiService) {
        qWarning() << "WifiSetupManager: Could not disconnect wireless network. Service not valid";
        return;
    }

    QLowEnergyCharacteristic ssidCharacteristic = m_wifiService->characteristic(wifiCommanderCharacteristicUuid);
    if (!ssidCharacteristic.isValid()) {
        qWarning() << "WifiSetupManager: Could not disconnect wireless ssid. Characteristic is not valid";
        return;
    }

    QVariantMap request;
    request.insert("c", (int)WirelessServiceCommandDisconnect);
    streamData(request);
}

void WirelessSetupManager::pressPushButton()
{
    qDebug() << "WifiSetupManager: Press push button";

    if (!m_systemService) {
        qWarning() << "WifiSetupManager: Could not press push button. Service not valid";
        return;
    }

    QLowEnergyCharacteristic commanderCharacteristic = m_systemService->characteristic(systemCommanderCharacteristicUuid);
    if (!commanderCharacteristic.isValid()) {
        qWarning() << "WifiSetupManager: Could not press push button. Characteristic is not valid";
        return;
    }

    QVariantMap request;
    request.insert("c", (int)SystemServiceCommandPushAuthentication);

    QByteArray data = QJsonDocument::fromVariant(request).toJson(QJsonDocument::Compact) + '\n';
    qDebug() << "WifiSetupManager: SystemService: Start streaming response data:" << data.count() << "bytes";

    int sentDataLength = 0;
    QByteArray remainingData = data;
    while (!remainingData.isEmpty()) {
        QByteArray package = remainingData.left(20);
        sentDataLength += package.count();
        m_systemService->writeCharacteristic(commanderCharacteristic, package);
        remainingData = remainingData.remove(0, package.count());
    }

    qDebug() << "WifiSetupManager: SystemService: Finished streaming request data";
}

void WirelessSetupManager::checkInitialized()
{
    bool initialized = false;

    if (m_systemService) {
        initialized = m_deviceInformationService->state() == QLowEnergyService::ServiceDiscovered
                && m_netwokService->state() == QLowEnergyService::ServiceDiscovered
                && m_wifiService->state() == QLowEnergyService::ServiceDiscovered
                && m_systemService->state() == QLowEnergyService::ServiceDiscovered;
    } else {
        initialized = m_deviceInformationService->state() == QLowEnergyService::ServiceDiscovered
                && m_netwokService->state() == QLowEnergyService::ServiceDiscovered
                && m_wifiService->state() == QLowEnergyService::ServiceDiscovered;
    }

    if (initialized && m_wirelessEnabled && m_networkingEnabled) {
        loadNetworks();
    }

    setInitialized(initialized);
}

void WirelessSetupManager::setModelNumber(const QString &modelNumber)
{
    m_modelNumber = modelNumber;
    emit modelNumberChanged();
}

void WirelessSetupManager::setManufacturer(const QString &manufacturer)
{
    m_manufacturer = manufacturer;
    emit manufacturerChanged();
}

void WirelessSetupManager::setSoftwareRevision(const QString &softwareRevision)
{
    m_softwareRevision = softwareRevision;
    emit softwareRevisionChanged();
}

void WirelessSetupManager::setFirmwareRevision(const QString &firmwareRevision)
{
    m_firmwareRevision = firmwareRevision;
    emit firmwareRevisionChanged();
}

void WirelessSetupManager::setHardwareRevision(const QString &hardwareRevision)
{    
    m_hardwareRevision = hardwareRevision;
    emit hardwareRevisionChanged();
}

void WirelessSetupManager::setInitializing(bool initializing)
{
    if (m_initializing == initializing)
        return;

    qDebug() << "WifiSetupManager:" << (initializing ? "initializing" : "not initializing");
    m_initializing = initializing;
    emit initializingChanged();
}

void WirelessSetupManager::setInitialized(bool initialized)
{
    if (m_initialized == initialized)
        return;

    qDebug() << "WifiSetupManager:" << (initialized ? "initialized" : "not initialized");
    m_initialized = initialized;
    emit initializedChanged();
}

void WirelessSetupManager::setWorking(bool working)
{
    if (m_working == working)
        return;

    qDebug() << "WifiSetupManager:" << (working ? "working" : "not working");
    m_working = working;
    emit workingChanged();
}

void WirelessSetupManager::setNetworkStatus(int networkStatus)
{
    if (m_networkStatus == networkStatus)
        return;

    m_networkStatus = static_cast<NetworkStatus>(networkStatus);
    qDebug() << "-->" << m_networkStatus;
    emit networkStatusChanged();
}

void WirelessSetupManager::setWirelessStatus(int wirelessStatus)
{
    if (m_wirelessStatus == wirelessStatus)
        return;

    m_wirelessStatus = static_cast<WirelessStatus>(wirelessStatus);
    qDebug() << "-->" << m_wirelessStatus;
    emit wirelessStatusChanged();
}

void WirelessSetupManager::setNetworkingEnabled(bool networkingEnabled)
{
    if (m_networkingEnabled == networkingEnabled)
        return;

    qDebug() << "WifiSetupManager: Networking enabled changed" << networkingEnabled;
    m_networkingEnabled = networkingEnabled;
    emit networkingEnabledChanged();

    if (!m_networkingEnabled)
        m_accessPoints->clearModel();

}

void WirelessSetupManager::setWirelessEnabled(bool wirelessEnabled)
{
    if (m_wirelessEnabled == wirelessEnabled)
        return;

    qDebug() << "WifiSetupManager: Wireless enabled changed" << wirelessEnabled;
    m_wirelessEnabled = wirelessEnabled;
    emit wirelessEnabledChanged();

    if (!m_wirelessEnabled)
        m_accessPoints->clearModel();

}

void WirelessSetupManager::streamData(const QVariantMap &request)
{
    QLowEnergyCharacteristic characteristic = m_wifiService->characteristic(wifiCommanderCharacteristicUuid);
    if (!characteristic.isValid()) {
        qWarning() << "WifiSetupManager: WirelessService: Wireless commander characteristic not valid";
        return;
    }

    QByteArray data = QJsonDocument::fromVariant(request).toJson(QJsonDocument::Compact) + '\n';
    qDebug() << "WifiSetupManager: WirelessService: Start streaming response data:" << data.count() << "bytes";

    int sentDataLength = 0;
    QByteArray remainingData = data;
    while (!remainingData.isEmpty()) {
        QByteArray package = remainingData.left(20);
        sentDataLength += package.count();
        m_wifiService->writeCharacteristic(characteristic, package);
        remainingData = remainingData.remove(0, package.count());
    }

    qDebug() << "WifiSetupManager: WirelessService: Finished streaming request data";
}

void WirelessSetupManager::processNetworkResponse(const QVariantMap &response)
{
    if (!response.contains("c") || !response.contains("r")) {
        qWarning() << "WifiSetupManager: Got invalid response map.";
        return;
    }

    NetworkServiceCommand command = (NetworkServiceCommand)response.value("c").toInt();
    NetworkServiceResponse responseCode = (NetworkServiceResponse)response.value("r").toInt();

    if (responseCode != NetworkServiceResponseSuccess) {
        qWarning() << "WifiSetupManager: Got error for command" << command << responseCode;


        switch (responseCode) {
        case NetworkServiceResponseIvalidValue:
            emit errorOccurred(tr("Invalid value."));
            break;
        case NetworkServiceResponseNetworkManagerNotAvailable:
            emit errorOccurred(tr("There is no networkmanager available on the device."));
            break;
        case NetworkServiceResponseWirelessNotAvailable:
            emit errorOccurred(tr("There is no wireless device available on the device."));
            break;
        default:
            emit errorOccurred(tr("Unknown error occurred."));
            break;
        }

        return;
    }

    qDebug() << "WifiSetupManager: Network command response" << command << responseCode;
}

void WirelessSetupManager::processWifiResponse(const QVariantMap &response)
{
    setWorking(false);

    if (!response.contains("c") || !response.contains("r")) {
        qWarning() << "WifiSetupManager: Got invalid response map.";
        return;
    }

    WirelessServiceCommand command = (WirelessServiceCommand)response.value("c").toInt();
    WirelessServiceResponse responseCode = (WirelessServiceResponse)response.value("r").toInt();

    if (responseCode != WirelessServiceResponseSuccess) {
        qWarning() << "WifiSetupManager: Got error for command" << command << responseCode;

        switch (responseCode) {
        case WirelessServiceResponseIvalidCommand:
            emit errorOccurred(tr("Invalid command."));
            break;
        case WirelessServiceResponseIvalidParameters:
            emit errorOccurred(tr("Invalid parameters."));
            break;
        case WirelessServiceResponseNetworkManagerNotAvailable:
            emit errorOccurred(tr("There is no networkmanager available on the device."));
            break;
        case WirelessServiceResponseWirelessNotAvailable:
            emit errorOccurred(tr("There is no wireless device available on the device."));
            break;
        case WirelessServiceResponseWirelessNotEnabled:
            emit errorOccurred(tr("The wireless networking is disabled on the device."));
            break;
        case WirelessServiceResponseNetworkingNotEnabled:
            emit errorOccurred(tr("The networking is disabled on the device."));
            break;
        default:
            emit errorOccurred(tr("Unknown error occurred."));
            break;
        }

        return;
    }

    switch (command) {
    case WirelessServiceCommandGetNetworks: {
        if (!response.contains("p")) {
            qWarning() << "WifiSetupManager: Missing parameters in response.";
            return;
        }

        m_accessPointsVariantList = response.value("p").toList();
        m_accessPoints->clearModel();

        foreach (const QVariant &accessPointVariant, m_accessPointsVariantList) {
            QVariantMap accessPointVariantMap = accessPointVariant.toMap();
            WirelessAccessPoint *accessPoint = new WirelessAccessPoint(this);
            accessPoint->setSsid(accessPointVariantMap.value("e").toString());
            accessPoint->setMacAddress(accessPointVariantMap.value("m").toString());
            accessPoint->setSignalStrength(accessPointVariantMap.value("s").toInt());
            accessPoint->setProtected(accessPointVariantMap.value("p").toBool());
            accessPoint->setHostAddress("");

            m_accessPoints->addWirelessAccessPoint(accessPoint);
        }

        loadCurrentConnection();

        break;
    }
    case WirelessServiceCommandConnect:

        break;
    case WirelessServiceCommandConnectHidden:

        break;
    case WirelessServiceCommandDisconnect:

        break;
    case WirelessServiceCommandGetCurrentConnection: {

        qDebug() << "Current network connection" << response;
        QVariantMap currentConnection = response.value("p").toMap();;

        // Find current network
        QString macAddress = currentConnection.value("m").toString();
        foreach (WirelessAccessPoint *accessPoint, m_accessPoints->wirelessAccessPoints()) {
            if (accessPoint->macAddress() == macAddress) {
                // Set the current network
                m_currentConnection = accessPoint;
                emit currentConnectionChanged();
                accessPoint->setHostAddress(currentConnection.value("i").toString());
            }
        }
        break;
    }
    default:
        break;
    }
}

void WirelessSetupManager::processSystemResponse(const QVariantMap &response)
{
    setWorking(false);

    if (!response.contains("c") || !response.contains("r")) {
        qWarning() << "WifiSetupManager: Got invalid response map.";
        return;
    }

    SystemServiceCommand command = (SystemServiceCommand)response.value("c").toInt();
    SystemServiceResponse responseCode = (SystemServiceResponse)response.value("r").toInt();

    if (responseCode != SystemServiceResponseSuccess) {
        qWarning() << "WifiSetupManager: Got error for command" << command << responseCode;
        return;
    }

    qDebug() << "WifiSetupManager: System command response" << command << responseCode;
}

void WirelessSetupManager::onConnectedChanged()
{
    if (!connected()) {
        // Clean up
        qDebug() << "WifiSetupManager: Clean up services";
        if (m_deviceInformationService) {
            m_deviceInformationService->deleteLater();
        }
        if (m_netwokService) {
            m_netwokService->deleteLater();
        }
        if (m_wifiService) {
            m_wifiService->deleteLater();
        }
        if (m_systemService) {
            m_systemService->deleteLater();
        }

        m_deviceInformationService = nullptr;
        m_netwokService = nullptr;
        m_wifiService = nullptr;
        m_systemService = nullptr;

        m_accessPoints->clearModel();

        setInitialized(false);
        setInitializing(false);
        setWorking(false);

        setManufacturer("");
        setModelNumber("");
        setSoftwareRevision("");
        setFirmwareRevision("");
        setHardwareRevision("");
    }
}

void WirelessSetupManager::onServiceDiscoveryFinished()
{
    setInitializing(true);

    foreach (const QBluetoothUuid &serviceUuid, controller()->services()) {
        qDebug() << "WifiSetupManager: -->" << serviceUuid.toString();
    }

    if (!controller()->services().contains(QBluetoothUuid::DeviceInformation)) {
        qWarning() << "WifiSetupManager: Could not find device information service";
        controller()->disconnectFromDevice();
        return;
    }

    if (!controller()->services().contains(networkServiceUuid)) {
        qWarning() << "WifiSetupManager: Could not find network service";
        controller()->disconnectFromDevice();
        return;
    }

    if (!controller()->services().contains(wifiServiceUuid)) {
        qWarning() << "WifiSetupManager: Could not find wifi service";
        controller()->disconnectFromDevice();
        return;
    }

    // Device Informantion service
    if (!m_deviceInformationService) {
        m_deviceInformationService = controller()->createServiceObject(QBluetoothUuid::DeviceInformation, this);
        if (!m_deviceInformationService) {
            qWarning() << "WifiSetupManager: Could not create temperature service.";
            controller()->disconnectFromDevice();
            return;
        }

        connect(m_deviceInformationService, &QLowEnergyService::stateChanged, this, &WirelessSetupManager::onDeviceInformationStateChanged);
        connect(m_deviceInformationService, &QLowEnergyService::characteristicChanged, this, &WirelessSetupManager::onDeviceInformationCharacteristicChanged);
        connect(m_deviceInformationService, &QLowEnergyService::characteristicRead, this, &WirelessSetupManager::onDeviceInformationCharacteristicChanged);

        if (m_deviceInformationService->state() == QLowEnergyService::DiscoveryRequired) {
            qDebug() << "WifiSetupManager: start discovering device information service...";
            m_deviceInformationService->discoverDetails();
        }

    }
}

void WirelessSetupManager::onDeviceInformationStateChanged(const QLowEnergyService::ServiceState &state)
{

    if (state != QLowEnergyService::ServiceDiscovered)
        return;

    qDebug() << "WifiSetupManager: Device information service discovered.";

    foreach (const QLowEnergyCharacteristic &characteristic, m_deviceInformationService->characteristics()) {
        qDebug() << "    -->" << characteristic.name() << characteristic.uuid().toString() << characteristic.value();
        foreach (const QLowEnergyDescriptor &descriptor, characteristic.descriptors()) {
            qDebug() << "        -->" << descriptor.name() << descriptor.uuid().toString() << descriptor.value();
        }
    }

    setManufacturer(QString::fromUtf8(m_deviceInformationService->characteristic(QBluetoothUuid::ManufacturerNameString).value()));
    setModelNumber(QString::fromUtf8(m_deviceInformationService->characteristic(QBluetoothUuid::ModelNumberString).value()));
    setSoftwareRevision(QString::fromUtf8(m_deviceInformationService->characteristic(QBluetoothUuid::SoftwareRevisionString).value()));
    setFirmwareRevision(QString::fromUtf8(m_deviceInformationService->characteristic(QBluetoothUuid::FirmwareRevisionString).value()));
    setHardwareRevision(QString::fromUtf8(m_deviceInformationService->characteristic(QBluetoothUuid::HardwareRevisionString).value()));

    // Network service
    if (!m_netwokService) {
        m_netwokService = controller()->createServiceObject(networkServiceUuid, this);
        if (!m_netwokService) {
            qWarning() << "WifiSetupManager: Could not create network service.";
            controller()->disconnectFromDevice();
            return;
        }

        connect(m_netwokService, &QLowEnergyService::stateChanged, this, &WirelessSetupManager::onNetworkServiceStateChanged);
        connect(m_netwokService, &QLowEnergyService::characteristicChanged, this, &WirelessSetupManager::onNetworkServiceCharacteristicChanged);
        connect(m_netwokService, &QLowEnergyService::characteristicRead, this, &WirelessSetupManager::onNetworkServiceReadFinished);

        if (m_netwokService->state() == QLowEnergyService::DiscoveryRequired) {
            qDebug() << "WifiSetupManager: start discovering network service...";
            m_netwokService->discoverDetails();
        }
    }
    checkInitialized();
}

void WirelessSetupManager::onDeviceInformationCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    Q_UNUSED(characteristic)
    Q_UNUSED(value)
}

void WirelessSetupManager::onDeviceInformationReadFinished(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    Q_UNUSED(characteristic)
    Q_UNUSED(value)
}

void WirelessSetupManager::onNetworkServiceStateChanged(const QLowEnergyService::ServiceState &state)
{
    if (state != QLowEnergyService::ServiceDiscovered)
        return;

    qDebug() << "WifiSetupManager: Network service discovered.";

    foreach (const QLowEnergyCharacteristic &characteristic, m_netwokService->characteristics()) {
        qDebug() << "    -->" << characteristic.name() << characteristic.uuid().toString() << characteristic.value();
        foreach (const QLowEnergyDescriptor &descriptor, characteristic.descriptors()) {
            qDebug() << "        -->" << descriptor.name() << descriptor.uuid().toString() << descriptor.value();
        }
    }

    QLowEnergyCharacteristic networkCharacteristic = m_netwokService->characteristic(networkStatusCharacteristicUuid);
    if (!networkCharacteristic.isValid()) {
        qWarning() << "Invalud networking status characteristic";
        return;
    }

    QLowEnergyCharacteristic networkingEnabledCharacteristic = m_netwokService->characteristic(networkingEnabledCharacteristicUuid);
    if (!networkingEnabledCharacteristic.isValid()) {
        qWarning() << "Invalud networking enabled characteristic";
        return;
    }

    QLowEnergyCharacteristic wirelessEnabledCharacteristic = m_netwokService->characteristic(wirelessEnabledCharacteristicUuid);
    if (!wirelessEnabledCharacteristic.isValid()) {
        qWarning() << "Invalud wireless enabled characteristic";
        return;
    }


    // Enable notifications
    qDebug() << "Enable notifications of network service";
    m_netwokService->writeDescriptor(networkCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration), QByteArray::fromHex("0100"));
    m_netwokService->writeDescriptor(networkingEnabledCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration), QByteArray::fromHex("0100"));
    m_netwokService->writeDescriptor(wirelessEnabledCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration), QByteArray::fromHex("0100"));

    setStatusText("Connected and ready");
    setWorking(false);

    // Done with discovery
    setNetworkStatus(networkCharacteristic.value().toHex().toUInt(0, 16));
    setNetworkingEnabled((bool)networkingEnabledCharacteristic.value().toHex().toUInt(0, 16));
    setWirelessEnabled((bool)wirelessEnabledCharacteristic.value().toHex().toUInt(0, 16));

    // Wifi service
    if (!m_wifiService) {
        m_wifiService = controller()->createServiceObject(wifiServiceUuid, this);
        if (!m_wifiService) {
            qWarning() << "WifiSetupManager: Could not create wifi service.";
            controller()->disconnectFromDevice();
            return;
        }

        connect(m_wifiService, &QLowEnergyService::stateChanged, this, &WirelessSetupManager::onWifiServiceStateChanged);
        connect(m_wifiService, &QLowEnergyService::characteristicChanged, this, &WirelessSetupManager::onWifiServiceCharacteristicChanged);
        connect(m_wifiService, &QLowEnergyService::characteristicRead, this, &WirelessSetupManager::onWifiServiceReadFinished);

        if (m_wifiService->state() == QLowEnergyService::DiscoveryRequired) {
            qDebug() << "WifiSetupManager: start discovering wifi service...";
            m_wifiService->discoverDetails();
        }

    }

    checkInitialized();
}

void WirelessSetupManager::onNetworkServiceCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    if (characteristic.uuid() == networkStatusCharacteristicUuid) {
        qDebug() << "Network status changed:" << value;
        setNetworkStatus(value.toHex().toInt(nullptr, 16));
        if (m_networkStatus == NetworkStatusGlobal) {
            loadCurrentConnection();
        }
        return;
    } else if (characteristic.uuid() == networkResponseCharacteristicUuid) {
        // Check if currently reading
        if (m_readingResponse) {
            m_inputDataStream.append(value);
        } else {
            m_inputDataStream.clear();
            m_readingResponse = true;
            m_inputDataStream.append(value);
        }

        // If command finished
        if (value.endsWith('\n')) {
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(m_inputDataStream, &error);
            if (error.error != QJsonParseError::NoError) {
                qWarning() << "Got invalid json object" << m_inputDataStream;
                m_inputDataStream.clear();
                m_readingResponse = false;
                return;
            }

            qDebug() << "Got command stream" << qUtf8Printable(jsonDocument.toJson());

            processNetworkResponse(jsonDocument.toVariant().toMap());

            m_inputDataStream.clear();
            m_readingResponse = false;
            return;
        }
    } else if (characteristic.uuid() == networkingEnabledCharacteristicUuid) {
        qDebug() << "Networking enabled changed" << (bool)value.toHex().toUInt(0, 16);
        setNetworkingEnabled((bool)value.toHex().toUInt(0, 16));
        return;
    } else if (characteristic.uuid() == wirelessEnabledCharacteristicUuid) {
        qDebug() << "Wireless enabled changed" << (bool)value.toHex().toUInt(0, 16);
        setWirelessEnabled((bool)value.toHex().toUInt(0, 16));
        return;
    }

    qWarning() << "Bluetooth: Unhandled service characteristic changed" << characteristic.uuid() << value;
}

void WirelessSetupManager::onNetworkServiceReadFinished(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    Q_UNUSED(characteristic)
    Q_UNUSED(value)
}

void WirelessSetupManager::onWifiServiceStateChanged(const QLowEnergyService::ServiceState &state)
{
    if (state != QLowEnergyService::ServiceDiscovered)
        return;

    qDebug() << "WifiSetupManager: Wifi service discovered.";

    foreach (const QLowEnergyCharacteristic &characteristic, m_wifiService->characteristics()) {
        qDebug() << "    -->" << characteristic.name() << characteristic.uuid().toString() << characteristic.value();
        foreach (const QLowEnergyDescriptor &descriptor, characteristic.descriptors()) {
            qDebug() << "        -->" << descriptor.name() << descriptor.uuid().toString() << descriptor.value();
        }
    }

    // Enable notifications
    m_wifiService->writeDescriptor(m_wifiService->characteristic(wifiResponseCharacteristicUuid).descriptor(QBluetoothUuid::ClientCharacteristicConfiguration), QByteArray::fromHex("0100"));
    m_wifiService->writeDescriptor(m_wifiService->characteristic(wifiStatusCharacteristicUuid).descriptor(QBluetoothUuid::ClientCharacteristicConfiguration), QByteArray::fromHex("0100"));

    setWirelessStatus(m_wifiService->characteristic(wifiStatusCharacteristicUuid).value().toHex().toUInt(0, 16));

    // System service
    if (!m_systemService) {
        m_systemService = controller()->createServiceObject(systemServiceUuid, this);
        if (!m_systemService) {
            qWarning() << "WifiSetupManager: Could not create system service. Looks like this networkmanager has not implemented that.";
            //controller()->disconnectFromDevice();
        } else {
            connect(m_systemService, &QLowEnergyService::stateChanged, this, &WirelessSetupManager::onSystemServiceStateChanged);
            connect(m_systemService, &QLowEnergyService::characteristicChanged, this, &WirelessSetupManager::onSystemServiceCharacteristicChanged);
            connect(m_systemService, &QLowEnergyService::characteristicRead, this, &WirelessSetupManager::onSystemServiceReadFinished);

            if (m_systemService->state() == QLowEnergyService::DiscoveryRequired) {
                qDebug() << "WifiSetupManager: start discovering system service...";
                m_systemService->discoverDetails();
            }
        }
    }

    checkInitialized();
}

void WirelessSetupManager::onWifiServiceCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    Q_UNUSED(characteristic)

    if (characteristic.uuid() == wifiResponseCharacteristicUuid) {
        // Check if currently reading
        if (m_readingResponse) {
            m_inputDataStream.append(value);
        } else {
            m_inputDataStream.clear();
            m_readingResponse = true;
            m_inputDataStream.append(value);
        }

        // If command finished
        if (value.endsWith('\n')) {
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson(m_inputDataStream, &error);
            if (error.error != QJsonParseError::NoError) {
                qWarning() << "Got invalid json object" << m_inputDataStream;
                m_inputDataStream.clear();
                m_readingResponse = false;
                return;
            }

            qDebug() << "Got command stream" << qUtf8Printable(jsonDocument.toJson());

            processWifiResponse(jsonDocument.toVariant().toMap());

            m_inputDataStream.clear();
            m_readingResponse = false;
        }
        return;
    }

    if (characteristic.uuid() == wifiStatusCharacteristicUuid) {
        qDebug() << "Wireless status changed:" << value.toHex().toUInt(nullptr, 16) << "Old status:" << m_wirelessStatus;
        setWirelessStatus(value.toHex().toInt(nullptr, 16));
        if (m_wirelessStatus == WirelessStatusActivated) {
            loadCurrentConnection();
        }
        return;
    }

    qWarning() << "Bluetooth: Unhandled service characteristic changed" << characteristic.uuid() << value;

}

void WirelessSetupManager::onWifiServiceReadFinished(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    Q_UNUSED(characteristic)
    Q_UNUSED(value)
}

void WirelessSetupManager::onSystemServiceStateChanged(const QLowEnergyService::ServiceState &state)
{
    if (state != QLowEnergyService::ServiceDiscovered)
        return;

    qDebug() << "WifiSetupManager: System service discovered.";

    foreach (const QLowEnergyCharacteristic &characteristic, m_systemService->characteristics()) {
        qDebug() << "    -->" << characteristic.name() << characteristic.uuid().toString() << characteristic.value();
        foreach (const QLowEnergyDescriptor &descriptor, characteristic.descriptors()) {
            qDebug() << "        -->" << descriptor.name() << descriptor.uuid().toString() << descriptor.value();
        }
    }

    // Enable notifications
    m_systemService->writeDescriptor(m_systemService->characteristic(systemResponseCharacteristicUuid).descriptor(QBluetoothUuid::ClientCharacteristicConfiguration), QByteArray::fromHex("0100"));

    checkInitialized();
}

void WirelessSetupManager::onSystemServiceCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    Q_UNUSED(characteristic)

    // Check if currently reading
    if (m_readingResponse) {
        m_inputDataStream.append(value);
    } else {
        m_inputDataStream.clear();
        m_readingResponse = true;
        m_inputDataStream.append(value);
    }

    // If command finished
    if (value.endsWith('\n')) {
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(m_inputDataStream, &error);
        if (error.error != QJsonParseError::NoError) {
            qWarning() << "Got invalid json object" << m_inputDataStream;
            m_inputDataStream.clear();
            m_readingResponse = false;
            return;
        }

        qDebug() << "Got command stream" << qUtf8Printable(jsonDocument.toJson());

        processSystemResponse(jsonDocument.toVariant().toMap());

        m_inputDataStream.clear();
        m_readingResponse = false;
    }
}

void WirelessSetupManager::onSystemServiceReadFinished(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    Q_UNUSED(characteristic)
    Q_UNUSED(value)
}
