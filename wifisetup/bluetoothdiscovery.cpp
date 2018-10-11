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

#include "bluetoothdiscovery.h"

#include <QDebug>
#include <QTimer>
#include <QBluetoothLocalDevice>

BluetoothDiscovery::BluetoothDiscovery(QObject *parent) :
    QObject(parent),
    m_deviceInfos(new BluetoothDeviceInfos(this))
{

#ifndef Q_OS_IOS
    // Check if bluetooth is available
    QBluetoothLocalDevice localDevice;
    if (!localDevice.isValid()) {
        qWarning() << "BluetoothDiscovery: there is no bluetooth device available.";
        m_bluetoothAvailable = false;
        return;
    }

    if (localDevice.allDevices().isEmpty()) {
        qWarning() << "BluetoothDiscovery: there is no bluetooth device available currently.";
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

    qDebug() << "Initializing Bluetooth";
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
    qDebug() << "bluetoothEnabled(): m_bluetoothAvailable:" << m_bluetoothAvailable;
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
    if (m_discoveryEnabled == discoveryEnabled) {
        return;
    }
    m_discoveryEnabled = discoveryEnabled;
    emit discoveringChanged();

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
    qDebug() << "BluetoothDiscovery: host mode changed" << hostMode;
    switch (hostMode) {
    case QBluetoothLocalDevice::HostPoweredOff:
        if (m_discoveryAgent) {
            stop();
            m_discoveryAgent->deleteLater();
            m_discoveryAgent = nullptr;
        }
        m_deviceInfos->clearModel();
        emit bluetoothEnabledChanged(false);
        break;
    default:
        // Note: discovery works in all other modes
        emit bluetoothEnabledChanged(hostMode != QBluetoothLocalDevice::HostPoweredOff);
        if (!m_discoveryAgent) {
#ifdef Q_OS_ANDROID
            m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(m_localDevice->address(), this);
#else
            m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
#endif
            connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothDiscovery::deviceDiscovered);
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
    if (!deviceInfo.isValid())
        return;

    BluetoothDeviceInfo *deviceInformation = new BluetoothDeviceInfo(deviceInfo);
    bool isLowEnergy = deviceInfo.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration;

    qDebug() << "BluetoothDiscovery: [+]" << deviceInformation->name() << "(" << deviceInformation->address() << ")" << (isLowEnergy ? "LE" : "");

    if (!isLowEnergy || deviceInformation->name().isEmpty()) {
        delete deviceInformation;
        return;
    }

    // Check if we already have added this device info
    foreach (BluetoothDeviceInfo *di, m_deviceInfos->deviceInfos()) {
        if (di->address() == deviceInformation->address()) {
            qWarning() << "BluetoothDiscover: device" << deviceInformation->name() << "(" << deviceInformation->address() << ") already added";
            deviceInformation->deleteLater();
            return;
        }
    }

    m_deviceInfos->addBluetoothDeviceInfo(deviceInformation);
}

void BluetoothDiscovery::discoveryFinished()
{
    qDebug() << "BluetoothDiscovery: Discovery finished";
    if (m_discoveryEnabled) {
        qDebug() << "BluetoothDiscovery: Restarting discovery";
        m_discoveryAgent->start();
    }
}

void BluetoothDiscovery::discoveryCancelled()
{
    qDebug() << "BluetoothDiscovery: Discovery cancelled";
}

void BluetoothDiscovery::onError(const QBluetoothDeviceDiscoveryAgent::Error &error)
{
    qWarning() << "BluetoothDiscovery: Discovery error:" << error << m_discoveryAgent->errorString();
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

    m_deviceInfos->clearModel();

    qDebug() << "BluetoothDiscovery: Start discovering.";
    m_discoveryAgent->start();
    emit discoveringChanged();
}

void BluetoothDiscovery::stop()
{
    if (!m_discoveryAgent) {
        return;
    }

    qDebug() << "BluetoothDiscovery: Stop discovering.";
    m_discoveryAgent->stop();
    emit discoveringChanged();
}
