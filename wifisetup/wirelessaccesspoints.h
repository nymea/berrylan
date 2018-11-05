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

#ifndef WIRELESSACCESSPOINTS_H
#define WIRELESSACCESSPOINTS_H

#include <QObject>
#include <QAbstractListModel>

class WirelessAccessPoint;

class WirelessAccessPoints : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum BluetoothDeviceInfoRole {
        WirelessAccesspointRoleSsid = Qt::DisplayRole,
        WirelessAccesspointRoleMacAddress,
        WirelessAccesspointRoleHostAddress,
        WirelessAccesspointRoleSignalStrength,
        WirelessAccesspointRoleProtected,
        WirelessAccesspointRoleSelectedNetwork
    };

    explicit WirelessAccessPoints(QObject *parent = nullptr);

    QList<WirelessAccessPoint *> wirelessAccessPoints();
    void setWirelessAccessPoints(QList<WirelessAccessPoint *> wirelessAccessPoints);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE WirelessAccessPoint *getAccessPoint(const QString &ssid) const;
    Q_INVOKABLE WirelessAccessPoint *get(int index);

    void clearModel();

    void addWirelessAccessPoint(WirelessAccessPoint *accessPoint);
    void removeWirelessAccessPoint(WirelessAccessPoint *accessPoint);

signals:
    void countChanged();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<WirelessAccessPoint *> m_wirelessAccessPoints;


};

#endif // WIRELESSACCESSPOINTS_H
