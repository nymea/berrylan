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

#include "wirelessaccesspoints.h"
#include "wirelessaccesspoint.h"

#include <QDebug>

WirelessAccessPoints::WirelessAccessPoints(QObject *parent) : QAbstractListModel(parent)
{

}

QList<WirelessAccessPoint *> WirelessAccessPoints::wirelessAccessPoints()
{
    return m_wirelessAccessPoints;
}

void WirelessAccessPoints::setWirelessAccessPoints(QList<WirelessAccessPoint *> wirelessAccessPoints)
{
    beginResetModel();

    // Delete all
    qDeleteAll(m_wirelessAccessPoints);
    m_wirelessAccessPoints.clear();

    m_wirelessAccessPoints = wirelessAccessPoints;
    endResetModel();

    emit countChanged();
}

int WirelessAccessPoints::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_wirelessAccessPoints.count();
}

QVariant WirelessAccessPoints::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_wirelessAccessPoints.count())
        return QVariant();

    WirelessAccessPoint *accessPoint = m_wirelessAccessPoints.at(index.row());
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

WirelessAccessPoint *WirelessAccessPoints::getAccessPoint(const QString &ssid) const
{
    foreach (WirelessAccessPoint *accessPoint, m_wirelessAccessPoints) {
        if (accessPoint->ssid() == ssid)
            return accessPoint;
    }

    return nullptr;
}

WirelessAccessPoint *WirelessAccessPoints::get(int index)
{
    if (index < 0 || index >= m_wirelessAccessPoints.count()) {
        return nullptr;
    }

    return m_wirelessAccessPoints.at(index);
}

void WirelessAccessPoints::clearModel()
{
    beginResetModel();
    qDeleteAll(m_wirelessAccessPoints);
    m_wirelessAccessPoints.clear();
    endResetModel();
    emit countChanged();
}

void WirelessAccessPoints::addWirelessAccessPoint(WirelessAccessPoint *accessPoint)
{
    accessPoint->setParent(this);

    beginInsertRows(QModelIndex(), m_wirelessAccessPoints.count(), m_wirelessAccessPoints.count());
     qDebug() << "WirelessAccessPoints: access point added" << accessPoint->ssid() << accessPoint->macAddress();
    m_wirelessAccessPoints.append(accessPoint);
    endInsertRows();

    connect(accessPoint, &WirelessAccessPoint::signalStrengthChanged, this, [accessPoint, this]() {
        int idx = m_wirelessAccessPoints.indexOf(accessPoint);
        if (idx < 0) return;
        emit dataChanged(index(idx), index(idx), {WirelessAccesspointRoleSignalStrength});
    });
    connect(accessPoint, &WirelessAccessPoint::hostAddressChanged, this, [accessPoint, this]() {
        int idx = m_wirelessAccessPoints.indexOf(accessPoint);
        if (idx < 0) return;
        emit dataChanged(index(idx), index(idx), {WirelessAccesspointRoleHostAddress});
    });

    emit countChanged();
}

void WirelessAccessPoints::removeWirelessAccessPoint(WirelessAccessPoint *accessPoint)
{
    int index = m_wirelessAccessPoints.indexOf(accessPoint);
    beginRemoveRows(QModelIndex(), index, index);
    qDebug() << "WirelessAccessPoints: access point removed" << accessPoint->ssid() << accessPoint->macAddress();
    m_wirelessAccessPoints.removeAt(index);
    endRemoveRows();

    emit countChanged();
}

QHash<int, QByteArray> WirelessAccessPoints::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[WirelessAccesspointRoleSsid] = "ssid";
    roles[WirelessAccesspointRoleMacAddress] = "macAddress";
    roles[WirelessAccesspointRoleHostAddress] = "hostAddress";
    roles[WirelessAccesspointRoleSignalStrength] = "signalStrength";
    roles[WirelessAccesspointRoleProtected] = "protected";
    return roles;
}


