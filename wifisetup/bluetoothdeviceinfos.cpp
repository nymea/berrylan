/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
* This project including source code and documentation is protected by
* copyright law, and remains the property of nymea GmbH. All rights, including
* reproduction, publication, editing and translation, are reserved. The use of
* this project is subject to the terms of a license agreement to be concluded
* with nymea GmbH in accordance with the terms of use of nymea GmbH, available
* under https://nymea.io/license
*
* GNU General Public License Usage
* Alternatively, this project may be redistributed and/or modified under the
* terms of the GNU General Public License as published by the Free Software
* Foundation, GNU version 3. This project is distributed in the hope that it
* will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
* Public License for more details.
*
* You should have received a copy of the GNU General Public License along with
* this project. If not, see <https://www.gnu.org/licenses/>.
*
* For any further details and any questions please contact us under
* contact@nymea.io or see our FAQ/Licensing Information on
* https://nymea.io/license/faq
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "bluetoothdeviceinfos.h"

#include <QDebug>

BluetoothDeviceInfos::BluetoothDeviceInfos(QObject *parent) : QAbstractListModel(parent)
{

}

QList<BluetoothDeviceInfo *> BluetoothDeviceInfos::deviceInfos()
{
    return m_deviceInfos;
}

int BluetoothDeviceInfos::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_deviceInfos.count();
}

QVariant BluetoothDeviceInfos::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_deviceInfos.count())
        return QVariant();

    BluetoothDeviceInfo *deviceInfo = m_deviceInfos.at(index.row());
    if (role == BluetoothDeviceInfoRoleName) {
        return deviceInfo->name();
    } else if (role == BluetoothDeviceInfoRoleAddress) {
        return deviceInfo->address();
    } else if (role == BluetoothDeviceInfoRoleLe) {
        return deviceInfo->isLowEnergy();
    } else if (role == BluetoothDeviceInfoRoleSignalStrength) {
        return deviceInfo->signalStrength();
    }

    return QVariant();
}

int BluetoothDeviceInfos::count() const
{
    return m_deviceInfos.count();
}

BluetoothDeviceInfo *BluetoothDeviceInfos::get(int index) const
{
    if (index >= m_deviceInfos.count() || index < 0)
        return Q_NULLPTR;

    return m_deviceInfos.at(index);
}

void BluetoothDeviceInfos::addBluetoothDeviceInfo(BluetoothDeviceInfo *deviceInfo)
{
    qDebug() << "Adding device" << deviceInfo->name();
    deviceInfo->setParent(this);
    beginInsertRows(QModelIndex(), m_deviceInfos.count(), m_deviceInfos.count());
    m_deviceInfos.append(deviceInfo);
    endInsertRows();
    emit countChanged();
}

void BluetoothDeviceInfos::clearModel()
{
    beginResetModel();
    qDeleteAll(m_deviceInfos);
    m_deviceInfos.clear();
    endResetModel();
    emit countChanged();
}

QHash<int, QByteArray> BluetoothDeviceInfos::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[BluetoothDeviceInfoRoleName] = "name";
    roles[BluetoothDeviceInfoRoleAddress] = "address";
    roles[BluetoothDeviceInfoRoleLe] = "lowEnergy";
    roles[BluetoothDeviceInfoRoleSignalStrength] = "signalStrength";
    return roles;
}
