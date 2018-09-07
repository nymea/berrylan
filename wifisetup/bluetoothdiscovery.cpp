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
        setBluetoothAvailable(false);
        return;
    }

    if (localDevice.allDevices().isEmpty()) {
        qWarning() << "BluetoothDiscovery: there is no bluetooth device available currently.";
        setBluetoothAvailable(false);
        return;
    }

    setBluetoothAvailable(true);

    // FIXME: check the device with the most capabilities and check if low energy is available
    QBluetoothHostInfo adapterHostInfo = localDevice.allDevices().first();

    qDebug() << "BluetoothDiscovery: using bluetooth adapter" << adapterHostInfo.name() << adapterHostInfo.address().toString();
    m_localDevice = new QBluetoothLocalDevice(adapterHostInfo.address(), this);
    connect(m_localDevice, &QBluetoothLocalDevice::hostModeStateChanged, this, &BluetoothDiscovery::onBluetoothHostModeChanged);
    onBluetoothHostModeChanged(m_localDevice->hostMode());

    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(m_localDevice->address(), this);
#else
    // Note: on iOS there is no QBluetoothLocalDevice available, therefore we have to assume there is one and
    //       start the discovery agent with the default constructor.
    // https://bugreports.qt.io/browse/QTBUG-65547

    setBluetoothAvailable(true);
    setBluetoothEnabled(true);
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
#endif

    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothDiscovery::deviceDiscovered);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BluetoothDiscovery::discoveryFinished);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &BluetoothDiscovery::discoveryCancelled);
    connect(m_discoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)), this, SLOT(onError(QBluetoothDeviceDiscoveryAgent::Error)));
}

bool BluetoothDiscovery::bluetoothAvailable() const
{
    return m_bluetoothAvailable;
}

bool BluetoothDiscovery::bluetoothEnabled() const
{
    return m_bluetoothEnabled;
}

void BluetoothDiscovery::setBluetoothEnabled(bool enabled)
{
    m_bluetoothEnabled = enabled;
    emit bluetoothEnabledChanged(m_bluetoothEnabled);

    if (!m_localDevice)
        return;

    if (enabled) {
        m_localDevice->powerOn();
    } else {
        m_localDevice->setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    }
}

bool BluetoothDiscovery::discovering() const
{
    return m_discoveryAgent && m_discoveryAgent->isActive();
}

void BluetoothDiscovery::setDiscovering(bool discovering)
{
    if (discovering == this->discovering()) {
        return;
    }

    qDebug() << "setDiscovering:" << discovering;
    if (discovering) {
        start();
    } else {
        stop();
    }
}

BluetoothDeviceInfos *BluetoothDiscovery::deviceInfos()
{
    return m_deviceInfos;
}

void BluetoothDiscovery::setBluetoothAvailable(bool available)
{
    if (m_bluetoothAvailable == available)
        return;

    m_bluetoothAvailable = available;
    emit bluetoothAvailableChanged(m_bluetoothAvailable);
}

void BluetoothDiscovery::onBluetoothHostModeChanged(const QBluetoothLocalDevice::HostMode &hostMode)
{
    qDebug() << "BluetoothDiscovery: host mode changed" << hostMode;
    switch (hostMode) {
    case QBluetoothLocalDevice::HostPoweredOff:
        setBluetoothEnabled(false);
        stop();
        m_deviceInfos->clearModel();
        break;
    default:
        // Note: discovery works in all other modes
        setBluetoothEnabled(true);
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
    if (m_enabled) {
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
    emit discoveringChanged();
}

void BluetoothDiscovery::start()
{
    if (!m_bluetoothEnabled) {
        return;
    }

    m_enabled = true;

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
    m_enabled = false;

    qDebug() << "BluetoothDiscovery: Stop discovering.";
    m_discoveryAgent->stop();
    emit discoveringChanged();
}
