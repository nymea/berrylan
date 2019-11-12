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

#include "bluetoothdeviceinfosmock.h"

#include <QDebug>

BluetoothDeviceInfosMock::BluetoothDeviceInfosMock(QObject *parent) : QAbstractListModel(parent)
{

}

QList<BluetoothDeviceInfoMock *> BluetoothDeviceInfosMock::deviceInfos()
{
    return m_deviceInfos;
}

int BluetoothDeviceInfosMock::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_deviceInfos.count();
}

QVariant BluetoothDeviceInfosMock::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_deviceInfos.count())
        return QVariant();

    BluetoothDeviceInfoMock *deviceInfo = m_deviceInfos.at(index.row());
    if (role == BluetoothDeviceInfoRoleName) {
        return deviceInfo->name();
    } else if (role == BluetoothDeviceInfoRoleAddress) {
        return deviceInfo->address();
    } else if (role == BluetoothDeviceInfoRoleLe) {
        return deviceInfo->isLowEnergy();
    }

    return QVariant();
}

int BluetoothDeviceInfosMock::count() const
{
    return m_deviceInfos.count();
}

BluetoothDeviceInfoMock *BluetoothDeviceInfosMock::get(int index) const
{
    if (index >= m_deviceInfos.count() || index < 0)
        return Q_NULLPTR;

    return m_deviceInfos.at(index);
}

void BluetoothDeviceInfosMock::addBluetoothDeviceInfo(BluetoothDeviceInfoMock *deviceInfo)
{
    qDebug() << "Adding device" << deviceInfo->name();
    deviceInfo->setParent(this);
    beginInsertRows(QModelIndex(), m_deviceInfos.count(), m_deviceInfos.count());
    m_deviceInfos.append(deviceInfo);
    endInsertRows();
    emit countChanged();
}

void BluetoothDeviceInfosMock::clearModel()
{
    beginResetModel();
    qDeleteAll(m_deviceInfos);
    m_deviceInfos.clear();
    endResetModel();
    emit countChanged();
}

QHash<int, QByteArray> BluetoothDeviceInfosMock::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[BluetoothDeviceInfoRoleName] = "name";
    roles[BluetoothDeviceInfoRoleAddress] = "address";
    roles[BluetoothDeviceInfoRoleLe] = "lowEnergy";
    return roles;
}
