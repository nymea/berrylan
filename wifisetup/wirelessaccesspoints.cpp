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
    switch (role) {
    case WirelessAccesspointRoleSsid:
        return accessPoint->ssid();
    case WirelessAccesspointRoleMacAddress:
        return accessPoint->macAddress();
    case WirelessAccesspointRoleHostAddress:
        return accessPoint->hostAddress();
    case WirelessAccesspointRoleSignalStrength:
        return accessPoint->signalStrength();
    case WirelessAccesspointRoleProtected:
        return accessPoint->isProtected();
    case WirelessAccessPointRoleFrequency:
        return accessPoint->frequency();
    }

    return QVariant();
}

int WirelessAccessPoints::count() const
{
    return m_wirelessAccessPoints.count();
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
    roles[WirelessAccessPointRoleFrequency] = "frequency";
    return roles;
}


