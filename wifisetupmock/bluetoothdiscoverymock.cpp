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

#include "bluetoothdiscoverymock.h"

#include <QDebug>
#include <QTimer>

BluetoothDiscoveryMock::BluetoothDiscoveryMock(QObject *parent) :
    QObject(parent),
    m_deviceInfos(new BluetoothDeviceInfosMock(this))
{
}

bool BluetoothDiscoveryMock::bluetoothAvailable() const
{
    return true;
}

bool BluetoothDiscoveryMock::bluetoothEnabled() const
{
    return true;
}

void BluetoothDiscoveryMock::setBluetoothEnabled(bool bluetoothEnabled) {
    if (m_bluetoothEnabled != bluetoothEnabled) {
        m_bluetoothEnabled = bluetoothEnabled;
        emit bluetoothEnabledChanged(bluetoothEnabled);
    }
}

bool BluetoothDiscoveryMock::discoveryEnabled() const
{
    return m_discoveryEnabled;
}

bool BluetoothDiscoveryMock::discovering() const
{
    return m_discoveryEnabled;
}

void BluetoothDiscoveryMock::setDiscoveryEnabled(bool discoveryEnabled)
{
    if (m_discoveryEnabled == discoveryEnabled) {
        return;
    }
    m_discoveryEnabled = discoveryEnabled;
    emit discoveringChanged();

    QTimer::singleShot(1000, this, [this](){
        m_deviceInfos->addBluetoothDeviceInfo(new BluetoothDeviceInfoMock("11:22:33:44:55:66", "BT-WLAN-Setup", true));
    });
}

BluetoothDeviceInfosMock *BluetoothDiscoveryMock::deviceInfos()
{
    return m_deviceInfos;
}
