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

#ifndef NETWORKMANAGERCONTROLLERMOCK_H
#define NETWORKMANAGERCONTROLLERMOCK_H

#include <QObject>
#include <QBluetoothDeviceInfo>

#include "bluetoothdeviceinfomock.h"
#include "wirelesssetupmanagermock.h"

class NetworkManagerControllerMock : public QObject
{
    Q_OBJECT
    Q_PROPERTY(BluetoothDeviceInfoMock* bluetoothDeviceInfo READ bluetoothDeviceInfo WRITE setBluetoothDeviceInfo)
    Q_PROPERTY(WirelessSetupManagerMock *manager READ manager NOTIFY managerChanged)

public:
    explicit NetworkManagerControllerMock(QObject *parent = nullptr);

    BluetoothDeviceInfoMock* bluetoothDeviceInfo() const;
    void setBluetoothDeviceInfo(BluetoothDeviceInfoMock* bluetoothDeviceInfo);

    WirelessSetupManagerMock *manager();

    Q_INVOKABLE void connectDevice();

private:
    BluetoothDeviceInfoMock *m_bluetoothDeviceInfo = nullptr;

    WirelessSetupManagerMock *m_wirelessSetupManager = nullptr;

signals:
    void managerChanged();
    void nameChanged();
    void bluetoothDeviceInfoChanged();

};

#endif // NETWORKMANAGERCONTROLLERMOCK_H
