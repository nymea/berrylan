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

#include "wirelessaccesspointsmock.h"
#include "wirelessaccesspointmock.h"

#include <QDebug>

WirelessAccessPointsMock::WirelessAccessPointsMock(QObject *parent) : QAbstractListModel(parent)
{

}

QList<WirelessAccessPointMock *> WirelessAccessPointsMock::wirelessAccessPoints()
{
    return m_wirelessAccessPoints;
}

void WirelessAccessPointsMock::setWirelessAccessPoints(QList<WirelessAccessPointMock *> wirelessAccessPoints)
{
    beginResetModel();

    // Delete all
    qDeleteAll(m_wirelessAccessPoints);
    m_wirelessAccessPoints.clear();

    m_wirelessAccessPoints = wirelessAccessPoints;
    endResetModel();

    emit countChanged();
}

int WirelessAccessPointsMock::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_wirelessAccessPoints.count();
}

QVariant WirelessAccessPointsMock::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_wirelessAccessPoints.count())
        return QVariant();

    WirelessAccessPointMock *accessPoint = m_wirelessAccessPoints.at(index.row());
    if (role == WirelessAccesspointRoleSsid) {
        return accessPoint->ssid();
    } else if (role == WirelessAccesspointRoleMacAddress) {
        return accessPoint->macAddress();
    } else if (role == WirelessAccesspointRoleHostAddress) {
        return accessPoint->hostAddress();
    }  else if (role == WirelessAccesspointRoleSignalStrength) {
        return accessPoint->signalStrength();
    } else if (role == WirelessAccesspointRoleProtected) {
        return accessPoint->isProtected();
    }

    return QVariant();
}

WirelessAccessPointMock *WirelessAccessPointsMock::getAccessPoint(const QString &ssid) const
{
    foreach (WirelessAccessPointMock *accessPoint, m_wirelessAccessPoints) {
        if (accessPoint->ssid() == ssid)
            return accessPoint;
    }

    return nullptr;
}

WirelessAccessPointMock *WirelessAccessPointsMock::get(int index)
{
    if (index < 0 || index >= m_wirelessAccessPoints.count()) {
        return nullptr;
    }

    return m_wirelessAccessPoints.at(index);
}

void WirelessAccessPointsMock::clearModel()
{
    beginResetModel();
    qDeleteAll(m_wirelessAccessPoints);
    m_wirelessAccessPoints.clear();
    endResetModel();
    emit countChanged();
}

void WirelessAccessPointsMock::addWirelessAccessPoint(WirelessAccessPointMock *accessPoint)
{
    accessPoint->setParent(this);

    beginInsertRows(QModelIndex(), m_wirelessAccessPoints.count(), m_wirelessAccessPoints.count());
     qDebug() << "WirelessAccessPoints: access point added" << accessPoint->ssid() << accessPoint->macAddress();
    m_wirelessAccessPoints.append(accessPoint);
    endInsertRows();

    connect(accessPoint, &WirelessAccessPointMock::signalStrengthChanged, this, [accessPoint, this]() {
        int idx = m_wirelessAccessPoints.indexOf(accessPoint);
        if (idx < 0) return;
        emit dataChanged(index(idx), index(idx), {WirelessAccesspointRoleSignalStrength});
    });
    connect(accessPoint, &WirelessAccessPointMock::hostAddressChanged, this, [accessPoint, this]() {
        int idx = m_wirelessAccessPoints.indexOf(accessPoint);
        if (idx < 0) return;
        emit dataChanged(index(idx), index(idx), {WirelessAccesspointRoleHostAddress});
    });

    emit countChanged();
}

void WirelessAccessPointsMock::removeWirelessAccessPoint(WirelessAccessPointMock *accessPoint)
{
    int index = m_wirelessAccessPoints.indexOf(accessPoint);
    beginRemoveRows(QModelIndex(), index, index);
    qDebug() << "WirelessAccessPoints: access point removed" << accessPoint->ssid() << accessPoint->macAddress();
    m_wirelessAccessPoints.removeAt(index);
    endRemoveRows();

    emit countChanged();
}

QHash<int, QByteArray> WirelessAccessPointsMock::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[WirelessAccesspointRoleSsid] = "ssid";
    roles[WirelessAccesspointRoleMacAddress] = "macAddress";
    roles[WirelessAccesspointRoleHostAddress] = "hostAddress";
    roles[WirelessAccesspointRoleSignalStrength] = "signalStrength";
    roles[WirelessAccesspointRoleProtected] = "protected";
    return roles;
}


