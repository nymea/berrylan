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
    Q_PROPERTY(bool bluetoothEnabled READ bluetoothEnabled NOTIFY bluetoothEnabledChanged)
    Q_PROPERTY(bool discovering READ discovering WRITE setDiscovering NOTIFY discoveringChanged)
    Q_PROPERTY(BluetoothDeviceInfos *deviceInfos READ deviceInfos CONSTANT)

public:
    explicit BluetoothDiscovery(QObject *parent = nullptr);

    bool bluetoothAvailable() const;

    bool bluetoothEnabled() const;
    void setBluetoothEnabled(bool enabled);

    bool discovering() const;
    void setDiscovering(bool discovering);

    BluetoothDeviceInfos *deviceInfos();

private:
    QBluetoothLocalDevice *m_localDevice = nullptr;
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent  = nullptr;
    BluetoothDeviceInfos *m_deviceInfos;

    bool m_enabled = false;
    bool m_bluetoothAvailable = false;
    bool m_bluetoothEnabled = false;

    void setBluetoothAvailable(bool available);
//    void setDiscovering(bool discovering);

signals:
    void bluetoothAvailableChanged(bool bluetoothAvailable);
    void bluetoothEnabledChanged(bool bluetoothEnabled);

    void discoveringChanged();

private slots:
    void onBluetoothHostModeChanged(const QBluetoothLocalDevice::HostMode &hostMode);
    void deviceDiscovered(const QBluetoothDeviceInfo &deviceInfo);
    void discoveryFinished();
    void discoveryCancelled();
    void onError(const QBluetoothDeviceDiscoveryAgent::Error &error);

public slots:
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

};

#endif // BLUETOOTHDISCOVERY_H
