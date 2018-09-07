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

#ifndef BLUETOOTHDEVICEINFOS_H
#define BLUETOOTHDEVICEINFOS_H

#include <QObject>
#include <QAbstractListModel>

#include "bluetoothdeviceinfo.h"

class BluetoothDeviceInfos : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    enum BluetoothDeviceInfoRole {
        BluetoothDeviceInfoRoleName = Qt::DisplayRole,
        BluetoothDeviceInfoRoleAddress,
        BluetoothDeviceInfoRoleLe
    };

    explicit BluetoothDeviceInfos(QObject *parent = nullptr);

    QList<BluetoothDeviceInfo *> deviceInfos();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    int count() const;
    Q_INVOKABLE BluetoothDeviceInfo *get(int index) const;

    void addBluetoothDeviceInfo(BluetoothDeviceInfo *deviceInfo);
    Q_INVOKABLE void clearModel();

signals:
    void countChanged();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<BluetoothDeviceInfo *> m_deviceInfos;
};

#endif // BLUETOOTHDEVICEINFOS_H
