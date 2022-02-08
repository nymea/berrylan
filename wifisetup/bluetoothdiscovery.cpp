/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
* This project including source code and documentation is protected by
* copyright law, and remains the property of nymea GmbH. All rights, including
* reproduction, publication, editing and translation, are reserved. The use of
* this project is subject to the terms of a license agreement to be concluded
* with nymea GmbH in accordance with the terms of use of nymea GmbH, available
* under https://nymea.io/license
*
* GNU General Public License Usage
* Alternatively, this project may be redistributed and/or modified under the
* terms of the GNU General Public License as published by the Free Software
* Foundation, GNU version 3. This project is distributed in the hope that it
* will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
* Public License for more details.
*
* You should have received a copy of the GNU General Public License along with
* this project. If not, see <https://www.gnu.org/licenses/>.
*
* For any further details and any questions please contact us under
* contact@nymea.io or see our FAQ/Licensing Information on
* https://nymea.io/license/faq
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "bluetoothdiscovery.h"

#include <QDebug>
#include <QTimer>
#include <QBluetoothLocalDevice>
#include <QBluetoothUuid>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(dcBtWiFiSetup, "BtWiFiSetup");

BluetoothDiscovery::BluetoothDiscovery(QObject *parent) :
    QObject(parent),
    m_deviceInfos(new BluetoothDeviceInfos(this))
{

#ifndef Q_OS_IOS
    // Check if bluetooth is available
    QBluetoothLocalDevice localDevice;
    if (!localDevice.isValid()) {
        qCWarning(dcBtWiFiSetup) << "BluetoothDiscovery: there is no bluetooth device available.";
        m_bluetoothAvailable = false;
        return;
    }

    if (localDevice.allDevices().isEmpty()) {
        qCWarning(dcBtWiFiSetup) << "BluetoothDiscovery: there is no bluetooth device available currently.";
        m_bluetoothAvailable = false;
        return;
    }

    m_bluetoothAvailable = true;

    // FIXME: check the device with the most capabilities and check if low energy is available
    QBluetoothHostInfo adapterHostInfo = localDevice.allDevices().first();

    m_localDevice = new QBluetoothLocalDevice(adapterHostInfo.address(), this);
    connect(m_localDevice, &QBluetoothLocalDevice::hostModeStateChanged, this, &BluetoothDiscovery::onBluetoothHostModeChanged);
    onBluetoothHostModeChanged(m_localDevice->hostMode());

#else
    // Note: on iOS there is no QBluetoothLocalDevice available, therefore we have to assume there is one and
    //       start the discovery agent with the default constructor.
    // https://bugreports.qt.io/browse/QTBUG-65547

    m_bluetoothAvailable = true;

    // Always start with assuming BT is enabled
    m_bluetoothEnabled = true;

    qCDebug(dcBtWiFiSetup) << "Initializing Bluetooth";
    onBluetoothHostModeChanged(QBluetoothLocalDevice::HostConnectable);
#endif


}

bool BluetoothDiscovery::bluetoothAvailable() const
{
    return m_bluetoothAvailable;
}

bool BluetoothDiscovery::bluetoothEnabled() const
{
#ifdef Q_OS_IOS
    return m_bluetoothAvailable && m_bluetoothEnabled;
#endif
    qCDebug(dcBtWiFiSetup) << "bluetoothEnabled(): m_bluetoothAvailable:" << m_bluetoothAvailable;
    return m_bluetoothAvailable && m_localDevice->hostMode() != QBluetoothLocalDevice::HostPoweredOff;
}
void BluetoothDiscovery::setBluetoothEnabled(bool bluetoothEnabled) {
    if (!m_bluetoothAvailable) {
        return;
    }
    if (bluetoothEnabled) {
        if (m_localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
            m_localDevice->powerOn();
        }
    } else {
        if (m_localDevice->hostMode() != QBluetoothLocalDevice::HostPoweredOff) {
            m_localDevice->setHostMode(QBluetoothLocalDevice::HostPoweredOff);
        }
    }
}

bool BluetoothDiscovery::discoveryEnabled() const
{
    return m_discoveryEnabled;
}

bool BluetoothDiscovery::discovering() const
{
    return m_discoveryAgent && m_discoveryAgent->isActive();
}

void BluetoothDiscovery::setDiscoveryEnabled(bool discoveryEnabled)
{
    if (m_discoveryEnabled != discoveryEnabled) {
        m_discoveryEnabled = discoveryEnabled;
        emit discoveryEnabledChanged(m_discoveryEnabled);
    }

    if (m_discoveryEnabled) {
        start();
    } else {
        stop();
    }
}

BluetoothDeviceInfos *BluetoothDiscovery::deviceInfos()
{
    return m_deviceInfos;
}

void BluetoothDiscovery::onBluetoothHostModeChanged(const QBluetoothLocalDevice::HostMode &hostMode)
{
    qCDebug(dcBtWiFiSetup) << "BluetoothDiscovery: host mode changed" << hostMode;
    switch (hostMode) {
    case QBluetoothLocalDevice::HostPoweredOff:
        if (m_discoveryAgent) {
            stop();
            m_discoveryAgent->deleteLater();
            m_discoveryAgent = nullptr;
        }
        m_deviceInfos->clearModel();
#ifdef Q_OS_IOS
        m_bluetoothEnabled = false;
#endif
        emit bluetoothEnabledChanged(false);
        break;
    default:
        // Note: discovery works in all other modes
#ifdef Q_OS_IOS
        m_bluetoothEnabled = true;
#endif
        emit bluetoothEnabledChanged(hostMode != QBluetoothLocalDevice::HostPoweredOff);
        if (!m_discoveryAgent) {
#ifdef Q_OS_ANDROID
            m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(m_localDevice->address(), this);
#else
            m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
#endif
            connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothDiscovery::deviceDiscovered);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
            connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceUpdated, this, &BluetoothDiscovery::deviceDiscovered);
#endif
            connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BluetoothDiscovery::discoveryFinished);
            connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &BluetoothDiscovery::discoveryCancelled);
            connect(m_discoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)), this, SLOT(onError(QBluetoothDeviceDiscoveryAgent::Error)));
        }
        if (m_discoveryEnabled && !m_discoveryAgent->isActive()) {
            start();
        }
        break;
    }
}

void BluetoothDiscovery::deviceDiscovered(const QBluetoothDeviceInfo &deviceInfo)
{
    qCDebug(dcBtWiFiSetup()) << "BluetoothDiscovery: Device discovered:" << deviceInfo.name() << deviceInfo.address().toString() << deviceInfo.deviceUuid() << deviceInfo.serviceUuids();
    foreach (BluetoothDeviceInfo *di, m_deviceInfos->deviceInfos()) {
        // Some platforms only provide device UUID (e.g. Apple) and MAC address is 00:00:00:00:00
        // Others provide only a MAC address and the UUID is null.
        // If we have a UUID, use that, otherwise use the MAC for comparison
        if (!deviceInfo.deviceUuid().isNull()) {
            if (di->bluetoothDeviceInfo().deviceUuid() == deviceInfo.deviceUuid()) {
                qCDebug(dcBtWiFiSetup()) << "Updating discovery result (UUID)";
                di->setBluetoothDeviceInfo(deviceInfo);
                return;
            }
        } else {
            if (di->bluetoothDeviceInfo().address() == deviceInfo.address()) {
                qCDebug(dcBtWiFiSetup()) << "Updating discovery result (MAC)";
                di->setBluetoothDeviceInfo(deviceInfo);
                return;
            }
        }
    }

    BluetoothDeviceInfo *deviceInformation = new BluetoothDeviceInfo(deviceInfo);
    qCDebug(dcBtWiFiSetup) << "BluetoothDiscovery: [+]" << deviceInformation->name() << "(" << deviceInformation->address() << ")" << (deviceInformation->isLowEnergy() ? "LE" : "") << deviceInfo.serviceUuids();
    m_deviceInfos->addBluetoothDeviceInfo(deviceInformation);
}

void BluetoothDiscovery::discoveryFinished()
{
    qCDebug(dcBtWiFiSetup) << "BluetoothDiscovery: Discovery finished" << m_discoveryEnabled << this;
    if (m_discoveryEnabled) {
        qCDebug(dcBtWiFiSetup) << "BluetoothDiscovery: Restarting discovery";
        m_discoveryAgent->start();
    }
}

void BluetoothDiscovery::discoveryCancelled()
{
    qCDebug(dcBtWiFiSetup) << "BluetoothDiscovery: Discovery cancelled";
}

void BluetoothDiscovery::onError(const QBluetoothDeviceDiscoveryAgent::Error &error)
{
    qCWarning(dcBtWiFiSetup) << "BluetoothDiscovery: Discovery error:" << error << m_discoveryAgent->errorString();
#ifdef Q_OS_IOS
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError) {
        m_bluetoothEnabled = false;
        emit bluetoothEnabledChanged(false);
        onBluetoothHostModeChanged(QBluetoothLocalDevice::HostPoweredOff);
        QTimer::singleShot(5000, this, [this] () {
            m_bluetoothEnabled = true;
            onBluetoothHostModeChanged(QBluetoothLocalDevice::HostConnectable);
        });
    }
#endif
    emit discoveringChanged();
}

void BluetoothDiscovery::start()
{
    if (!m_discoveryAgent || !bluetoothEnabled()) {
        return;
    }

    if (m_discoveryAgent->isActive()) {
        m_discoveryAgent->stop();
    }

    foreach (const QBluetoothDeviceInfo &info, m_discoveryAgent->discoveredDevices()) {
        qCDebug(dcBtWiFiSetup()) << "Already discovered device:" << info.name();
        deviceDiscovered(info);
    }

    qCDebug(dcBtWiFiSetup) << "BluetoothDiscovery: Starting discovery.";
    m_discoveryAgent->start();
    emit discoveringChanged();
}

void BluetoothDiscovery::stop()
{
    if (!m_discoveryAgent) {
        return;
    }

    qCDebug(dcBtWiFiSetup) << "BluetoothDiscovery: Stop discovering.";
    m_discoveryAgent->stop();
    emit discoveringChanged();
}
