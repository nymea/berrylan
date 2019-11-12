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

#include "networkmanagercontrollermock.h"

NetworkManagerControllerMock::NetworkManagerControllerMock(QObject *parent) : QObject(parent)
{
}

BluetoothDeviceInfoMock *NetworkManagerControllerMock::bluetoothDeviceInfo() const
{
    return m_bluetoothDeviceInfo;
}

void NetworkManagerControllerMock::setBluetoothDeviceInfo(BluetoothDeviceInfoMock *bluetoothDeviceInfo)
{
    if (m_bluetoothDeviceInfo != bluetoothDeviceInfo) {
        m_bluetoothDeviceInfo = bluetoothDeviceInfo;
        emit bluetoothDeviceInfoChanged();
    }
}

WirelessSetupManagerMock *NetworkManagerControllerMock::manager()
{
    return m_wirelessSetupManager;
}

void NetworkManagerControllerMock::connectDevice()
{
    if (!m_bluetoothDeviceInfo) {
        qWarning() << "Can't connect to device. bluetoothDeviceInfo not set.";
        return;
    }

    if (m_wirelessSetupManager) {
        delete m_wirelessSetupManager;
        m_wirelessSetupManager = nullptr;
        emit managerChanged();
    }

    if (!m_bluetoothDeviceInfo) {
        qDebug() << "Could not connect to device. There is no device info for" << m_bluetoothDeviceInfo->name() << m_bluetoothDeviceInfo->address();
        return;
    }

    m_wirelessSetupManager = new WirelessSetupManagerMock(this);
    emit managerChanged();

    m_wirelessSetupManager->connectDevice();
}
