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

#ifndef BLUETOOTHDISCOVERYMOCK_H
#define BLUETOOTHDISCOVERYMOCK_H

#include <QObject>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceDiscoveryAgent>

#include "bluetoothdeviceinfosmock.h"

class BluetoothDiscoveryMock : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool bluetoothAvailable READ bluetoothAvailable NOTIFY bluetoothAvailableChanged)
    Q_PROPERTY(bool bluetoothEnabled READ bluetoothEnabled WRITE setBluetoothEnabled NOTIFY bluetoothEnabledChanged)
    Q_PROPERTY(bool discoveryEnabled READ discoveryEnabled WRITE setDiscoveryEnabled NOTIFY discoveryEnabledChanged)
    Q_PROPERTY(bool discovering READ discovering NOTIFY discoveringChanged)
    Q_PROPERTY(BluetoothDeviceInfosMock *deviceInfos READ deviceInfos CONSTANT)

public:
    explicit BluetoothDiscoveryMock(QObject *parent = nullptr);

    bool bluetoothAvailable() const;
    bool bluetoothEnabled() const;
    void setBluetoothEnabled(bool bluetoothEnabled);

    bool discoveryEnabled() const;
    void setDiscoveryEnabled(bool discoveryEnabled);

    bool discovering() const;

    BluetoothDeviceInfosMock *deviceInfos();

signals:
    void bluetoothAvailableChanged(bool bluetoothAvailable);
    void bluetoothEnabledChanged(bool bluetoothEnabled);
    void discoveryEnabledChanged(bool discoveryEnabled);
    void discoveringChanged();

private:
    BluetoothDeviceInfosMock *m_deviceInfos;

    bool m_bluetoothEnabled = true;
    bool m_discoveryEnabled = false;
};

#endif // BLUETOOTHDISCOVERYMOCK_H
