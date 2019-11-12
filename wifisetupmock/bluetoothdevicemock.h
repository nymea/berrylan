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

#ifndef BLUETOOTHDEVICEMOCK_H
#define BLUETOOTHDEVICEMOCK_H

#include <QObject>
#include <QBluetoothUuid>
#include <QLowEnergyService>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyCharacteristic>

class BluetoothDeviceMock : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)

public:
    explicit BluetoothDeviceMock(const QString &name, const QString &address, QObject *parent = nullptr);

    QString name() const;
    QBluetoothAddress address() const;
    QString addressString() const;
    bool connected() const;
    QString statusText() const;

    Q_INVOKABLE void connectDevice();
    Q_INVOKABLE void disconnectDevice();

private:
    QString m_name;
    QString m_address;
    bool m_connected;
    QString m_statusText;

    void setConnected(const bool &connected);

protected:
    void setStatusText(const QString &statusText);

signals:
    void connectedChanged();
    void serviceDiscoveryFinished();

    void statusTextChanged();

};

#endif // BLUETOOTHDEVICE_H
