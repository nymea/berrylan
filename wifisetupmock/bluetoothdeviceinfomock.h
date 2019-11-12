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

#ifndef BLUETOOTHDEVICEINFOMOCK_H
#define BLUETOOTHDEVICEINFOMOCK_H

#include <QList>
#include <QObject>
#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>

class BluetoothDeviceInfoMock : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY deviceChanged)
    Q_PROPERTY(QString address READ address NOTIFY deviceChanged)

public:
    BluetoothDeviceInfoMock();
    BluetoothDeviceInfoMock(const QString &address, const QString &name, bool isLowEnergy);

    QString address() const;
    QString name() const;
    bool isLowEnergy() const;

signals:
    void deviceChanged();

private:
    QString m_address;
    QString m_name;
    bool m_isLowEnergy = true;
};

#endif // BLUETOOTHDEVICEINFO_H
