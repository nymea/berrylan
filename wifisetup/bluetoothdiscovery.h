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

#ifndef BLUETOOTHDISCOVERY_H
#define BLUETOOTHDISCOVERY_H

#include <QObject>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceDiscoveryAgent>

#include "bluetoothdeviceinfos.h"

class BluetoothDiscovery : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool bluetoothAvailable READ bluetoothAvailable NOTIFY bluetoothAvailableChanged)
    Q_PROPERTY(bool bluetoothEnabled READ bluetoothEnabled WRITE setBluetoothEnabled NOTIFY bluetoothEnabledChanged)
    Q_PROPERTY(bool discoveryEnabled READ discoveryEnabled WRITE setDiscoveryEnabled NOTIFY discoveryEnabledChanged)
    Q_PROPERTY(bool discovering READ discovering NOTIFY discoveringChanged)
    Q_PROPERTY(BluetoothDeviceInfos *deviceInfos READ deviceInfos CONSTANT)

public:
    explicit BluetoothDiscovery(QObject *parent = nullptr);

    bool bluetoothAvailable() const;
    bool bluetoothEnabled() const;
    void setBluetoothEnabled(bool bluetoothEnabled);

    bool discoveryEnabled() const;
    void setDiscoveryEnabled(bool discoveryEnabled);

    bool discovering() const;

    BluetoothDeviceInfos *deviceInfos();

signals:
    void bluetoothAvailableChanged(bool bluetoothAvailable);
    void bluetoothEnabledChanged(bool bluetoothEnabled);
    void discoveryEnabledChanged(bool discoveryEnabled);
    void discoveringChanged();

private slots:
    void onBluetoothHostModeChanged(const QBluetoothLocalDevice::HostMode &hostMode);
    void deviceDiscovered(const QBluetoothDeviceInfo &deviceInfo);
    void discoveryFinished();
    void discoveryCancelled();
    void onError(const QBluetoothDeviceDiscoveryAgent::Error &error);

private slots:
    void start();
    void stop();

private:
    QBluetoothLocalDevice *m_localDevice = nullptr;
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent  = nullptr;
    BluetoothDeviceInfos *m_deviceInfos;

    bool m_bluetoothAvailable = false;
#ifdef Q_OS_IOS
    bool m_bluetoothEnabled = false;
#endif
    bool m_discoveryEnabled = false;
};

#endif // BLUETOOTHDISCOVERY_H
