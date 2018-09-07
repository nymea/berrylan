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

#ifndef NETWORKMANAGERCONTROLLER_H
#define NETWORKMANAGERCONTROLLER_H

#include <QObject>
#include <QBluetoothDeviceInfo>

#include "bluetoothdeviceinfo.h"
#include "wirelesssetupmanager.h"

class NetworkManagerController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(BluetoothDeviceInfo* bluetoothDeviceInfo READ bluetoothDeviceInfo WRITE setBluetoothDeviceInfo)
    Q_PROPERTY(WirelessSetupManager *manager READ manager NOTIFY managerChanged)

public:
    explicit NetworkManagerController(QObject *parent = nullptr);

    BluetoothDeviceInfo* bluetoothDeviceInfo() const;
    void setBluetoothDeviceInfo(BluetoothDeviceInfo* bluetoothDeviceInfo);

    WirelessSetupManager *manager();

    Q_INVOKABLE void connectDevice();

private:
    BluetoothDeviceInfo* m_bluetoothDeviceInfo = nullptr;

    WirelessSetupManager *m_wirelessSetupManager = nullptr;

signals:
    void managerChanged();
    void nameChanged();
    void bluetoothDeviceInfoChanged();

};

#endif // NETWORKMANAGERCONTROLLER_H
